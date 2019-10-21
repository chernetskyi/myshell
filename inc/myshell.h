#ifndef MYSHELL_MYSHELL_H
#define MYSHELL_MYSHELL_H

#include <string>

std::string normalize_input(char *input);

void execute(const std::string &input, char *envp[], int &errn);

constexpr const char command_not_found_error[] = "myshell: commnad not found: ";

#endif //MYSHELL_MYSHELL_H
