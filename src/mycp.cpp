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
        std::cout << "Are you sure, you want to overwrite " << filename << "? (Y[es]/N[o]/A[ll]/C[ancel])" << std::endl;
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
            ("force,f", "do not prompt before overwriting")
            ("recursive,R", "copy directories recursively");
    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
            ("files", po::value<std::vector<std::string>>(), "files to copy");
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

    boost::system::error_code ec;
    std::vector<std::string> files = vm["files"].as<std::vector<std::string>>();
    if (files.size() < 2) {
        std::cerr << "mycp: missing operand\nTry 'mycp --help' for more information." << std::endl;
        return EXIT_FAILURE;
    } else if (files.size() == 2) {
        fs::path from(files[0]);
        fs::path to(files[1]);
        if (!fs::exists(from)) {
            std::cerr << "mycp: file does not exist" << std::endl;
            return EXIT_FAILURE;
        }
        if (fs::is_directory(from) && !vm.count("recursive")) {
            std::cerr << "mycp: cannot copy directory" << std::endl;
            return EXIT_FAILURE;
        }
        if (!fs::exists(to)) {
            fs::copy(from, to, ec);
            return EXIT_SUCCESS;
        }
        if (fs::is_directory(from) && fs::is_regular_file(to)) {
            std::cerr << "mycp: cannot overwrite non-directory with a directory" << std::endl;
            return EXIT_FAILURE;
        }
        if (fs::is_regular_file(from) && fs::is_regular_file(to)) {
            if (!vm.count("force")) {
                int answer = prompt(files[1]);
                if ((answer == 0) || (answer == 2)) {
                    fs::copy_file(from, to, fs::copy_option::overwrite_if_exists, ec);
                }
            } else {
                fs::copy_file(from, to, fs::copy_option::overwrite_if_exists, ec);
            }
            return EXIT_SUCCESS;
        }
        if (fs::is_regular_file(from) && fs::is_directory(to)) {
            if (!fs::exists(to / from.filename())) {
                fs::copy_file(from, to / from.filename(), fs::copy_option::overwrite_if_exists, ec);
            } else {
                if (!vm.count("force")) {
                    int answer = prompt(files[1]);
                    if ((answer == 0) || (answer == 2)) {
                        fs::copy_file(from, to / from.filename(), fs::copy_option::overwrite_if_exists, ec);
                    }
                } else {
                    fs::copy_file(from, to / from.filename(), fs::copy_option::overwrite_if_exists, ec);
                }
            }
            return EXIT_SUCCESS;
        }
        if (fs::is_directory(from) && fs::is_directory(to)) {
            if (!fs::exists(to / from.filename())) {
                fs::copy_directory(from, to / from.filename(), ec);
            } else {
                if (!vm.count("force")) {
                    int answer = prompt(files[1]);
                    if ((answer == 0) || (answer == 2)) {
                        fs::copy_directory(from, to / from.filename(), ec);
                    }
                } else {
                    fs::copy_file(from, to / from.filename(), ec);
                }
            }
            return EXIT_SUCCESS;
        }
    } else if (files.size() > 2) {
        fs::path to(files[files.size() - 1]);
        if (!fs::is_directory(to)) {
            std::cerr << "mycp: destination file is not a directory" << std::endl;
            return EXIT_FAILURE;
        }
        if (!vm.count("recursive")) {
            for (size_t i = 0; i < files.size() - 1; ++i) {
                fs::path from(files[i]);
                if (fs::is_directory(from)) {
                    std::cerr << "mycp: trying to copy directory " << files[i] << std::endl;
                    return EXIT_FAILURE;
                }
            }
            for (size_t i = 0; i < files.size() - 1; ++i) {
                fs::path from(files[i]);
                if (!fs::exists(to / from.filename())) {
                    fs::copy_file(from, to / from.filename(), fs::copy_option::overwrite_if_exists, ec);
                } else {
                    if (!vm.count("force")) {
                        int answer = prompt(files[1]);
                        if (answer == 0) {
                            fs::copy_file(from, to / from.filename(), fs::copy_option::overwrite_if_exists, ec);
                        } else if (answer == 1) {
                            continue;
                        } else if (answer == 2) {
                            for (size_t j = i; j < files.size() - 1; ++j) {
                                from = fs::path(files[j]);
                                fs::copy_file(from, to / from.filename(), fs::copy_option::overwrite_if_exists, ec);
                            }
                            return EXIT_SUCCESS;
                        } else if (answer == 3) {
                            return EXIT_SUCCESS;
                        }
                    } else {
                        fs::copy_file(from, to / from.filename(), fs::copy_option::overwrite_if_exists, ec);
                    }
                }
                return EXIT_SUCCESS;
            }
        } else {
            for (size_t i = 0; i < files.size() - 1; ++i) {
                fs::path from(files[i]);
                if (!fs::exists(to / from.filename())) {
                    if (fs::is_directory(from)) {
                        fs::copy_directory(from, to / from.filename(), ec);
                    } else {
                        fs::copy_file(from, to / from.filename(), fs::copy_option::overwrite_if_exists, ec);
                    }
                } else {
                    if (!vm.count("force")) {
                        int answer = prompt(files[i]);
                        if (answer == 0) {
                            if (fs::is_directory(from)) {
                                fs::copy_directory(from, to / from.filename(), ec);
                            } else {
                                fs::copy_file(from, to / from.filename(), fs::copy_option::overwrite_if_exists, ec);
                            }
                        } else if (answer == 1) {
                            continue;
                        } else if (answer == 2) {
                            for (size_t j = i; j < files.size() - 1; ++j) {
                                from = fs::path(files[j]);
                                if (fs::is_directory(from)) {
                                    fs::copy_directory(from, to / from.filename(), ec);
                                } else {
                                    fs::copy_file(from, to / from.filename(), fs::copy_option::overwrite_if_exists, ec);
                                }
                            }
                            return EXIT_SUCCESS;
                        } else if (answer == 3) {
                            return EXIT_SUCCESS;
                        }
                    } else {
                        if (fs::is_directory(from)) {
                            fs::copy_directory(from, to / from.filename(), ec);
                        } else {
                            fs::copy_file(from, to / from.filename(), fs::copy_option::overwrite_if_exists, ec);
                        }
                    }
                }
            }
            return EXIT_SUCCESS;
        }
    }
}
