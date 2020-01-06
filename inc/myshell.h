#ifndef MYSHELL_MYSHELL_H
#define MYSHELL_MYSHELL_H

#include <functional>
#include <map>
#include <string>
#include <vector>

typedef std::function<int(int, char **, char **)> builtin;

#include "builtins.h"

class MyShell {
private:
    int erno;
    std::vector<char *> env;
    std::map<std::string, builtin> builtins_map;

    void initialize_builtins();

    std::vector<const char * > parse(std::string &input);

    std::string prompt();

public:
    explicit MyShell(char *envp[]);

    void start();

    int run_script(char *scriptname);
};

constexpr const char pipe_error_message[] = "myshell: can not create pipe";

#endif //MYSHELL_MYSHELL_H
