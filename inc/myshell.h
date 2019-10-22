#ifndef MYSHELL_MYSHELL_H
#define MYSHELL_MYSHELL_H

#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unistd.h>
#include <limits>

#include "builtins.h"

#define MAX_PATH_LEN 4096

std::string normalize_input(char *input);

class MyShell {
private:
    int erno = 0;
    char *buff = nullptr;
    std::string prompt = " # ";
    char *current_dir = new char[MAX_PATH_LEN];
    char **envp;

    std::map<std::string, builtin> builtins_map;

    void initialize_builtins();

    builtin builtins(const std::string &command);

public:
    explicit MyShell(char *envp[]);

    void start();

    void execute(const std::string &input);

    ~MyShell();
};

constexpr const char command_not_found_error[] = "myshell: command not found: ";


#endif //MYSHELL_MYSHELL_H
