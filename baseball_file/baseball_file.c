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
    const char *prog = argv[PROGRAM_NAME];
    const char *slash = strrchr(argv[PROGRAM_NAME], '/');
    if (slash) prog = slash + 1;

    if (argc < 2) handle_error(
        "Error: missing operation.\n"
        "Try '%s --help' for more information.\n",
        prog
    );

    int mode = resolve_mode(argv[MODE]);

    // call function by the mode
    switch (mode) {
        case READ:
            if (argc != 3) handle_error("%s", get_cmd_usage(prog, "--read <filename>"));
            read_file(argv[FILE_NAME]);
            break;
        case CREATE:
            if (argc != 3) handle_error("%s", get_cmd_usage(prog, "--create <filename>"));
            create_file(argv[FILE_NAME]);
            break;
        case ADD:
            if (argc != MAX_ARGC) {
                fprintf(stderr, "format : %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                       "Player", "Position", "Game", "Batting Avg", "Home Runs",
                       "Era", "Innings", "Wins", "Losses", "Saves", "Strike Outs");
                handle_error("%s", get_cmd_usage(prog, "--add <filename> <player>"));
            }
            add_data(argc, argv);
            break;
        case DELETE:
            if (argc != 4) handle_error("%s", get_cmd_usage(prog, "--delete <filename> <player>"));
            delete_data(argv[FILE_NAME], argv[PLAYER]);
            break;
        case SEARCH:
            if (argc != 4) handle_error("%s", get_cmd_usage(prog, "--search <filename> <player>"));
            search_data(argv[FILE_NAME], argv[PLAYER]);
            break;
        case EDIT:
            if (argc < 4) handle_error("%s", get_cmd_usage(prog, "--edit <filename> <player>"));
            edit_data(argv[FILE_NAME], argv[PLAYER]);
            break;
        case MERGE:
            if (argc < 5) handle_error("%s", get_cmd_usage(prog, "--merge <output_file> <input_file1> <input_file2> ..."));
            merge_files(argc, argv);
            break;
        case HELP:
            printf("%s", get_prog_usage());
            break;
        default:
            handle_error(
                "Error: unknown mode '%s'.\n"
                "Try '%s --help' for more information.\n",
                argv[MODE], argv[PROGRAM_NAME]
            );
    }

    return EXIT_SUCCESS;
}