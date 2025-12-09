//
// Created by ckh on 12/2/25.
//

#define _GNU_SOURCE  // GPT

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

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
    read_header(file, &file_header);

    int bucket_num = file_header.bucket_num;
    long *bucket_table_head = NULL;
    if ((bucket_table_head = (long *)malloc(sizeof(long) * bucket_num)) == NULL) handle_error("Error allocating memory for bucket_head.");
    if (fread(bucket_table_head, sizeof(long), bucket_num, file) != bucket_num) handle_error("Error reading file header.");


    printf("%d %d\n", bucket_num, file_header.record_num); // TODO test
    printf("+----------------+--------+------+-----------+----------+-------+--------+-------+-------+-------+------------+\n");
    printf("|");
    utf8_print_padded("Player", 16, 1);      // 16 column 폭, 왼쪽 정렬
    printf("|%8s|%6s|%11s|%10s|%7s|%8s|%7s|%7s|%7s|%12s|\n",
           "Position", "Game", "Batting Avg", "Home Runs",
           "Era", "Innings", "Wins", "Losses", "Saves", "Strike Outs");
    printf("+----------------+--------+------+-----------+----------+-------+--------+-------+-------+-------+------------+\n");


    PlayerRecord p_rec;
    while (fread(&p_rec, sizeof(PlayerRecord), 1, file) == 1) {
        printf("|");
        utf8_print_padded(p_rec.player, 16, 1);  // 이름 열
        printf("|%8c|%6d|%11.3f|%10d|%7.3f|%8.2f|%7d|%7d|%7d|%12d| %5ld\n",  // TODO test
            p_rec.position,
            p_rec.game,
            p_rec.batting_avg,
            p_rec.home_runs,
            p_rec.era,
            p_rec.innings,
            p_rec.wins,
            p_rec.losses,
            p_rec.saves,
            p_rec.strikeouts,
            p_rec.next

        );
    }
    printf("+----------------+--------+------+-----------+----------+-------+--------+-------+-------+-------+------------+\n");
    for (int i = 0; i < bucket_num; i++) { // TODO test
        printf("%ld ", bucket_table_head[i]);
    } printf("\n");

    fclose(file);
    free(bucket_table_head);
}

// create new file
void create_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        fclose(file);
        handle_error("The file is already exist: %s", filename);
    }

    file = fopen(filename, "wb");

    FileHeader file_header = {
        .bucket_num = INITIAL_BUCKET_NUM,
        .record_num = 0
    };
    long *bucket_table_head = NULL;
    if ((bucket_table_head = (long *)malloc(sizeof(long) * INITIAL_BUCKET_NUM)) == NULL) handle_error("Error allocating memory for bucket_table_head.");
    memset(bucket_table_head, -1, sizeof(long) * INITIAL_BUCKET_NUM);

    write_header(file, &file_header);
    if (fwrite(bucket_table_head, sizeof(long), INITIAL_BUCKET_NUM, file) != INITIAL_BUCKET_NUM) handle_error("Error writing file header.");

    fclose(file);
    free(bucket_table_head);
}

// add new player
void add_data(const char **argv) {
    const char *filename = argv[FILE_NAME];

    FILE *file = fopen(filename, "r+b");
    if (file == NULL) handle_error("failed to open file: %s\n", filename);

    FileHeader file_header;
    read_header(file, &file_header);

    // check load factor and resize
    if (CALC_LOAD_FACTOR(file_header.record_num + 1, file_header.bucket_num) >= LOAD_FACTOR_THRESHOLD) {
        fclose(file);

        resize_file(filename);

        file = fopen(filename, "r+b");
        if (file == NULL) handle_error("Error fopen file.");

        read_header(file, &file_header);
    }
    file_header.record_num += 1;
    write_header(file, &file_header);

    unsigned long bucket_idx = hash_name(argv[PLAYER]) & (file_header.bucket_num - 1);  // NOTE: bucket_num is always power of two
    long old_head = read_bucket_slot_head(file, bucket_idx);

    PlayerRecord p_rec = { 0, };
    set_record(argv, &p_rec, old_head);

    long new_record_offset = append_record(file, &p_rec);

    write_bucket_slot_head(file, (long)bucket_idx, &new_record_offset);

    fclose(file);
}

