#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <readline/readline.h>
#include <readline/history.h>

#include "builtins.h"
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

void MyShell::execute(std::string &input) {
    std::vector<std::string> args;
    process(input, args);
    if (std::string(args[0]).find('=') != std::string::npos)
        if (args.size() == 1) {
            std::vector<std::string> splits;
            boost::split(splits, std::string(args[0]), boost::is_any_of("="));
            setenv(splits[0].c_str(), splits[1].c_str(), 1);
        } else {
            std::cerr << command_not_found_error << args[1] << std::endl;
            erno = 1;
        }
    else {
        std::vector<char *> cargs;
        for (auto &arg : args)
            cargs.push_back(const_cast<char *>(arg.c_str()));
        if (builtins_map.find(args[0]) != builtins_map.end()) {
            builtin func = builtins_map[args[0]];
            erno = func(args.size(), cargs.data(), env.data());
        } else
            fork_exec(cargs[0], cargs.data());
    }
}

void MyShell::process(std::string &input, std::vector<std::string> &args) {
    boost::trim(input);
    int begin = -1;
    bool double_quotes = false;
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '"') {
            if (double_quotes) {
                args.push_back(input.substr(begin, i - begin));
                begin = -1;
            } else {
                if (i != input.size() - 1)
                    begin = i + 1;
                double_quotes = true;
            }
        } else if (input[i] == ' ') {
            if ((begin != -1) && !double_quotes) {
                args.push_back(input.substr(begin, i - begin));
                begin = -1;
            }
        } else if (begin == -1)
            begin = i;
    }
    if (begin != -1)
        args.push_back(input.substr(begin, input.size() - begin));
}

void MyShell::fork_exec(char *proc, char **args) {
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << could_not_create_process_error << proc << std::endl;
        erno = 1;
    } else if (!pid) {
#ifdef _GNU_SOURCE
        int res = execvpe(proc, args, env.data());
#else
        int res = execve(proc, args, env.data());
#endif
        if (res == -1) {
            std::cerr << command_not_found_error << proc << std::endl;
            erno = 1;
        }
    } else
        waitpid(pid, &erno, 0);
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
}

void MyShell::start() {
    setenv("PS1", "\n\e[1;34m%D\e[m\n>>> ", 1);
    char *input_buff = nullptr;
    while ((input_buff = readline(prompt().c_str())) != nullptr) {
        if (strlen(input_buff) == 0)
            continue;
        else if (!isspace(input_buff[0]))
            add_history(input_buff);
        std::string user_input = input_buff;
        free(input_buff);
        execute(user_input);
    }
}

int main(int argc, char *argv[], char *envp[]) {
    MyShell shell = MyShell(envp);
    shell.start();
    exit(0);
}
