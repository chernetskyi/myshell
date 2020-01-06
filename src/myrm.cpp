#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int prompt(const std::string &filename) {
    while (true) {
        std::cout << "Are you sure, you want to delete " << filename << "? (Y[es]/N[o]/A[ll]/C[ancel])" << std::endl;
        std::string answer;
        std::cin >> answer;
        if (answer.size() == 1) {
            switch (tolower(answer[0])) {
                case 'y':
                    return 0;
                case 'n':
                    return 1;
                case 'a':
                    return 2;
                case 'c':
                    return 3;
                default:
                    continue;
            }
        }
    };
}

int main(int argc, char *argv[], char *envp[]) {
    po::options_description basic_options("Options");

    basic_options.add_options()
            ("help,h", "display this help and exit")
            ("force,f", "ignore nonexistent files, never prompt")
            ("recursive,R", "remove directories and their contents recursively");
    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
            ("files", po::value<std::vector<std::string>>(), "files to remove");
    po::options_description options;
    options.add(basic_options).add(hidden_options);
    po::positional_options_description positional;
    positional.add("files", -1);
    po::variables_map vm;
    auto parsed = po::command_line_parser(argc, argv).options(options).positional(positional).run();
    po::store(parsed, vm);

    if (vm.count("help")) {
        std::cout << basic_options << std::endl;
        return EXIT_SUCCESS;
    }

    int error = 0;

    std::vector<std::string> files = vm["files"].as<std::vector<std::string>>();
    if (files.empty()) {
        std::cerr << "rm: missing operand\nTry 'rm --help' for more information." << std::endl;
        return EXIT_FAILURE;
    } else {
        boost::system::error_code ec;
        for (size_t i = 0; i < files.size(); ++i) {
            fs::path pathname(files[i]);
            if (!fs::exists(pathname)) {
                std::cerr << "myrm: file does not exist " << files[i] << std::endl;
                if (error == 0) {
                    error = 1;
                }
            } else if (vm.count("recursive")) {
                if (vm.count("force")) {
                    fs::remove_all(pathname, ec);
                } else {
                    int answer = prompt(files[i]);
                    if (answer == 0) {
                        fs::remove_all(pathname, ec);
                    } else if (answer == 1) {
                        continue;
                    } else if (answer == 2) {
                        for (size_t j = i; j < files.size(); ++j) {
                            pathname = fs::path(files[j]);
                            fs::remove_all(pathname, ec);
                        }
                        return EXIT_SUCCESS;
                    } else if (answer == 3) {
                        return EXIT_SUCCESS;
                    }
                }
            } else {
                if (vm.count("force")) {
                    if (fs::is_directory(pathname)) {
                        std::cerr << "myrm: file is a directory " << files[i] << std::endl;
                        if (error == 0) {
                            error = 1;
                        }
                    } else if (!fs::remove(pathname, ec)) {
                        std::cerr << "myrm: an error occurred while trying to remove " << files[i] << std::endl;
                        if (error == 0) {
                            error = 1;
                        }
                    }
                } else {
                    int answer = prompt(files[i]);
                    if (answer == 0) {
                        if (fs::is_directory(pathname)) {
                            std::cerr << "myrm: file is a directory " << files[i] << std::endl;
                            if (error == 0) {
                                error = 1;
                            }
                        } else if (!fs::remove(pathname, ec)) {
                            std::cerr << "myrm: an error occurred while trying to remove " << files[i] << std::endl;
                            if (error == 0) {
                                error = 1;
                            }
                        }
                    } else if (answer == 1) {
                        continue;
                    } else if (answer == 2) {
                        for (size_t j = i; j < files.size(); ++j) {
                            pathname = fs::path(files[j]);
                            if (!fs::exists(pathname)) {
                                std::cerr << "myrm: file does not exist " << files[j] << std::endl;
                                if (error == 0) {
                                    error = 1;
                                }
                            } else if (fs::is_directory(pathname)) {
                                std::cerr << "myrm: file is a directory " << files[j] << std::endl;
                                if (error == 0) {
                                    error = 1;
                                }
                            } else if (!fs::remove(pathname, ec)) {
                                std::cerr << "myrm: an error occurred while trying to remove " << files[j] << std::endl;
                                if (error == 0) {
                                    error = 1;
                                }
                            }
                        }
                        return error;
                    } else if (answer == 3) {
                        return error;
                    }
                }
            }
        }
    }
    return error;
}
