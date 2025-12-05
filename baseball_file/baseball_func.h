//
// Created by ckh on 12/2/25.
//

#ifndef FILE_STRUCTURE_PRACTICE_BASEBALL_FUNC_H
#define FILE_STRUCTURE_PRACTICE_BASEBALL_FUNC_H

#include <stdbool.h>

#include "baseball_file.h"

char *get_prog_usage();
char *get_cmd_usage(const char *, const char *);
int resolve_mode(const char *);

void read_file(const char *);
void create_file(const char *);
void add_data(const char **);
void delete_data(const char *, const char *);
void search_data(const char *, const char *);
void edit_data(const char *, const char *);
void merge_files(int, const char **);

unsigned long hash_name(const char *);
void resize_file(const char *);
void read_header(FILE *, FileHeader *);
void update_header(FILE *, const FileHeader *);
int get_bucket_slot_pos(int, unsigned long);
int read_bucket_head(FILE *, long);
void update_bucket_head(FILE *, long, long);
PlayerRecord set_new_record(const char **);
long append_record(FILE *, PlayerRecord *);

void handle_error(const char *, ...);

// --------------------- GPT ---------------------
static int utf8_display_width(const char *);  // 정의 유지?
void utf8_print_padded(const char *, int, int);

#endif //FILE_STRUCTURE_PRACTICE_BASEBALL_FUNC_H