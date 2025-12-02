//
// Created by ckh on 12/2/25.
//

#ifndef FILE_STRUCTURE_PRACTICE_BASEBALL_FUNC_H
#define FILE_STRUCTURE_PRACTICE_BASEBALL_FUNC_H

char *get_usage();
char *get_cmd_usage(const char *, const char *);
int resolve_mode(int, const char *);

void read_file(const char *);
void write_file(const char *);
void add_data(const char *, const char *);
void delete_data(const char *, const char *);
void search_data(const char *, const char *);
void edit_data(const char *, const char *);
void merge_files(int, const char **);

void handle_error(const char *, ...);

#endif //FILE_STRUCTURE_PRACTICE_BASEBALL_FUNC_H