#include <cctype>
#include <iostream>
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "myshell.h"

int main(int argc, char *argv[], char *envp[]) {
    int erno = 0;
    char *buff = nullptr;
    while ((buff = readline("# ")) != nullptr) {
        if ((strlen(buff) > 0) && !isspace(buff[0]))
            add_history(buff);
        std::cout << buff << std::endl;
        free(buff);
    }
    _exit(0);
}
