#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <readline/readline.h>
#include <readline/history.h>

#include "builtins.h"
#include "myshell.h"
#include <iostream>
#include <map>


std::string normalize_input(char *input) {
    std::string string_input = std::string(input), filtered_input;
    boost::trim(string_input);
    std::unique_copy(string_input.begin(), string_input.end(), std::back_insert_iterator<std::string>(filtered_input),
                     [](char a, char b) { return isspace(a) && isspace(b); });
    return filtered_input;
}


void MyShell::execute(const std::string &input) {
    std::vector<std::string> splits;
    boost::split(splits, input, boost::is_space());
    if (builtins(splits[0]) != nullptr) {
        builtin func = builtins(splits[0]);
        char **margv = new char *[splits.size()];
        for (size_t i = 0; i < splits.size(); ++i)
            margv[i] = const_cast<char *>(splits[i].c_str());
        erno = func(splits.size(), margv, envp);
        delete[] margv;
    } else {
        std::cerr << command_not_found_error << splits[0] << std::endl;
        erno = 1;
    }
}

void MyShell::start() {
    while ((buff = readline(prompt)) != nullptr) {
        if ((strlen(buff) > 0) && !isspace(buff[0]))
            add_history(buff);
        std::string user_input = normalize_input(buff);
        free(buff);
        execute(user_input);
    }
};


builtin MyShell::builtins(const std::string &command) {
    return builtins_map.find(command) != builtins_map.end() ? builtins_map[command] : nullptr;
}

void MyShell::initialize_builtins() {
    builtins_map["mexit"] = &mexit;
    builtins_map["merrno"] = [this](int argc, char **argv, char **envp) { return merrno(argc, argv, envp, erno); };
    builtins_map["mpwd"] = [this](int argc, char **argv, char **envp) { return mpwd(argc, argv, envp, &current_dir); };
    builtins_map["mcd"] = [this](int argc, char **argv, char **envp) { return mcd(argc, argv, envp, &current_dir); };
}


int main(int argc, char *argv[], char *envp[]) {

    MyShell shell = MyShell(envp);
    shell.start();
    exit(0);
}
