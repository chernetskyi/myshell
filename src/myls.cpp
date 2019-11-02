#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <sys/stat.h>

#include "myls.h"

namespace po = boost::program_options;
using namespace boost::filesystem;

bool is_special(const mode_t &st_mode) {
    return !S_ISDIR(st_mode) || !S_ISREG(st_mode);
}

bool is_dir(const mode_t &st_mode) {
    return S_ISDIR(st_mode);
}

bool file_comparator(const directory_entry &file1, const directory_entry &file2, options_struct &opts,
        std::map<directory_entry, struct stat> stats) {

    struct stat x = stats[file1], y = stats[file2];

    if (opts.directories_first && is_dir(x.st_mode)) {
        if (!is_dir(y.st_mode)) return true;
    } else if (opts.directories_first && is_dir(y.st_mode)) return false;

    if (opts.separate_special_files && is_special(x.st_mode)) {
        if (!is_special(y.st_mode)) return true;
    } else if (opts.separate_special_files && is_special(y.st_mode)) return false;


    bool return_flag = false;
    bool equal = false;

    switch (opts.sort_method) {
        case 'U':
            return_flag = false;
            break;
        case 'S':
            if (x.st_size == y.st_size) equal = true;
            return_flag = x.st_size > y.st_size;
            break;
        case 't':
            if (x.st_mtimespec.tv_sec == y.st_mtimespec.tv_sec) equal = true;
            return_flag = x.st_mtimespec.tv_sec > y.st_mtimespec.tv_sec;
            break;
        case 'X':
            if (file1.path().extension() == file2.path().extension()) equal = true;
            return_flag = file1.path().extension() < file2.path().extension();
            break;
        case 'N':
            return_flag = file1.path().string() < file2.path().string();
            break;
    }

    if (equal) return_flag = file1.path().string() < file2.path().string();
    if (opts.reverse) return_flag = !return_flag;

    return return_flag;
}


std::string trailing_slesh(std::string file) {
    return is_directory(file) ? "/" + file : file;
}

void print_file(directory_entry &file) {
    std::string filename = file.path().filename().string();
    std::cout << trailing_slesh(filename) << "\t";

}

void list_dirs(options_struct &opts) {
    std::map<directory_entry, struct stat> stats;

    for (auto &directory : opts.files) {
        path p(directory);
        std::vector<directory_entry> v;
        if (is_directory(p)) {
            copy(directory_iterator(p), directory_iterator(), back_inserter(v));
            for (auto &file:v) {
                struct stat tmp{};
                stat(const_cast<char *>(file.path().string().c_str()), &tmp);
                stats[file] = tmp;
            }
            std::sort(v.begin(), v.end(),
                    [&opts, &stats] (const directory_entry& x, const directory_entry& y) {
                return file_comparator(x, y, opts, stats);
            });
            std::cout << p.stem() << ":" << std::endl;
            for (auto &file : v) {
                print_file(file);
            }
            std::cout << std::endl;

        } else { std::cout << (is_directory(p.string()) ? "/" + p.string() : p.string()) << std::endl; }
    }
    exit(0);
}

int main(int argc, char *argv[], char *envp[]) {
    po::options_description basic_options("Options");
    basic_options.add_options()

            ("help,h", "print help message")
            (",l", "use a long listing format")
            (",r", "reverse order while sorting")
            (",F", "append indicator to entries")
            (",R", "list subdirectories recursively")
            ("sort", po::value<std::vector<std::string>>()->default_value(std::vector<std::string>{"N"}, "N"),
             "sort by parameter instead of name");
    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
            ("files", po::value<std::vector<std::string>>()->default_value(std::vector<std::string>{"."}, "."),
             "files to apply myls to");
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
    opts.long_listing = vm.count("l") != 0;
    parse_sorting_option(opts, vm["sort"].as<std::vector<std::string>>());
    opts.reverse = vm.count("r") != 0;
    opts.indicate = vm.count("F") != 0;
    opts.recursive = vm.count("R") != 0;

    list_dirs(opts);

    exit(0);
}

void parse_sorting_option(options_struct &options, std::vector<std::string> value) {
    std::string val = value[value.size() - 1];
    switch (val.size()) {
        case 1:
            switch (val[0]) {
                case 'U':
                case 'S':
                case 't':
                case 'X':
                case 'N':
                    options.sort_method = val[0];
                    options.directories_first = options.separate_special_files = false;
                    break;
                default:
                    error(1, invalid_option_error + val + '\n' + try_help_message);
            }
            break;
        case 2:
            if ((val.find('D') == std::string::npos) && (val.find('s') == std::string::npos))
                error(1, invalid_option_error + val + '\n' + try_help_message);
            for (char &c : val)
                switch (c) {
                    case 'U':
                    case 'S':
                    case 't':
                    case 'X':
                    case 'N':
                        options.sort_method = val[0];
                        break;
                    case 'D':
                        options.directories_first = true;
                        options.separate_special_files = false;
                        break;
                    case 's':
                        options.directories_first = false;
                        options.separate_special_files = true;
                        break;
                    default:
                        error(1, invalid_option_error + val + '\n' + try_help_message);
                }
            break;
        case 3:
            if ((val.find('D') == std::string::npos) || (val.find('s') == std::string::npos))
                error(1, invalid_option_error + val + '\n' + try_help_message);
            options.separate_special_files = options.directories_first = true;
            for (char &c : val)
                switch (c) {
                    case 'U':
                    case 'S':
                    case 't':
                    case 'X':
                    case 'N':
                        options.sort_method = val[0];
                        break;
                    case 'D':
                    case 's':
                        break;
                    default:
                        error(1, invalid_option_error + val + '\n' + try_help_message);
                }
            break;
        default:
            error(1, invalid_option_error + val + '\n' + try_help_message);
    }
}

void error(int ec, const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(ec);
}