// remove player
void delete_data(const char *filename, const char *player) {
    handle_error("not ready...");
}

// search player
void search_data(const char *filename, const char *player) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) handle_error("failed to open file: %s\n", filename);

    FileHeader file_header;
    PlayerRecord p_rec = { 0, };
    read_header(file, &file_header);

    long bucket_idx = hash_name(player) & (file_header.bucket_num - 1);
    long next_record_offset = read_bucket_slot_head(file, bucket_idx);

    int same_name_num = 0;
    while (next_record_offset != -1) {
        read_record(file, next_record_offset, &p_rec);
        next_record_offset = p_rec.next;

        if (strcmp(p_rec.player, player) == 0) {
            printf("%s %c %d %.3f %d %.3f %.3f %d %d %d %d\n",
                p_rec.player,
                p_rec.position,
                p_rec.game,
                p_rec.batting_avg,
                p_rec.home_runs,
                p_rec.era,
                p_rec.innings,
                p_rec.wins,
                p_rec.losses,
                p_rec.saves,
                p_rec.strikeouts
            );
            same_name_num += 1;
        }
    }

    if (same_name_num == 1)
        printf("Found 1 player.\n");
    else
        printf("Found %d players who have the same name.\n", same_name_num);

    fclose(file);
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

// ====================================================== helper ======================================================
// NOTE: bucket_num is always power of two
unsigned long hash_name(const char *str) {
    unsigned long hash = 146527; // FNV offset basis
    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= 16777619; // FNV prime
    }
    return hash;
}

void resize_file(const char *filename) {
    FILE *old_file = fopen(filename, "rb");
    if (old_file == NULL) handle_error("failed to open file: %s\n", filename);

    // open temp file
    char tmp_filename[64] = "tmp_file.bin";
    FILE *tmp_file = fopen(tmp_filename, "rb");
    if (tmp_file == NULL) {
        tmp_file = fopen(tmp_filename, "w+b");
        if (tmp_file == NULL) handle_error("Error opening tmp file: %s\n", tmp_filename);
    } else {
        fclose(tmp_file);
        int i = 0;
        while (i < 1000) {
            snprintf(tmp_filename, sizeof(tmp_filename), "%s_%03d", "tmp_file.bin", i);
            tmp_file = fopen(tmp_filename, "rb");
            if (!tmp_file) {
                tmp_file = fopen(tmp_filename, "w+b");
                break;
            }
            fclose(tmp_file);
            i++;
        }

        if (i == 1000) handle_error("Fatal Error can't make more tmp file.");
    }

    // write header to tmp_file
    FileHeader file_header;
    read_header(old_file, &file_header);
    file_header.bucket_num <<= 1;
    write_header(tmp_file, &file_header);

    long *bucket_table_head = NULL;
    if ((bucket_table_head = (long *)malloc(sizeof(long) * file_header.bucket_num)) == NULL) handle_error("Error allocating memory for bucket_table_head.");
    memset(bucket_table_head, -1, sizeof(long) * file_header.bucket_num);

    if (fwrite(bucket_table_head, sizeof(long), file_header.bucket_num, tmp_file) != file_header.bucket_num) handle_error("Error writing file header.");

    int old_bucket_num = file_header.bucket_num >> 1;
    if (fseek(old_file, sizeof(long) * old_bucket_num, SEEK_CUR) != 0) handle_error("Error seeking to beginning of file.");

    // write new bucket_table_head to tmp_file
    PlayerRecord p_rec = { 0, };
    for (int i = 0; i < file_header.record_num; i++) {
        if (fread(&p_rec, sizeof(PlayerRecord), 1, old_file) != 1)
            handle_error("Error reading player record.");

        unsigned long bucket_idx = hash_name(p_rec.player) & (file_header.bucket_num - 1);

        long prev_head = read_bucket_slot_head(tmp_file, bucket_idx);
        p_rec.next = prev_head;

        long new_record_offset = append_record(tmp_file, &p_rec);

        write_bucket_slot_head(tmp_file, bucket_idx, &new_record_offset);
    }

    fclose(old_file);
    fclose(tmp_file);
    free(bucket_table_head);

    if (rename(tmp_filename, filename) != 0) handle_error("Error renaming tmp file to %s\n", filename);
}

