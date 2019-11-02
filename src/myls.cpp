#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <sys/stat.h>


#include "myls.h"

namespace po = boost::program_options;
using namespace boost::filesystem;


std::string trailing_slesh(std::string file) {
    return is_directory(file) ? "/" + file : file;
}


std::string f_flag(std::string &filename, struct stat &stat_buff) {
    std::string smb = "?";
    if (stat_buff.st_mode & S_IXUSR)
        smb = "*";
    else {
        switch (stat_buff.st_mode & S_IFMT) {
            case S_IFDIR:
                smb = "/";
                break;
            case S_IFLNK:
                smb = "@";
                break;
            case S_IFSOCK:
                smb = "=";
                break;
            default:
                smb = "?";
        }
    }
    return smb + filename + "\t";
}

std::string l_flag(std::string &filename, struct stat &stat_buff) {
    struct tm *tm;
    char data_time[200];
    tm = localtime(&stat_buff.st_atime);
    strftime(data_time, sizeof(data_time), "%d.%m.%Y %H:%M:%S", tm);

    return trailing_slesh(filename) + "\t" + std::to_string(stat_buff.st_size) + "\t" + data_time + "\n";
}

std::string format_file(directory_entry &file, options_struct &flags) {
    struct stat stat_buff;

    stat(file.path().c_str(), &stat_buff);
    std::string filename = file.path().filename().string();

    //working with flags
    if (flags.indicate) filename = f_flag(filename, stat_buff);
    else if (flags.long_listing) filename = l_flag(filename, stat_buff);
    else filename += "\t";

    return filename;
}

void list_dirs(options_struct &opts) {
    for (auto &directory : opts.files) {
        path directory_path(directory);
        std::vector<directory_entry> files;
        if (is_directory(directory_path)) {
            if (opts.recursive)
                copy(recursive_directory_iterator(directory_path), recursive_directory_iterator(),
                     back_inserter(files));
            else
                copy(directory_iterator(directory_path), directory_iterator(), back_inserter(files));
            std::cout << directory_path.stem() << ":" << std::endl;
            for (auto &file :  files) std::cout << format_file(file, opts);
            std::cout << std::endl;
        } else std::cout << trailing_slesh(directory_path.string()) << std::endl;
    }
    exit(0);
}

int main(int argc, char *argv[], char *envp[]) {
    po::options_description basic_options("Options");

    //TODO fix this s** only forks with --arg, must be -arg
    basic_options.add_options()
            ("help,h", "print help message")
            ("l", "use a long listing format")
            ("r", "reverse order while sorting")
            ("F", "append indicator to entries")
            ("R", "list subdirectories recursively")
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

void error(int ec, std::string msg) {
    std::cerr << msg << std::endl;
    exit(ec);
}
