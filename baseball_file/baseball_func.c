//
// Created by ckh on 12/2/25.
//

#define _GNU_SOURCE  // GPT

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>

#include "baseball_file.h"
#include "baseball_func.h"

// return usage text
char *get_prog_usage() {  // GPT
    return "Usage: baseball_file [operation] <filename> <player> ...\n"
           "\n"
           "Operations:\n"
           "  -r, --read      <filename>             - Read data from the specified file\n"
           "  -c, --create    <filename>             - Create a new file\n"
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
    static char buf[256];

    snprintf(buf, sizeof(buf),
             "Usage: %s %s\n"
             "Try '%s --help' for more information.\n",
             prog, pattern, prog);

    return buf;
}

// resolve mode
int resolve_mode(const char *mode) {
    if (strcmp(mode, "--read")   == 0 || strcmp(mode, "-r") == 0) return READ;
    if (strcmp(mode, "--create") == 0 || strcmp(mode, "-c") == 0) return CREATE;
    if (strcmp(mode, "--add")    == 0 || strcmp(mode, "-a") == 0) return ADD;
    if (strcmp(mode, "--delete") == 0 || strcmp(mode, "-d") == 0) return DELETE;
    if (strcmp(mode, "--search") == 0 || strcmp(mode, "-s") == 0) return SEARCH;
    if (strcmp(mode, "--edit")   == 0 || strcmp(mode, "-e") == 0) return EDIT;
    if (strcmp(mode, "--merge")  == 0 || strcmp(mode, "-m") == 0) return MERGE;
    if (strcmp(mode, "--help")   == 0 || strcmp(mode, "-h") == 0) return HELP;
    return UNKNOWN_MODE;
}

// =====================================================================================================================
// read & print file
void read_file(const char *filename) {  // 파일이 길어지면 출력에 문제 생길거 같음
    FILE *file = fopen(filename, "rb");
    if (file == NULL) handle_error("failed to open file: %s", filename);

    FileHeader file_header;
    if (fread(&file_header, sizeof(FileHeader), 1, file) != 1) handle_error("Error reading file header");

    int bucket_num = file_header.bucket_num;
    int *bucket_head = (int *)malloc(sizeof(int) * bucket_num);
    if (bucket_head == NULL) handle_error("Error allocating memory for bucket_head");
    if (fread(bucket_head, sizeof(int), bucket_num, file) != bucket_num) handle_error("Error reading file header");

    printf("|");
    utf8_print_padded("Player", 16, 1);      // 16 column 폭, 왼쪽 정렬
    printf("|%8s|%6s|%11s|%10s|%7s|%8s|%7s|%7s|%7s|%12s|\n",
           "Position", "Game", "Batting Avg", "Home Runs",
           "Era", "Innings", "Wins", "Losses", "Saves", "Strike Outs");
    printf("+----------------+--------+------+-----------+----------+-------+--------+-------+-------+-------+------------+\n");


    PlayerRecord player_record;
    while (fread(&player_record, sizeof(PlayerRecord), 1, file) == 1) {
        printf("|");
        utf8_print_padded(player_record.player, 16, 1);  // 이름 열
        printf("|%8c|%6d|%11.3f|%10d|%7.3f|%8.2f|%7d|%7d|%7d|%12d|\n",
            player_record.position,
            player_record.game,
            player_record.batting_avg,
            player_record.home_runs,
            player_record.era,
            player_record.innings,
            player_record.wins,
            player_record.losses,
            player_record.saves,
            player_record.strikeouts
        );
    }

    fclose(file);
    free(bucket_head);
}

// create new file
void create_file(const char *filename) {
    handle_error("not ready...");
}