void read_header(FILE *file, FileHeader *header) {
    if (fseek(file, 0, SEEK_SET) != 0) handle_error("Error fseek SEEK_SET.");
    if (fread(header, sizeof(*header), 1, file) != 1) handle_error("Error fread header.");
}

void write_header(FILE *file, const FileHeader *header) {
    if (fseek(file, 0, SEEK_SET) != 0) handle_error("Error fseek SEEK_SET.");
    if (fwrite(header, sizeof(*header), 1, file) != 1) handle_error("Error fwrite header.");
}

long get_bucket_slot_offset(unsigned long bucket_index) {
    return sizeof(FileHeader) + (sizeof(long) * bucket_index);
}

long read_bucket_slot_head(FILE *file, unsigned long bucket_index) {
    long bucket_value;

    if (fseek(file, get_bucket_slot_offset(bucket_index), SEEK_SET) != 0) handle_error("Error fseek SEEK_SET.");
    if (fread(&bucket_value, sizeof(long), 1, file) != 1) handle_error("Error fread bucket_value.");

    return bucket_value;
}

void write_bucket_slot_head(FILE *file, unsigned long bucket_index, const long *new_record_offset) {
    if (fseek(file, get_bucket_slot_offset(bucket_index), SEEK_SET) != 0) handle_error("Error fseek SEEK_SET.");
    if (fwrite(new_record_offset, sizeof(long), 1, file) != 1) handle_error("Error fwrite new_record_offset.");
}

// set new record
void set_record(const char **argv, PlayerRecord *player, long old_head) {
    strncpy(player->player, argv[PLAYER], NAME_LEN - 1);
    player->player[NAME_LEN - 1] = '\0';

    if (*argv[POSITION] == '\0') handle_error("Error reading position.");
    player->position = argv[POSITION][0];

    player->game        = parse_int(argv[GAME]);
    player->batting_avg = parse_float(argv[BATTING_AVG]);
    player->home_runs   = parse_int(argv[HOME_RUNS]);
    player->era         = parse_float(argv[ERA]);
    player->innings     = parse_float(argv[INNINGS]);
    player->wins        = parse_int(argv[WINS]);
    player->losses      = parse_int(argv[LOSSES]);
    player->saves       = parse_int(argv[SAVES]);
    player->strikeouts  = parse_int(argv[STRIKEOUTS]);

    player->next = old_head;
}

long append_record(FILE *file, PlayerRecord *p_rec) {
    if (fseek(file, 0, SEEK_END) != 0) handle_error("Error fseek SEEK_END.");
    long new_record_offset = ftell(file);
    if (new_record_offset < 0) handle_error("Error ftell while append.");
    if (fwrite(p_rec, sizeof(PlayerRecord), 1, file) != 1) handle_error("Error fwrite player_record.");
    return new_record_offset;
}

void read_record(FILE *file, long record_offset, PlayerRecord *p_rec) {
    if (fseek(file, record_offset, SEEK_SET) != 0) handle_error("Error fseek SEEK_SET.");
    if (fread(p_rec, sizeof(PlayerRecord), 1, file) != 1) handle_error("Error fread player_record.");
}

// GPT
static int parse_int(const char *arg) {
    char *end;
    long val = strtol(arg, &end, 10);
    if (end == arg || *end != '\0') handle_error("Invalid integer input.");
    return (int) val;
}

// GPT
static float parse_float(const char *arg) {
    char *end;
    float val = strtof(arg, &end);
    if (end == arg || *end != '\0') handle_error("Invalid float input.");
    return val;
}


// ======================================================== GPT ========================================================
#include <wchar.h>
#include <locale.h>

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
