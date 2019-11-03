#ifndef MYSHELL_MYLS_H
#define MYSHELL_MYLS_H

#include <string>
#include <vector>

#include <boost/filesystem.hpp>

typedef struct {
    std::vector<std::string> files;
    bool long_listing;
    char sort_method;
    bool directories_first;
    bool separate_special_files;
    bool reverse;
    bool indicate;
    bool recursive;
} options_struct;

void list_dirs(options_struct &opts);

bool file_comparator(const boost::filesystem::directory_entry &file1, const boost::filesystem::directory_entry &file2,
                     options_struct &opts, std::map<boost::filesystem::directory_entry, struct stat> stats);

std::string format_file(boost::filesystem::directory_entry &file, options_struct &flags);

std::string l_flag(std::string &filename, struct stat &stat_buff);
std::string f_flag(std::string &filename, struct stat &stat_buff);

std::string trailing_slash(std::string file);

void parse_sorting_option(options_struct &options, std::vector<std::string> value);

bool is_special(const mode_t &st_mode);

void error(int ec, const std::string &msg);

constexpr char invalid_option_error[] = "myls: invalid option -- ";
constexpr char try_help_message[] = "Try 'myls --help' for more information.";

#endif //MYSHELL_MYLS_H
