//
// Created by ckh on 12/2/25.
//

#ifndef FILE_STRUCTURE_PRACTICE_BASEBALL_FUNC_H
#define FILE_STRUCTURE_PRACTICE_BASEBALL_FUNC_H

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
void write_header(FILE *, const FileHeader *);
long get_bucket_slot_offset(unsigned long);
long read_bucket_slot_head(FILE *, unsigned long);
void write_bucket_slot_head(FILE *, unsigned long, const long *);
void set_new_record(const char **, PlayerRecord *, long);
long append_new_record(FILE *, PlayerRecord *);
static int parse_int(const char *);
static float parse_float(const char *);

void handle_error(const char *, ...);

// --------------------- GPT ---------------------
void utf8_print_padded(const char *, int, int);

#endif //FILE_STRUCTURE_PRACTICE_BASEBALL_FUNC_H