#ifndef MYSHELL_MYSHELL_H
#define MYSHELL_MYSHELL_H

#include <map>
#include <string>
#include <vector>

#include "builtins.h"

class MyShell {
private:
    int erno;
    std::vector<char *> env;
    std::vector<char *> export_env;
    std::map<std::string, builtin> builtins_map;

    void initialize_builtins();

    void execute(std::string &input);

    void process(std::string &input, std::vector<char *> &args);

    void fork_exec(char *proc, char **margv);

    std::string prompt();

public:
    explicit MyShell(char *envp[]);

    void start();
};

constexpr const char command_not_found_error[] = "myshell: command not found: ";
constexpr const char could_not_create_process_error[] = "myshell: could not create process: ";

#endif //MYSHELL_MYSHELL_H
