//
// Created by ckh on 12/2/25.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "baseball_file.h"
#include "baseball_func.h"

// return usage text
char *get_usage() {
    return "Usage: baseball_file [operation] <filename> <player> ...\n"
           "\n"
           "Operations:\n"
           "  -r, --read      <filename>             - Read data from the specified file\n"
           "  -w, --write     <filename>             - Write data to the specified file\n"
           "  -a, --add       <filename> <player>    - Add a new record to the file\n"
           "  -d, --delete    <filename> <player>    - Delete a record from the file\n"
           "  -s, --search    <filename> <player>    - Search for a record in the file\n"
           "  -e, --edit      <filename> <player>    - Edit an existing record\n"
           "  -m, --merge     <output_file> <input_file1> <input_file2> ...\n"
           "                                         - Merge multiple files into one\n"
           "  -h, --help                             - Display this help message\n"
           "\n"
           "Record format (one line per player):\n"
           "  name,position,game,avg,home_runs,era,innings,wins,losses,saves,strikeouts\n"
           "\n"
           "  - position : 'B' (batter) or 'P' (pitcher)\n"
           "  - For batters : era/innings/wins/losses/saves/strikeouts are set to 0\n"
           "  - For pitchers: avg and home_runs are set to 0\n"
           "\n"
           "Example usage:\n"
           "  baseball_file --read players.bin\n"
           "  baseball_file -a players.bin '홍길동'\n"
           "  baseball_file --search players.bin '홍길동'\n"
           "  baseball_file -m output.bin players1.bin players2.bin\n"
           "\n"
           "For more detailed information on each operation, use 'baseball_file --help'.\n";
}

char *get_cmd_usage(const char *prog, const char *pattern) {
    static char buf[256];  // Heap

    snprintf(buf, sizeof(buf),
             "Usage: %s %s\n"
             "Try '%s --help' for more information.\n",
             prog, pattern, prog);

    return buf;
}

// resolve mode
int resolve_mode(int argc, const char *opt) {
    if (strcmp(opt, "--read") == 0 || strcmp(opt, "-r") == 0) return READ;
    if (strcmp(opt, "--write") == 0 || strcmp(opt, "-w") == 0) return WRITE;
    if (strcmp(opt, "--add") == 0 || strcmp(opt, "-a") == 0) return ADD;
    if (strcmp(opt, "--delete") == 0 || strcmp(opt, "-d") == 0) return DELETE;
    if (strcmp(opt, "--search") == 0 || strcmp(opt, "-s") == 0) return SEARCH;
    if (strcmp(opt, "--edit") == 0 || strcmp(opt, "-e") == 0) return EDIT;
    if (strcmp(opt, "--merge") == 0 || strcmp(opt, "-m") == 0) return MERGE;
    if (strcmp(opt, "--help") == 0 || strcmp(opt, "-h") == 0) return HELP;
    return MODE_NUM;
}

// read file
void read_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) handle_error("failed to open file: %s", filename);

    

    fclose(file);
}

// write new file
void write_file(const char *filename) {

}

// add new player
void add_data(const char *filename, const char *player) {

}

// remove player
void delete_data(const char *filename, const char *player) {

}

// search player
void search_data(const char *filename, const char *player) {

}

// edit player's data
void edit_data(const char *filename, const char *player) {

}

// merge files
void merge_files(int argc, const char **argv) {

}

void handle_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);

    va_end(ap);

    exit(EXIT_FAILURE);
}