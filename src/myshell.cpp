#include <cctype>
#include <glob.h>
#include <iostream>
#include <libgen.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <readline/readline.h>
#include <readline/history.h>

#include "builtins.h"
#include "command.h"
#include "myshell.h"

MyShell::MyShell(char *envp[]) : erno(0) {
    size_t i = 0;
    while (envp[i] != nullptr) {
        env.push_back(envp[i]);
        i++;
    }
    env.push_back(nullptr);
    initialize_builtins();
}

std::vector<const char *> MyShell::parse(std::string &input) {
    std::vector<const char *> args;
    char *buffer;
    boost::trim(input);

    std::string sub;
    int begin = -1;
    bool double_quotes = false;
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '"') {
            if (double_quotes) {
                //we need extra char for NUL
                sub = input.substr(begin, i - begin);
                buffer = new char[sub.size()];
                memcpy(buffer, sub.c_str(), sub.size() + 1);
                args.push_back(buffer);
                begin = -1;
            } else {
                if (i != input.size() - 1)
                    begin = i + 1;
                double_quotes = true;
            }
        } else if (input[i] == ' ') {
            if ((begin != -1) && !double_quotes) {
                sub = input.substr(begin, i - begin);
                buffer = new char[sub.size()];
                memcpy(buffer, sub.c_str(), sub.size() + 1);
                args.push_back(buffer);
                begin = -1;
            }
        } else if (begin == -1) {
            begin = i;
        }
    }
    if (begin != -1) {
        sub = input.substr(begin, input.size() - begin);
        buffer = new char[sub.size()];
        memcpy(buffer, sub.c_str(), sub.size() + 1);
        args.push_back(buffer);
    }

    for (auto &arg : args)
        if (arg[0] == '$') {
            const char *value = getenv(arg + 1);
            arg = value == nullptr ? "" : value;
        }

    for (size_t i = 1; i < args.size(); ++i) {
        std::string tmp(basename(const_cast<char *>(args[i])));
        if ((tmp.find('*') != std::string::npos) ||
            (tmp.find('?') != std::string::npos) ||
            ((tmp.find('[') != std::string::npos) && (tmp.find(']') != std::string::npos) &&
             (tmp.find('[') < tmp.find(']')))) {
            glob_t glob_result;
            memset(&glob_result, 0, sizeof(glob_result));
            int return_value = glob(tmp.c_str(), GLOB_TILDE, nullptr, &glob_result);
            if (return_value == 0) {
                args.erase(args.begin() + i);
                for (size_t j = 0; j < glob_result.gl_pathc; ++j) {
                    size_t size = strlen(glob_result.gl_pathv[j]) + 1;
                    char *buff = new char[size];
                    memcpy(buff, glob_result.gl_pathv[j], size);
                    args.insert(args.begin() + i, buff);
                }
                i += glob_result.gl_pathc;
            }
            globfree(&glob_result);
        }
    }

    args.push_back(nullptr);
    return args;
}

std::string MyShell::prompt() {
    std::string prompt(getenv("PS1"));
    size_t start_pos = 0;
    if ((start_pos = prompt.find("%D")) != std::string::npos) {
        auto working_dir = boost::filesystem::current_path();
        prompt.insert(start_pos, working_dir.string());
        prompt.erase(start_pos + working_dir.size(), 2);
    }
    return prompt;
}

void MyShell::initialize_builtins() {
    builtins_map["mexit"] = &mexit;
    builtins_map["merrno"] = [this](int argc, char **argv, char **envp) { return merrno(argc, argv, envp, erno); };
    builtins_map["mpwd"] = &mpwd;
    builtins_map["mcd"] = &mcd;
    builtins_map["mecho"] = &mecho;
    builtins_map["mexport"] = [this](int argc, char **argv, char **envp) { return mexport(argc, argv, envp, env); };
    builtins_map["."] = [this](int argc, char **argv, char **envp) { return dotbuiltin(argc, argv, envp, this); };
}

void MyShell::start() {
    setenv("PS1", "\n%D\n➜ ", 1);
    char *input_buff = nullptr;
    while ((input_buff = readline(prompt().c_str())) != nullptr) {
        if (strlen(input_buff) == 0) {
            continue;
        }
        if (!isspace(input_buff[0])) {
            add_history(input_buff);
        }
        std::string user_input = input_buff;

        std::vector<std::string> pipe_buss;
        boost::split(pipe_buss, user_input, boost::is_any_of("|"));

        int in = STDIN_FILENO;
        int j = 0;
        int pid = 0;
        for (auto &cmnd: pipe_buss) {
            int pipe_fd[2];
            if (pipe(pipe_fd) != 0) {
                std::cerr << pipe_error_message << std::endl;
                erno = 1;
                break;
            }
            if (j++ == pipe_buss.size() - 1)
                pipe_fd[1] = STDOUT_FILENO;
            auto args = parse(cmnd);
            bool bckgrnd = false;
            if (strcmp(args[args.size() - 2], "&") == 0) {
                args.erase(args.end() - 2);
                bckgrnd = true;
            }
            Command item = Command(in, pipe_fd[1], STDERR_FILENO, bckgrnd, static_cast<int>(args.size() - 1),
                                   args, env.data(), builtins_map);
            pid = item.execute();
            in = pipe_fd[0];
        }
        waitpid(pid, nullptr, 0);
    }
}

int MyShell::run_script(char *scriptname) {
    std::ifstream script(scriptname);
    std::string line;
    while (getline(script, line)) {
        if (line[0] != '#') {
            std::vector<std::string> pipe_buss;
            boost::split(pipe_buss, line, boost::is_any_of("|"));

            int in = STDIN_FILENO;
            int j = 0;
            int pid = 0;
            for (auto &cmnd: pipe_buss) {
                int pipe_fd[2];
                if (pipe(pipe_fd) != 0) {
                    std::cerr << pipe_error_message << std::endl;
                    erno = 1;
                    break;
                }
                if (j++ == pipe_buss.size() - 1)
                    pipe_fd[1] = STDOUT_FILENO;
                auto args = parse(cmnd);
                bool bckgrnd = false;
                if (strcmp(args[args.size() - 2], "&") == 0) {
                    args.erase(args.end() - 2);
                    bckgrnd = true;
                }
                Command item = Command(in, pipe_fd[1], STDERR_FILENO, bckgrnd, static_cast<int>(args.size() - 1),
                                       args, env.data(), builtins_map);
                pid = item.execute();
                in = pipe_fd[0];
            }
            waitpid(pid, nullptr, 0);
        }
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[], char *envp[]) {
    MyShell shell = MyShell(envp);
    if (argc == 1) {
        shell.start();
    } else {
        return shell.run_script(argv[1]);
    }
}
