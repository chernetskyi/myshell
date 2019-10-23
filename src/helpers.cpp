#include "helpers.h"


#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>


#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>
#include <string>
#include <map>


std::string normalize_input(char *input) {
    std::string string_input = std::string(input), filtered_input;
    boost::trim(string_input);
    std::unique_copy(string_input.begin(), string_input.end(), std::back_insert_iterator<std::string>(filtered_input),
                     [](char a, char b) { return isspace(a) && isspace(b); });
    return filtered_input;
}

char **get_args(std::vector<char *> splits) {
    char **margv = new char *[splits.size() + 1];
    for (size_t i = 0; i < splits.size(); ++i)
        margv[i] = splits[i];
    margv[splits.size()] = NULL;
    return margv;
}


void preprocess_line(std::vector<std::string> &from, std::vector<char *> &to) {

    for (auto &value: from) {

        if (value[0] == '$') {
            value.erase(0, 1);
            char *val = getenv(value.c_str());
            to.push_back(val);
        } else {
            to.push_back(const_cast<char *>(value.c_str()));
        }
    }

}


