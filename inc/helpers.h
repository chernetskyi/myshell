#ifndef MYSHELL_HELPERS_H
#define MYSHELL_HELPERS_H

#include <string>
#include <vector>

std::string normalize_input(char *input);

char **get_args(std::vector<char *> splits);

void preprocess_line(std::vector<std::string> &from, std::vector<char *> &to);

#endif //MYSHELL_HELPERS_H
