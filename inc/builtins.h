#ifndef MYSHELL_BUILTINS_H
#define MYSHELL_BUILTINS_H

#include "myshell.h"

int mexit(int argc, char *argv[], char *envp[]);
constexpr const char mexit_help_message[] = "mexit: mexit [n]\n"
                                            "    Exit the shell.\n"
                                            "    Exits the shell with a status of N. If N is omitted, the exit status is that of the last command executed.";
constexpr const char mexit_too_many_args_error_message[] = "mexit: too many arguments";
constexpr const char mexit_not_numeric_error_message[] = "mexit: numeric argument required";

int mpwd(int argc, char *argv[], char *envp[]);
constexpr const char mpwd_help_message[] = "mpwd: mpwd\n"
                                           "    Print full filename of current working directory.";
constexpr const char mpwd_too_many_args_error_message[] = "mpwd: too many arguments";

int mcd(int argc, char *argv[], char *envp[]);
constexpr const char mcd_help_message[] = "mcd: mcd <dir>\n"
                                          "    Change the working directory.";
constexpr const char mcd_no_such_dir_error_message[] = "mcd: no such file or directory: ";
constexpr const char mcd_too_many_args_error_message[] = "mcd: too many arguments";


int merrno(int argc, char *argv[], char *envp[], const int &errn);
constexpr const char merrno_help_message[] = "merrno: merrno\n"
                                             "    Print the return value of the last command.";
constexpr const char merrno_too_many_args_error_message[] = "merrno: too many arguments";

int mecho(int argc, char *argv[], char *envp[]);

int mexport(int argc, char *argv[], char *envp[], std::vector<char *> &env);
constexpr const char mexport_wrong_args_error_message[] = "mexport: wrong number of arguments";

int dotbuiltin(int argc, char *argv[], char *envp[], MyShell *shell);
constexpr const char dotbuiltin_wrong_args_error_message[] = ".: wrong number of arguments";

#endif //MYSHELL_BUILTINS_H
