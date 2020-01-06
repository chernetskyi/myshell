#include <iostream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include <boost/algorithm/string.hpp>

#include "builtins.h"
#include "myshell.h"

int mexit(int argc, char *argv[], char *envp[]) {
    switch (argc) {
        case 1:
            exit(EXIT_SUCCESS);
        case 2:
            if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
                std::cout << mexit_help_message << std::endl;
                return EXIT_SUCCESS;
            } else {
                try {
                    return boost::lexical_cast<int>(argv[1]);
                } catch (boost::bad_lexical_cast &) {
                    std::cerr << mexit_not_numeric_error_message << std::endl;
                    return EXIT_FAILURE;
                }
            }
        default:
            std::cerr << mexit_too_many_args_error_message << std::endl;
            return EXIT_FAILURE;
    }
}

int mpwd(int argc, char *argv[], char *envp[]) {
    switch (argc) {
        case 1:
            std::cout << boost::filesystem::current_path().string() << std::endl;
            return EXIT_SUCCESS;
        case 2:
            if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
                std::cout << mpwd_help_message << std::endl;
                return EXIT_SUCCESS;
            } else
                default:
                    std::cerr << mpwd_too_many_args_error_message << std::endl;
            return EXIT_FAILURE;
    }
}

int mcd(int argc, char *argv[], char *envp[]) {
    switch (argc) {
        case 1:
            boost::filesystem::current_path(getenv("HOME"));
            return EXIT_FAILURE;
        case 2:
            if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
                std::cout << mcd_help_message << std::endl;
                return EXIT_SUCCESS;
            } else {
                try {
                    boost::filesystem::current_path(argv[1]);
                    return EXIT_SUCCESS;
                } catch (boost::filesystem::filesystem_error &) {
                    std::cerr << mcd_no_such_dir_error_message << argv[1] << std::endl;
                    return EXIT_FAILURE;
                }
            }
        default:
            std::cerr << mcd_too_many_args_error_message << std::endl;
            return EXIT_FAILURE;
    }
}

int merrno(int argc, char *argv[], char *envp[], const int &errn) {
    switch (argc) {
        case 1:
            std::cout << errn << std::endl;
            return EXIT_SUCCESS;
        case 2:
            if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
                std::cout << merrno_help_message << std::endl;
                return EXIT_SUCCESS;
            } else
                default:
                    std::cerr << merrno_too_many_args_error_message << std::endl;
            return EXIT_FAILURE;
    }
}

int mecho(int argc, char *argv[], char *envp[]) {
    for (size_t i = 1; i < argc; ++i)
        std::cout << argv[i] << ' ';
    std::cout << std::endl;
    return EXIT_SUCCESS;
}

int mexport(int argc, char *argv[], char *envp[], std::vector<char *> &env) {
    if (argc == 2) {
        env.insert(env.begin(), argv[1]);
        if (std::string(argv[1]).find('=') != std::string::npos) {
            std::vector<std::string> splits;
            std::string st = std::string(argv[1]);
            boost::split(splits, st, boost::is_any_of("="));
            setenv(splits[0].c_str(), splits[1].c_str(), 1);
        } else {
            setenv(argv[1], "", 1);
        }
        return EXIT_SUCCESS;
    } else {
        std::cerr << mexport_wrong_args_error_message << std::endl;
        return EXIT_FAILURE;
    }
}

int dotbuiltin(int argc, char *argv[], char *envp[], MyShell *shell) {
    if (argc == 2) {
        return shell->run_script(argv[1]);
    } else {
        std::cerr << dotbuiltin_wrong_args_error_message << std::endl;
        return EXIT_FAILURE;
    }
}
