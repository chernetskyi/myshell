#include <iostream>
#include <map>
#include <string>
#include <sys/wait.h>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "builtins.h"
#include "command.h"

Command::Command(int in_fd, int out_fd, int err_fd, bool background, int argc, std::vector<const char *> &argv, char **envp,
                 std::map<std::string, builtin> builtins_map)
        : in_fd(in_fd), out_fd(out_fd), err_fd(err_fd), background(background), argc(argc), argv(argv), envp(envp),
          builtins_map(std::move(builtins_map)) {}

int Command::execute() {
//    if (std::string(argv0]).find('=') != std::string::npos) {
//        if (argc == 1) {
//            std::vector<std::string> splits;
//            std::string st = std::string(argv[0]);
//            boost::split(splits, st , boost::is_any_of("="));
//            setenv(splits[0].c_str(), splits[1].c_str(), 1);
//            return 0;
//        } else {
//            std::cerr << command_not_found_error << argv[1] << std::endl;
//            return 1;
//        }
//    } else if (builtins_map.find(argv[0]) != builtins_map.end()) {
//        builtin func = builtins_map[argv[0]];
//        return func(argc, alsrgv, envp);
//    } else {
    return fork_exec();
//    }
}


int Command::fork_exec() {
    std::cout << in_fd << " " << out_fd << " " << "argv" << argv.data()[0] << std::endl;
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << could_not_create_process_error << argv.data()[0] << std::endl;
        return 1;
    } else if (!pid) {
#ifdef _GNU_SOURCE
        int res = execvpe(argv[0], argv, envp);
#else


        if (out_fd != 0) {
            dup2(in_fd, 0);
            close(in_fd);
        }

        if (out_fd != 1) {
            dup2(out_fd, 1);
            close(out_fd);
        }

        int res = execvp(argv.data()[0], argv.data());


#endif
        if (res == -1) {
            std::cerr << command_not_found_error << argv[0] << std::endl;
            return 1;
        }
    } else {
        int res;
        if (out_fd != 1)
            close(out_fd);
        if (in_fd != 0)
            close(in_fd);



        std::cout << "ended" << std::endl;
        return pid;
    }
}
