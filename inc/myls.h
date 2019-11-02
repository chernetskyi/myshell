#ifndef MYSHELL_MYLS_H
#define MYSHELL_MYLS_H

#include <string>
#include <vector>

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

void parse_sorting_option(options_struct &options, std::vector<std::string> value);

void error(int ec, const std::string& msg);

constexpr char invalid_option_error[] = "myls: invalid option -- ";
constexpr char try_help_message[] = "Try 'myls --help' for more information.";

#endif //MYSHELL_MYLS_H
