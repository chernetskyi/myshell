#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mycat.h"

int main(int argc, char *argv[]) {
    if (argc == 1)
        display_help_message();

    int opt;
    size_t invisible_to_hex = 0;
    while ((opt = getopt_long(argc, argv, "Ah", long_options, NULL)) != -1) {
        switch (opt) {
            case 'A':
                invisible_to_hex++;
                break;
            case 'h':
            default:
                display_help_message();
        }
    }

    size_t passed_files = argc - 1 - invisible_to_hex;
#ifndef EXTRA_POINTS
    int *opened_files = (int *) malloc(sizeof(int) * passed_files);
    if (opened_files == NULL) {
        error(malloc_error);
        exit(EXIT_FAILURE);
    }
#else
    FILE **opened_files_stdlib = (FILE **) malloc(sizeof(FILE *) * passed_files);
    if (opened_files_stdlib == NULL) {
        error(malloc_error);
        exit(EXIT_FAILURE);
    }
#endif
    size_t j = 0;
    for (size_t i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "-A", strlen(argv[i]))) {
#ifndef EXTRA_POINTS
            int fd;
            if ((fd = open(argv[i], O_RDONLY)) == -1) {
                error(open_error);
                for (size_t k = 0; k < j; k++)
                    close(opened_files[k]);
                free(opened_files);
                exit(EXIT_FAILURE);
            }
            opened_files[j++] = fd;
#else
            FILE *f;
            if ((f = fopen(argv[i], "r")) == NULL) {
                fwrite(open_error, sizeof(char), strlen(open_error), stderr);
                exit(EXIT_FAILURE);
            }
            opened_files_stdlib[j++] = f;
#endif
        }
    }

#ifndef EXTRA_POINTS
    int err_cd;
    for (size_t i = 0; i < passed_files; ++i) {
        err_cd = cat(opened_files[i], invisible_to_hex);
        close(opened_files[i]);
        if (err_cd) {
            free(opened_files);
            exit(EXIT_FAILURE);
        }
    }
    free(opened_files);
#else
    for (size_t i = 0; i < passed_files; ++i) {
        cat_stdlib(opened_files_stdlib[i], invisible_to_hex);
        fclose(opened_files_stdlib[i]);
    }
    free(opened_files_stdlib);
#endif
    exit(EXIT_SUCCESS);
}

void error(const char *err) {
#ifndef EXTRA_POINTS
    writebuff(STDERR_FILENO, err, strlen(err), false);
#else
    fwrite(err, sizeof(char), strlen(err), stderr);
#endif
}

void display_help_message() {
#ifndef EXTRA_POINTS
    writebuff(STDOUT_FILENO, help_message, strlen(help_message), false);
#else
    fwrite(help_message, sizeof(char), strlen(help_message), stdout);
#endif
    exit(0);
}

#ifndef EXTRA_POINTS

ssize_t readbuff(int fd, char *buffer, ssize_t size) {
    ssize_t read_bytes = 0;
    ssize_t just_read;
    do {
        just_read = read(fd, buffer + read_bytes, size - read_bytes);
        if (just_read < 0) {
            if (errno == EINTR)
                continue;
            return just_read;
        }
        read_bytes += just_read;
    } while (read_bytes < size && just_read != 0);
    return read_bytes;
}

ssize_t writebuff(int fd, const char *buffer, ssize_t size, bool to_hex) {
    if (to_hex) {
        char hexed_buffer[BUFFSIZE * 4];
        size_t j = 0;
        for (size_t i = 0; i < size; ++i, ++j)
            if (!isprint(buffer[i]) && !isspace(buffer[i])) {
                sprintf(&hexed_buffer[j], "\\x%02X", (unsigned char) buffer[i]);
                j += 3;
            } else
                hexed_buffer[j] = buffer[i];
        buffer = hexed_buffer;
        size = j;
    }
    ssize_t written_bytes = 0;
    while (written_bytes < size) {
        ssize_t just_written = write(fd, buffer + written_bytes, size - written_bytes);
        if (just_written == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        written_bytes += just_written;
    }
    return written_bytes;
}

int cat(int fd, bool to_hex) {
    char buffer[BUFFSIZE];
    ssize_t n;
    while ((n = readbuff(fd, buffer, BUFFSIZE)) > 0)
        if (writebuff(STDOUT_FILENO, buffer, n, to_hex) < n) {
            error(write_error);
            return -1;
        }
    if (n < 0) {
        error(read_error);
        return -1;
    }
    return 0;
}

#else

int cat_stdlib(FILE *fp, bool to_hex) {
    char buffer[BUFFSIZE];
    int err = 0;
    do {
        size_t count = fread(buffer, sizeof(char), BUFFSIZE, fp);
        if ((err = ferror(fp))) {
            error(read_error);
            break;
        }
        if (write_stdlib(buffer, count, to_hex) < count) {
            error(write_error);
            return -1;
        }
    } while (!feof(fp));
    return err;
}

size_t write_stdlib(const char *buffer, size_t size, bool to_hex) {
    if (to_hex) {
        char hexed_buffer[BUFFSIZE * 4];
        size_t j = 0;
        for (size_t i = 0; i < size; ++i, ++j)
            if (!isprint(buffer[i]) && !isspace(buffer[i])) {
                sprintf(&hexed_buffer[j], "\\x%02X", (unsigned char) buffer[i]);
                j += 3;
            } else
                hexed_buffer[j] = buffer[i];
        buffer = hexed_buffer;
        size = j;
    }
    return fwrite(buffer, sizeof(char), size, stdout);
}

#endif
