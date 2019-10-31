#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "myls.h"

namespace po = boost::program_options;

int main(int argc, char *argv[], char *envp[]) {
    po::options_description basic_options("Options");
    basic_options.add_options()
            ("help,h", "print help message")
            (",l", "use a long listing format")
            (",r", "reverse order while sorting")
            (",F", "append indicator to entries")
            (",R", "list subdirectories recursively")
            ("sort", po::value<std::vector<std::string>>()->default_value(std::vector<std::string>{"N"}, "N"), "sort by parameter instead of name");
    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
            ("files", po::value<std::vector<std::string>>()->default_value(std::vector<std::string>{"."}, "."), "files to apply myls to");
    po::options_description options;
    options.add(basic_options).add(hidden_options);
    po::positional_options_description positional;
    positional.add("files", -1);
    po::variables_map vm;
    auto parsed = po::command_line_parser(argc, argv).options(options).positional(positional).run();
    po::store(parsed, vm);

    if (vm.count("help")) {
        std::cout << basic_options << std::endl;
        exit(0);
    }

    options_struct opts;
    opts.files = vm["files"].as<std::vector<std::string>>();
    opts.long_listing = vm.count("l");
    parse_sorting_option(opts, vm["sort"].as<std::vector<std::string>>());
    opts.reverse = vm.count("r");
    opts.indicate = vm.count("F");
    opts.recursive = vm.count("R");

    exit(0);
}

void parse_sorting_option(options_struct options, std::vector<std::string> value) {

}