// add new player
void add_data(int argc, const char **argv) {
    const char *file_name = argv[2];
    const char *player  = argv[3];

    // open file
    FILE *file = fopen(file_name, "rb+");
    if (file == NULL) handle_error("Error fopen file");

    // read file_header
    FileHeader file_header;
    if (fread(&file_header, sizeof(FileHeader), 1, file) != 1) handle_error("Error fread file header");

    // get offsets of header
    const size_t record_num_offset  = offsetof(FileHeader, record_num);
    const size_t bucket_head_offset = offsetof(FileHeader, bucket_head);

    // check load factor and resize
    file_header.record_num += 1;
    int record_num = file_header.record_num;
    int bucket_num = file_header.bucket_num;
    if (CALC_LOAD_FACTOR(record_num, bucket_num) >= LOAD_FACTOR_THRESHOLD) {
        fclose(file);

        resize_file(file_name);

        file = fopen(file_name, "rb+");
        if (file == NULL) handle_error("Error fopen file");

        if (fread(&file_header, sizeof(FileHeader), 1, file) != 1) handle_error("Error fread file header");
        bucket_num = file_header.bucket_num;
    }
    // update record_num
    if (fseek(file, (long)record_num_offset, SEEK_SET) != 0) handle_error("Error fseek file_header record_num failed");
    if (fwrite(&record_num, sizeof(file_header.record_num), 1, file) != 1) handle_error("Error fwrite file header");

    // read bucket_head
    int old_head;
    unsigned long hashed_name = hash_name(player) & (bucket_num - 1);  // NOTE: bucket_num is always power of two
    long bucket_slot_pos = (long)bucket_head_offset + ((long)sizeof(file_header.bucket_head[0]) * (long)hashed_name);
    if (fseek(file, bucket_slot_pos, SEEK_SET) != 0) handle_error("Error fseek bucket_head failed");
    if (fread(&old_head, sizeof(old_head), 1, file) != 1) handle_error("Error fread file header next offset");

    char *end;
    PlayerRecord player_record = { 0, };
    strncpy(player_record.player, argv[3], NAME_LEN - 1);
    player_record.position    = argv[4][0];
    player_record.game        = (int)   strtol(argv[5],  &end, 10); if (*end != '\0') handle_error("Error reading player");
    player_record.batting_avg = (float) strtod(argv[6],  &end);          if (*end != '\0') handle_error("Error reading player");
    player_record.home_runs   = (int)   strtol(argv[7],  &end, 10); if (*end != '\0') handle_error("Error reading player");
    player_record.era         = (float) strtod(argv[8],  &end);          if (*end != '\0') handle_error("Error reading player");
    player_record.innings     = (float) strtod(argv[9],  &end);          if (*end != '\0') handle_error("Error reading player");
    player_record.wins        = (int)   strtol(argv[10], &end, 10); if (*end != '\0') handle_error("Error reading player");
    player_record.losses      = (int)   strtol(argv[11], &end, 10); if (*end != '\0') handle_error("Error reading player");
    player_record.saves       = (int)   strtol(argv[12], &end, 10); if (*end != '\0') handle_error("Error reading player");
    player_record.strikeouts  = (int)   strtol(argv[13], &end, 10); if (*end != '\0') handle_error("Error reading player");
    player_record.next = old_head;

    // add new record
    if (fseek(file, 0, SEEK_END) != 0) handle_error("Error fseek SEEK_END");
    const long new_record_offset = ftell(file);
    if (new_record_offset == -1L) handle_error("Error ftell get new record offset");
    if (fwrite(&player_record, sizeof(player_record), 1, file) != 1) handle_error("Error fwrite player record");

    // update bucket head
    const int head_value = (int)new_record_offset;
    if (fseek(file, bucket_slot_pos, SEEK_SET) != 0) handle_error("Error fseek bucket head slot position failed");
    if (fwrite(&head_value, sizeof(file_header.bucket_head[0]), 1, file) != 1) handle_error("Error fwrite file header next offset");

    fclose(file);
}

// remove player
void delete_data(const char *filename, const char *player) {
    handle_error("not ready...");
}

// search player
void search_data(const char *filename, const char *player) {
    handle_error("not ready...");
}

// edit player's data
void edit_data(const char *filename, const char *player) {
    handle_error("not ready...");
}

// merge files
void merge_files(int argc, const char **argv) {
    handle_error("not ready...");
}

void handle_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");

    va_end(ap);

    exit(EXIT_FAILURE);
}

// =====================================================================================================================
unsigned long hash_name(const char *str) {
    unsigned long hash = 146527; // FNV offset basis
    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= 16777619; // FNV prime
    }
    return hash;
}


void resize_file(const char *filename) {

}

// ======================================================== GPT ========================================================
#include <wchar.h>
#include <locale.h>

// UTF-8 문자열의 "터미널 폭(column width)" 계산
static int utf8_display_width(const char *s) {
    mbstate_t st;
    memset(&st, 0, sizeof(st));

    const char *p = s;
    int width = 0;
    wchar_t wc;
    size_t len;

    while ((len = mbrtowc(&wc, p, MB_CUR_MAX, &st)) > 0) {
        int w = wcwidth(wc);
        if (w > 0) {
            width += w;
        }
        p += len;
    }
    return width;
}

// width(열 수, column)만큼 차도록 UTF-8 문자열 출력 + 공백 패딩
// left_align != 0 이면 왼쪽 정렬, 0이면 오른쪽 정렬
void utf8_print_padded(const char *s, int width, int left_align) {
    mbstate_t st;
    memset(&st, 0, sizeof(st));

    const char *p = s;
    const char *cut = s;      // 출력 끝낼 위치 (바이트 포인터)
    int cur_width = 0;
    wchar_t wc;
    size_t len;

    // 1) width를 넘지 않는 범위까지 잘라낼 위치 찾기
    while ((len = mbrtowc(&wc, p, MB_CUR_MAX, &st)) > 0) {
        int w = wcwidth(wc);
        if (w < 0) w = 0;
        if (cur_width + w > width) break;
        cur_width += w;
        p += len;
        cut = p;
    }

    int pad = width - cur_width;
    if (pad < 0) pad = 0;

    if (!left_align) {
        // 오른쪽 정렬이면 먼저 공백 출력
        for (int i = 0; i < pad; ++i) putchar(' ');
    }

    // 2) 잘라낸 부분까지만 실제 문자열 출력
    fwrite(s, 1, (size_t)(cut - s), stdout);

    if (left_align) {
        // 왼쪽 정렬이면 뒤에 공백
        for (int i = 0; i < pad; ++i) putchar(' ');
    }
}
