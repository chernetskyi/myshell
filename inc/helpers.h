//
// Created by nick bilayev on 2019-10-23.
//

#include <string>
#include <vector>
#include <string>

#ifndef MYSHELL_HELPERS_H
#define MYSHELL_HELPERS_H


std::string normalize_input(char *input);

char **get_args(std::vector<char *> splits);

void preprocess_line(std::vector<std::string> &from, std::vector<char *> &to);

#endif //MYSHELL_HELPERS_H
