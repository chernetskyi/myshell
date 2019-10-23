#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "builtins.h"

namespace po = boost::program_options;

int mexit(int argc, char *argv[], char *envp[]) {
    po::options_description basic_options("Options");
    basic_options.add_options()
            ("help,h", "print help message");
    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
            ("status-code", po::value<int>()->default_value(0), "status code to return");
    po::options_description options;
    options.add(basic_options).add(hidden_options);
    po::positional_options_description positional;
    positional.add("status-code", 1);
    po::variables_map vm;
    auto parsed = po::command_line_parser(argc, argv).options(options).positional(positional).run();
    po::store(parsed, vm);

    if (vm.count("help")) {
        std::cout << mexit_help_message << std::endl;
        return 0;
    } else
        exit(vm["status-code"].as<int>());
}

int mpwd(int argc, char *argv[], char *envp[]) {
    po::options_description options("Options");
    options.add_options()
            ("help,h", "print help message");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, options), vm);

    if (vm.count("help"))
        std::cout << mpwd_help_message << std::endl;
    else
        std::cout << boost::filesystem::current_path().string() << std::endl;
    return 0;
}

int mcd(int argc, char *argv[], char *envp[]) {
    po::options_description basic_options("Options");
    basic_options.add_options()
            ("help,h", "print help message");
    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
            ("path", po::value<std::string>()->default_value("~"), "path to new directory");
    po::options_description options;
    options.add(basic_options).add(hidden_options);
    po::positional_options_description positional;
    positional.add("path", 1);
    po::variables_map vm;
    auto parsed = po::command_line_parser(argc, argv).options(options).positional(positional).run();
    po::store(parsed, vm);

    if (vm.count("help"))
        std::cout << mcd_help_message << std::endl;
    else
        boost::filesystem::current_path(vm["path"].as<std::string>());
    return 0;
}

int merrno(int argc, char *argv[], char *envp[], const int &errn) {
    po::options_description options("Options");
    options.add_options()
            ("help,h", "print help message");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, options), vm);

    if (vm.count("help"))
        std::cout << merrno_help_message << std::endl;
    else
        std::cout << errn << std::endl;
    return 0;
}
