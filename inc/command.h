#ifndef MYSHELL_COMMAND_H
#define MYSHELL_COMMAND_H

#include <map>

#include "builtins.h"

class Command {
private:
    int err_fd;
    bool background;
    int argc;
    char **envp;
    std::map<std::string, builtin> builtins_map;

    int fork_exec();

public:
    Command(int in_fd, int out_fd, int err_fd, bool background, int argc, std::vector<const char *> &argv, char **envp,
            std::map<std::string, builtin> builtins_map);

    int execute();

    int in_fd;
    int out_fd;
    std::vector<const char *> argv;
};

constexpr const char command_not_found_error[] = "myshell: command not found: ";
constexpr const char could_not_create_process_error[] = "myshell: could not create process: ";

#endif //MYSHELL_COMMAND_H
