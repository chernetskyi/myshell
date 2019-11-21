#ifndef MYSHELL_MYSHELL_H
#define MYSHELL_MYSHELL_H

#include <map>
#include <string>
#include <vector>

#include "builtins.h"

class MyShell {
private:
    int erno;
    std::vector<char *> env;
    std::map<std::string, builtin> builtins_map;

    void initialize_builtins();

    std::vector<const char * > process(std::string &input);

    std::string prompt();

public:
    explicit MyShell(char *envp[]);

    void start();
};

#endif //MYSHELL_MYSHELL_H
