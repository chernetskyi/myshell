#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include "myls.h"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(int argc, char *argv[], char *envp[]) {
    po::options_description basic_options("Options");

    basic_options.add_options()
            ("help,h", "print help message")
            ("long,l", "use a long listing format")
            ("reverse,r", "reverse order while sorting")
            ("classify,F", "append indicator to entries")
            ("recursive,R", "list subdirectories recursively")
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
    opts.long_listing = vm.count("long") != 0;
    parse_sorting_option(opts, vm["sort"].as<std::vector<std::string>>());
    opts.reverse = vm.count("reverse") != 0;
    opts.indicate = vm.count("classify") != 0;
    opts.recursive = vm.count("recursive") != 0;

    list_dirs(opts);

    exit(0);
}

void list_dirs(options_struct &opts) {
    std::map<fs::directory_entry, struct stat> stats;

    for (auto &directory : opts.files) {
        fs::path directory_path(directory);
        std::vector<fs::directory_entry> files;

        if (is_directory(directory_path)) {
            if (opts.recursive)
                copy(fs::recursive_directory_iterator(directory_path), fs::recursive_directory_iterator(),
                     back_inserter(files));
            else
                copy(fs::directory_iterator(directory_path), fs::directory_iterator(), back_inserter(files));

            for (auto &file : files) {
                struct stat tmp{};
                stat(const_cast<char *>(file.path().string().c_str()), &tmp);
                stats[file] = tmp;
            }

            std::sort(files.begin(), files.end(),
                      [&opts, &stats](const fs::directory_entry &x, const fs::directory_entry &y) {
                          return file_comparator(x, y, opts, stats);
                      });

            if ((opts.files.size() > 1) || opts.recursive)
                std::cout << directory_path.stem() << ":" << std::endl;
            for (auto &file : files)
                std::cout << format_file(file, opts);
            std::cout << std::endl;
        } else
            std::cout << trailing_slash(directory_path.string()) << std::endl;
    }
    exit(0);
}

bool file_comparator(const fs::directory_entry &file1, const fs::directory_entry &file2, options_struct &opts,
                     std::map<fs::directory_entry, struct stat> stats) {

    struct stat x = stats[file1], y = stats[file2];

    if (opts.directories_first && S_ISDIR(x.st_mode)) {
        if (!S_ISDIR(y.st_mode))
            return true;
    } else if (opts.directories_first && S_ISDIR(y.st_mode))
        return false;

    if (opts.separate_special_files && is_special(x.st_mode)) {
        if (!is_special(y.st_mode))
            return true;
    } else if (opts.separate_special_files && is_special(y.st_mode))
        return false;

    bool return_flag = false, equal = false;

    switch (opts.sort_method) {
        case 'U':
            return_flag = false;
            break;
        case 'S':
            if (x.st_size == y.st_size)
                equal = true;
            return_flag = x.st_size > y.st_size;
            break;
        case 't':
#ifdef __APPLE__
            if (x.st_mtimespec.tv_sec == y.st_mtimespec.tv_sec)
                equal = true;
            return_flag = x.st_mtimespec.tv_sec > y.st_mtimespec.tv_sec;
#else
            if (x.st_mtime == y.st_mtime)
                equal = true;
            return_flag = x.st_mtime > y.st_mtime;
#endif
            break;
        case 'X':
            if (file1.path().extension() == file2.path().extension())
                equal = true;
            return_flag = file1.path().extension() < file2.path().extension();
            break;
        case 'N':
            return_flag = file1.path().string() < file2.path().string();
            break;
    }

    if (equal)
        return_flag = file1.path().string() < file2.path().string();

    return opts.reverse == !return_flag;
}

std::string format_file(fs::directory_entry &file, options_struct &flags) {
    struct stat stat_buff;

    stat(file.path().c_str(), &stat_buff);
    std::string filename = trailing_slash(file.path().filename().string()) + ' ';

    if (flags.indicate && !S_ISDIR(stat_buff.st_mode))
        filename = f_flag(filename, stat_buff);
    if (flags.long_listing)
        filename = l_flag(filename, stat_buff);

    return filename;
}

std::string l_flag(std::string &filename, struct stat &stat_buff) {
    struct tm *tm;
    char data_time[256];
    tm = localtime(&stat_buff.st_atime);
    strftime(data_time, sizeof(data_time), "%d.%m.%Y %H:%M:%S", tm);
    return filename + "\t" + std::to_string(stat_buff.st_size) + "\t" + data_time + "\n";
}

std::string f_flag(std::string &filename, struct stat &stat_buff) {
    std::string smb = "?";
    if (stat_buff.st_mode & S_IXUSR)
        smb = "*";
    else {
        switch (stat_buff.st_mode & S_IFMT) {
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
    return smb + filename;
}

std::string trailing_slash(std::string file) {
    return fs::is_directory(file) ? "/" + file : file;
}

void parse_sorting_option(options_struct &options, std::vector<std::string> value) {
    std::string val = value[value.size() - 1];
    switch (val.size()) {
        case 1:
            switch (val[0]) {
                case 'U': case 'S': case 't': case 'X': case 'N':
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
                    case 'U': case 'S': case 't': case 'X': case 'N':
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
                    case 'U': case 'S': case 't': case 'X': case 'N':
                        options.sort_method = val[0];
                        break;
                    case 'D': case 's':
                        break;
                    default:
                        error(1, invalid_option_error + val + '\n' + try_help_message);
                }
            break;
        default:
            error(1, invalid_option_error + val + '\n' + try_help_message);
    }
}

bool is_special(const mode_t &st_mode) {
    return !S_ISDIR(st_mode) && !S_ISREG(st_mode);
}

void error(int ec, const std::string &msg) {
    std::cerr << msg << std::endl;
    exit(ec);
}
