//
// Created by ckh on 12/2/25.
//

#ifndef FILE_STRUCTURE_PRACTICE_BASEBALL_FUNC_H
#define FILE_STRUCTURE_PRACTICE_BASEBALL_FUNC_H

char *get_prog_usage();
char *get_cmd_usage(const char *, const char *);
int resolve_mode(const char *);

void read_file(const char *);
void create_file(const char *);
void add_data(int, const char **);
void delete_data(const char *, const char *);
void search_data(const char *, const char *);
void edit_data(const char *, const char *);
void merge_files(int, const char **);

unsigned long hash_name(const char *);
void resize_file(const char *);


void handle_error(const char *, ...);

// --------------------- GPT ---------------------
static int utf8_display_width(const char *);
void utf8_print_padded(const char *, int, int);

#endif //FILE_STRUCTURE_PRACTICE_BASEBALL_FUNC_H