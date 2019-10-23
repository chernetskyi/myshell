#ifndef MYSHELL_MYSHELL_H
#define MYSHELL_MYSHELL_H

#include <string>
#include "builtins.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>
#include <string>
#include <map>


class MyShell {
private:
    int erno = 0;
    const char *prompt = "# ";
    std::string current_dir = "/";
    char **envp;

    std::map<std::string, builtin> builtins_map;

    void initialize_builtins();

    builtin builtins(const std::string &command);

    void fork_exec(char *proc,char **margv);

public:
    explicit MyShell(char *envp[]) : envp(envp) {
        initialize_builtins();
    }


    void start();

    void execute(const std::string &input);
};

constexpr const char command_not_found_error[] = "myshell: commnad not found: ";
constexpr const char could_not_create_process_error[] = "myshell: could not create process: ";


#endif //MYSHELL_MYSHELL_H
