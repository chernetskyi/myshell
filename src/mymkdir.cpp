#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(int argc, char *argv[], char *envp[]) {
    po::options_description basic_options("Options");

    basic_options.add_options()
            ("help,h", "display this help and exit")
            ("parents,p", "no error if existing, make parent directories as needed");
    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
            ("directory", po::value<std::vector<std::string>>(), "directories to create");
    po::options_description options;
    options.add(basic_options).add(hidden_options);
    po::positional_options_description positional;
    positional.add("directory", 1);
    po::variables_map vm;
    auto parsed = po::command_line_parser(argc, argv).options(options).positional(positional).run();
    po::store(parsed, vm);

    if (vm.count("help")) {
        std::cout << basic_options << std::endl;
        return EXIT_SUCCESS;
    }

    if (vm["directory"].as<std::vector<std::string>>().empty()) {
        std::cerr << "mymkdir: missing operand\nTry 'mkdir --help' for more information." << std::endl;
        return EXIT_FAILURE;
    } else {
        boost::system::error_code ec;
        fs::path pathname(vm["directory"].as<std::vector<std::string>>()[0]);
        if (vm.count("parents")) {
            fs::create_directories(pathname, ec);
            return EXIT_SUCCESS;
        } else {
            if (!fs::create_directory(pathname, ec)) {
                std::cerr << "mymkdir: an error occurred while trying to create directory" << std::endl;
                return EXIT_FAILURE;
            } else {
                return EXIT_SUCCESS;
            }
        }
    }
}
