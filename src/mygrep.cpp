#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

void lowercase(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
}

int main(int argc, char *argv[], char *envp[]) {
    std::vector<std::string> regexp, pttrn, file;
    po::options_description basic_options("Options");
    basic_options.add_options()
            ("help,h", "display this help and exit")
            ("invert-match,v", "invert the sense of matching, to select non-matching lines")
            ("ignore-case,i", "ignore case distinctions")
            ("regexp", po::value<std::vector<std::string>>(&regexp), "use regexp instead of a pattern");
    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
            ("pattern", po::value<std::vector<std::string>>(&pttrn), "pattern to match")
            ("file", po::value<std::vector<std::string>>(&file), "file to parse");
    po::options_description options;
    options.add(basic_options).add(hidden_options);
    po::positional_options_description positional;
    positional.add("pattern", 1);
    positional.add("file", 1);
    po::variables_map vm;
    auto parsed = po::command_line_parser(argc, argv).options(options).positional(positional).run();
    po::store(parsed, vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << basic_options << std::endl;
        return EXIT_SUCCESS;
    }

    bool icase = vm.count("ignore-case");
    bool invert = vm.count("invert-match");
    bool regex = !regexp.empty();
    std::string pattern = regex ? regexp[0] : pttrn[0];

    if (icase) {
        lowercase(pattern);
    }
    std::string line;
    std::vector<std::string> results;
    if (file.empty()) {
        while (std::getline(std::cin, line)) {
            if (icase) {
                lowercase(line);
            }
            bool to_add;
            if (regex) {
                std::regex ex(pattern);
                to_add = std::regex_match(line, ex) == !invert;
            } else {
                to_add = (line.find(pattern) != std::string::npos) == !invert;
            }
            if (to_add) {
                std::cout << line << std::endl;
                results.push_back(line);
            }
        }
    } else {
        std::ifstream is(file[0]);
        while (std::getline(is, line)) {
            if (icase) {
                lowercase(line);
            }
            bool to_add;
            if (regex) {
                std::regex ex(pattern);
                to_add = std::regex_match(line, ex) == !invert;
            } else {
                to_add = (line.find(pattern) != std::string::npos) == !invert;
            }
            if (to_add) {
                std::cout << line << std::endl;
                results.push_back(line);
            }
        }
    }
    return !results.empty();
}
