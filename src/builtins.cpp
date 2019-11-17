#include <iostream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "builtins.h"

int mexit(int argc, char *argv[], char *envp[]) {
    switch (argc) {
        case 1:
            exit(0);
        case 2:
            if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
                std::cout << mexit_help_message << std::endl;
                return 0;
            } else {
                try {
                    return boost::lexical_cast<int>(argv[1]);
                } catch (boost::bad_lexical_cast&) {
                    std::cerr << mexit_not_numeric_error_message << std::endl;
                    return 1;
                }
            }
        default:
            std::cerr << mexit_too_many_args_error_message << std::endl;
            return 1;
    }
}

int mpwd(int argc, char *argv[], char *envp[]) {
    switch (argc) {
        case 1:
            std::cout << boost::filesystem::current_path().string() << std::endl;
            return 0;
        case 2:
            if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
                std::cout << mpwd_help_message << std::endl;
                return 0;
            } else
        default:
            std::cerr << mpwd_too_many_args_error_message << std::endl;
            return 1;
    }
}

int mcd(int argc, char *argv[], char *envp[]) {
    switch (argc) {
        case 1:
            boost::filesystem::current_path(getenv("HOME"));
            return 1;
        case 2:
            if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
                std::cout << mcd_help_message << std::endl;
                return 0;
            } else {
                try {
                    boost::filesystem::current_path(argv[1]);
                    return 0;
                } catch (boost::filesystem::filesystem_error&) {
                    std::cerr << mcd_no_such_dir_error_message << argv[1] << std::endl;
                    return 1;
                }
            }
        default:
            std::cerr << mcd_too_many_args_error_message << std::endl;
            return 1;
    }
}

int merrno(int argc, char *argv[], char *envp[], const int &errn) {
    switch (argc) {
        case 1:
            std::cout << errn << std::endl;
            return 0;
        case 2:
            if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
                std::cout << merrno_help_message << std::endl;
                return 0;
            } else
        default:
            std::cerr << merrno_too_many_args_error_message << std::endl;
            return 1;
    }
}

int mecho(int argc, char *argv[], char *envp[]) {
    for (size_t i = 1; i < argc; ++i)
        std::cout << argv[i] << ' ';
    std::cout << std::endl;
    return 0;
}

int mexport(int argc, char *argv[], char *envp[]) {
    return 0;
}

int dotbuiltin(int argc, char *argv[], char *envp[]) {
    return 0;
}
