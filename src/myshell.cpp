#include <cctype>
#include <iostream>
#include <string>
#include <unistd.h>
#include <map>
#include <vector>
#include <map>

#include <sys/wait.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <readline/readline.h>
#include <readline/history.h>

#include "builtins.h"
#include "helpers.h"
#include "myshell.h"

MyShell::MyShell(char *envp[]) : envp(envp) {
    getcwd(current_dir, MAX_PATH_LEN);
    initialize_builtins();
}

void MyShell::execute(const std::string &input) {
    std::vector<std::string> splits;
    std::vector<char *> c_splits;
    std::string process;
    char **margv;

    boost::split(splits, input, boost::is_space());
    c_splits.reserve(splits.size());

    preprocess_line(splits, c_splits);

    margv = get_args(c_splits);
    process = splits[0];

    if (builtins(process) != nullptr) {
        builtin func = builtins(process);
        erno = func(splits.size(), margv, envp);
    } else {
        fork_exec(c_splits[0], margv);
    }
    delete[] margv;
}


void MyShell::fork_exec(char *proc, char **args) {
    int pid = fork();
    if (pid == -1) {
        std::cerr << could_not_create_process_error << proc << std::endl;
        erno = 1;
    } else if (!pid) {
        int res = execvp(proc, args);
        if (res == -1) {
            std::cerr << command_not_found_error << proc << std::endl;
            erno = 1;
        }
    } else {
        waitpid(pid, nullptr, 0);
    }

};

void MyShell::start() {
    char *buff = nullptr;
    while ((buff = readline((current_dir + prompt).c_str())) != nullptr) {
        if ((strlen(buff) > 0) && !isspace(buff[0]))
            add_history(buff);
        std::string user_input = normalize_input(buff);
        free(buff);
        execute(user_input);
    }
};


builtin MyShell::builtins(const std::string &command) {
    return builtins_map.find(command) != builtins_map.end() ? builtins_map[command] : nullptr;
}

void MyShell::initialize_builtins() {
    builtins_map["mexit"] = &mexit;
    builtins_map["merrno"] = [this](int argc, char **argv, char **envp) { return merrno(argc, argv, envp, erno); };
    builtins_map["mpwd"] = [this](int argc, char **argv, char **envp) { return mpwd(argc, argv, envp, current_dir); };
    builtins_map["mcd"] = [this](int argc, char **argv, char **envp) { return mcd(argc, argv, envp, current_dir); };
}

MyShell::~MyShell() {
    delete[] current_dir;
}


int main(int argc, char *argv[], char *envp[]) {
    MyShell shell = MyShell(envp);
    shell.start();
    exit(0);
}
