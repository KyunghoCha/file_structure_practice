//
// Created by ckh on 12/1/25.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <locale.h>

#include "baseball_file.h"
#include "baseball_func.h"

int main(int argc, const char **argv) {
    setlocale(LC_ALL, "");  // GPT

    // get program name from the location
    const char *prog = argv[0];
    const char *slash = strrchr(argv[0], '/');
    if (slash) prog = slash + 1;

    if (argc < 2) handle_error(
        "Error: missing operation.\n"
        "Try '%s --help' for more information.\n",
        prog
    );

    int mode = resolve_mode(argv[1]);

    // call function by the mode
    switch (mode) {
        case READ:
            if (argc != 3) handle_error(get_cmd_usage(prog, "--read <filename>"));
            read_file(argv[2]);
            break;
        case CREATE:
            if (argc != 3) handle_error(get_cmd_usage(prog, "--create <filename>"));
            create_file(argv[2]);
            break;
        case ADD:
            if (argc != 14) {
                fprintf(stderr, "format : %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                       "Player", "Position", "Game", "Batting Avg", "Home Runs",
                       "Era", "Innings", "Wins", "Losses", "Saves", "Strike Outs");
                handle_error(get_cmd_usage(prog, "--add <filename> <player>"));
            }
            add_data(argc, argv);
            break;
        case DELETE:
            if (argc != 4) handle_error(get_cmd_usage(prog, "--delete <filename> <player>"));
            delete_data(argv[2], argv[3]);
            break;
        case SEARCH:
            if (argc != 4) handle_error(get_cmd_usage(prog, "--search <filename> <player>"));
            search_data(argv[2], argv[3]);
            break;
        case EDIT:
            if (argc < 4) handle_error(get_cmd_usage(prog, "--edit <filename> <player>"));
            edit_data(argv[2], argv[3]);
            break;
        case MERGE:
            if (argc < 5) handle_error(get_cmd_usage(prog, "--merge <output_file> <input_file1> <input_file2> ..."));
            merge_files(argc, argv);
            break;
        case HELP:
            printf("%s", get_prog_usage());
            break;
        default:
            handle_error(
                "Error: unknown mode '%s'.\n"
                "Try '%s --help' for more information.\n",
                argv[1], argv[0]
            );
    }

    return EXIT_SUCCESS;
}