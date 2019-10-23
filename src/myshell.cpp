#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
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
    export_env = env;
    initialize_builtins();
}

void MyShell::execute(std::string &input) {
    std::vector<char *> args;
    process(input, args);
    if (builtins_map.find(args[0]) != builtins_map.end()) {
        builtin func = builtins_map[args[0]];
        erno = func(args.size(), args.data(), export_env.data());
    } else
        fork_exec(args[0], args.data());
}

void MyShell::process(std::string &input, std::vector<char *> &args) {
    std::string string_input = std::string(input), filtered_input;
    boost::trim(string_input);
    std::unique_copy(string_input.begin(), string_input.end(), std::back_insert_iterator<std::string>(filtered_input),
                     [](char a, char b) { return isspace(a) && isspace(b); });
    std::vector<std::string> splits;
    boost::split(splits, filtered_input, boost::is_space());
    for (auto &split : splits)
        args.push_back(const_cast<char *>(split.c_str()));
}

void MyShell::fork_exec(char *proc, char **args) {
    int pid = fork();
    if (pid == -1) {
        std::cerr << could_not_create_process_error << proc << std::endl;
        erno = 1;
    } else if (!pid) {
        int res = execve(proc, args, export_env.data());
        if (res == -1) {
            std::cerr << command_not_found_error << proc << std::endl;
            erno = 1;
        }
    } else
        waitpid(pid, &erno, 0);
}

std::string MyShell::prompt() {
    std::string PS1 = "\n%D\n>>> ";
    std::string prompt = PS1;
    size_t start_pos = 0;
    if ((start_pos = PS1.find("%D")) != std::string::npos) {
        auto workind_dir = boost::filesystem::current_path();
        prompt.insert(start_pos, workind_dir.string());
        prompt.erase(start_pos + workind_dir.size(), 2);
    }
    return prompt;
}

void MyShell::initialize_builtins() {
    builtins_map["mexit"] = &mexit;
    builtins_map["merrno"] = [this](int argc, char **argv, char **envp) { return merrno(argc, argv, envp, erno); };
    builtins_map["mpwd"] = &mpwd;
    builtins_map["mcd"] = &mcd;
}

void MyShell::start() {
    char *input_buff = nullptr;
    while ((input_buff = readline(prompt().c_str())) != nullptr) {
        if (strlen(input_buff) == 0)
            continue;
        else if ((strlen(input_buff) > 0) && !isspace(input_buff[0]))
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
