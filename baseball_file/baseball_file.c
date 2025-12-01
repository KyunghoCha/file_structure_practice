//
// Created by ckh on 12/1/25.
//

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef enum {
    READ,
    WRITE, // 鞘夸?
    ADD,
    REMOVE,
    SEARCH,
    MERGE,
    MODE_NUM
} Mode;

void resolve_mode(int argc, const char *mode);
void handle_error(const char *, ...);

int main(int argc, char **argv) {
    resolve_mode(argc, argv[1]);
    if (argc < 2) {
        handle_error("usage: baseball_file [read|add|remove|search|merge] <filename> <data> ...\n");
    }

    if (strcmp(argv[1], "READ") == 0) {
        if (argc != 3) {
            handle_error("usage: baseball_file [read] <filename>\n");
        }
        // READ 贸府 内靛 眠啊

    } else if (strcmp(argv[1], "WRITE") == 0) {
        // WRITE 贸府 内靛 眠啊

    } else if (strcmp(argv[1], "ADD") == 0) {
        if (argc != 4) {
            handle_error("usage: baseball_file [add] <filename> <data> ...\n");
        }
        // ADD 贸府 内靛 眠啊

    } else if (strcmp(argv[1], "REMOVE") == 0) {
        if (argc != 4) {
            handle_error("usage: baseball_file [remove] <filename> <data>\n");
        }
        // REMOVE 贸府 内靛 眠啊

    } else if (strcmp(argv[1], "SEARCH") == 0) {
        if (argc != 4) {
            handle_error("usage: baseball_file [search] <filename> <data>\n");
        }
        // SEARCH 贸府 内靛 眠啊

    } else if (strcmp(argv[1], "MERGE") == 0) {
        if (argc != 4) {
            handle_error("usage: baseball_file [merge] <master_file> <add_file>\n");
        }
        // MERGE 贸府 内靛 眠啊

    } else {
        handle_error("Invalid operation. Usage: baseball_file [read|add|remove|search|merge] <filename> <data> ...\n");
    }
    if (mode == READ) {
        if (argc != 2) handle_error("usage: baseball_file [READ] <file>");
    }
    if (argc != 4) {
        if (mode == READ)
        else if (mode == MERGE) handle_error("usage: baseball_file <mode> <master_file> <add_file>");
    }


    FILE *file = fopen(argv[1], "rb");

    printf("ckh:\n");

    return 0;
}

void handle_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);

    va_end(ap);
}