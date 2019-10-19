#include <cstdio>
#include <cctype>
#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

#include "myshell.h"

int main(int argc, char *argv[], char *envp[]) {
    int erno = 0;
    char *buff = nullptr;
    while ((buff = readline("# ")) != nullptr) {
        if ((strlen(buff) > 0) && !isspace(buff[0]))
            add_history(buff);
        std::cout << buff << std::endl;
        delete buff;
    }
    _exit(0);
}
