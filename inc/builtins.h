#ifndef MYSHELL_BUILTINS_H
#define MYSHELL_BUILTINS_H

typedef std::function<int(int, char **, char **)> builtin;

builtin builtins(const std::string *command, const int &errn);

int mexit(int argc, char *argv[], char *envp[]);

int mpwd(int argc, char *argv[], char *envp[], char *current_dir);

int mcd(int argc, char *argv[], char *envp[], char *current_dir);

int merrno(int argc, char *argv[], char *envp[], const int &errn);

constexpr const char mexit_help_message[] = "exit: exit [n]"
                                            "    Exit the shell.\n"
                                            "    Exits the shell with a status of N. If N is omitted, the exit status is that of the last command executed.";

constexpr const char merrno_help_message[] = "merrno: print the return code of the last command";

constexpr const char mpwd_help_message[] = "mpwd: print current directory";

constexpr const char mcd_help_message[] = "mcd: change directory";

constexpr const char mcd_error_message[] = "mcd: no such file or directory: ";

#endif //MYSHELL_BUILTINS_H
