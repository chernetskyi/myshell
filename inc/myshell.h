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


class MyShell {
private:
    int erno = 0;

    std::string prompt = " # ";
    char *current_dir = new char[MAX_PATH_LEN];

    char **envp;

    std::map<std::string, builtin> builtins_map;

    void initialize_builtins();

    builtin builtins(const std::string &command);

    void fork_exec(char *proc,char **margv);

public:
    explicit MyShell(char *envp[]);

    void start();

    void execute(const std::string &input);

    ~MyShell();
};

constexpr const char command_not_found_error[] = "myshell: commnad not found: ";
constexpr const char could_not_create_process_error[] = "myshell: could not create process: ";



#endif //MYSHELL_MYSHELL_H
