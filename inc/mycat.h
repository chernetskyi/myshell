#ifndef MYCAT_MYCAT_H
#define MYCAT_MYCAT_H

//#define EXTRA_POINTS 1

#include <stdbool.h>

#define BUFFSIZE 4096

const char help_message[] = "Usage: mycat [OPTION] ... [FILE] ...\n"
                            "Concatenate FILE(s) to standard output.\n\n"
                            "    -A          show hex values of invisible characters instead\n"
                            "    -h, --help  display this help and exit\n\n"
                            "Examples:\n"
                            "    mycat f g h     Output f's contents, then g's and h's.\n";

const char open_error[] = "mycat: open error\n";
const char read_error[] = "mycat: read error\n";
const char write_error[] = "mycat: write error\n";
const char malloc_error[] = "mycat: memory allocation error\n";

struct option long_options[] = {{"help", no_argument, NULL, 'h'},
                                {NULL, 0,             NULL, 0}};

void error(const char *error_message);
void display_help_message();

#ifndef EXTRA_POINTS

#include <unistd.h>

ssize_t readbuff(int fd, char *buffer, ssize_t size);
ssize_t writebuff(int fd, const char *buffer, ssize_t size, bool to_hex);

int cat(int fp, bool to_hex);

#else

int cat_stdlib(FILE *fd, bool to_hex);

size_t write_stdlib(const char *buffer, size_t size, bool to_hex);

#endif

#endif //MYCAT_MYCAT_H
