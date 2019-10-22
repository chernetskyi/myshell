#ifndef MYSHELL_MYSHELL_H
#define MYSHELL_MYSHELL_H

#include <string>
#include "builtins.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>
#include <map>

std::string normalize_input(char *input);

class MyShell {
private:
    int erno = 0;
    char *buff = nullptr;
    const char *prompt = "# ";
    char **envp;

    std::map<std::string, builtin> builtins_map;

    void initialize_builtins();

    builtin builtins(const std::string &command);

public:
    explicit MyShell(char *envp[]) : envp(envp) {
        initialize_builtins();
    }


    void start();

    void execute(const std::string &input);
};

constexpr const char command_not_found_error[] = "myshell: commnad not found: ";


#endif //MYSHELL_MYSHELL_H
