#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <map>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <readline/readline.h>
#include <readline/history.h>


#include <sys/wait.h>

#include "builtins.h"
#include "myshell.h"
#include "helpers.h"



void MyShell::execute(const std::string &input) {
    std::vector<std::string> splits;
    std::vector<char *> c_splits;
    c_splits.reserve(splits.size());

    boost::split(splits, input, boost::is_space());
    preprocess_line(splits, c_splits);

    if (builtins(splits[0]) != nullptr) {
        builtin func = builtins(splits[0]);
        char **margv = get_args(c_splits);
        erno = func(splits.size(), margv, envp);
        delete[] margv;
    } else {
        fork_exec(c_splits[0], get_args(c_splits));
    }
}


void MyShell::fork_exec(char *proc, char **args) {
    int pid = fork();
    if (pid == -1) {
        exit(1);
    } else if (!pid) {
        int res = execvp(proc, args);
        if (res == -1) {
            std::cerr << command_not_found_error << proc << std::endl;
            erno = 1;
        }
    } else {
        wait(nullptr);
    }

};

void MyShell::start() {
    char *buff = nullptr;
    while ((buff = readline(prompt)) != nullptr) {
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
    builtins_map["mpwd"] = [this](int argc, char **argv, char **envp) { return mpwd(argc, argv, envp, &current_dir); };
    builtins_map["mcd"] = [this](int argc, char **argv, char **envp) { return mcd(argc, argv, envp, &current_dir); };
}


int main(int argc, char *argv[], char *envp[]) {

    MyShell shell = MyShell(envp);
    shell.start();
    exit(0);
}
