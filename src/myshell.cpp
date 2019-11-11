#include <cctype>
#include <string>
#include <unistd.h>
#include <vector>

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
    builtins_map["mexport"] = &mexport;
    builtins_map["."] = &dotbuiltin;
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
        std::vector<std::string> args;
        process(user_input, args);
        std::vector<char *> cargs;
        for (auto &arg : args)
            cargs.push_back(const_cast<char *>(arg.c_str()));
        cargs.push_back(nullptr);
        Command command{0, 1, 2, false, static_cast<int>(cargs.size() - 1), cargs.data(), env.data(), builtins_map};
        erno = command.execute();
    }
}

int main(int argc, char *argv[], char *envp[]) {
    MyShell shell = MyShell(envp);
    shell.start();
    exit(0);
}
