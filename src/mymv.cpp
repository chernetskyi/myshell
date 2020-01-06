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
            ("force,f", "do not prompt before overwriting");
    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
            ("files", po::value<std::vector<std::string>>(), "files to move");
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
        std::cerr << "mymv: missing operand\nTry 'mv --help' for more information." << std::endl;
        return EXIT_FAILURE;
    } else if (files.size() == 2) {
        fs::path from(files[0]);
        fs::path to(files[1]);
        if (fs::exists(from)) {
            if (fs::exists(to)) {
                if ((fs::is_directory(from)) && (fs::is_regular_file(to))) {
                    std::cerr << "mymv: cannot overwrite non-directory with directory" << std::endl;
                    return EXIT_FAILURE;
                } else if ((fs::is_regular_file(from)) && (fs::is_regular_file(to))) {
                    if (!vm.count("force")) {
                        int answer = prompt(files[1]);
                        if ((answer == 0) || (answer == 2)) {
                            fs::rename(from, to, ec);
                        }
                        return EXIT_SUCCESS;
                    } else {
                        fs::rename(from, to, ec);
                        return EXIT_SUCCESS;
                    }
                } else {
                    if (fs::exists(to / from.filename())) {
                        if (!vm.count("force")) {
                            int answer = prompt(files[1]);
                            if ((answer == 0) || (answer == 2)) {
                                fs::rename(from, to / from.filename(), ec);
                            }
                            return EXIT_SUCCESS;
                        }
                    }
                    fs::rename(from, to / from.filename(), ec);
                    return EXIT_SUCCESS;
                }
            }
            fs::rename(from, to, ec);
            return EXIT_SUCCESS;
        } else {
            std::cerr << "mymv: source file does not exist " << files[0] << std::endl;
            return EXIT_FAILURE;
        }
    } else if (files.size() > 2) {
        int error = 0;
        fs::path to(files[files.size() - 1]);
        if (!fs::is_directory(to)) {
            std::cerr << "mymv: destination file is not a directory" << std::endl;
            return EXIT_FAILURE;
        } else {
            for (size_t i = 0; i < files.size() - 1; ++i) {
                fs::path from(files[i]);
                if (fs::exists(to / from.filename()) && !vm.count("force")) {
                    int answer = prompt(files[i]);
                    if (answer == 0) {
                        rename(from, to / from.filename(), ec);
                    } else if (answer == 1) {
                        continue;
                    } else if (answer == 2) {
                        for (size_t j = i; j < files.size() - 1; ++j) {
                            from = fs::path(files[j]);
                            rename(from, to / from.filename(), ec);
                        }
                        return error;
                    } else if (answer == 3) {
                        return error;
                    }
                } else {
                    rename(from, to / from.filename(), ec);
                }
            }
            return error;
        }
    }
}
