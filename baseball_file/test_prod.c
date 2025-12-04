//
// Created by ckh on 12/3/25.
//

// make_players_bin.c
#include <stdio.h>
#include <string.h>

#define NAME_LEN 64

typedef struct {
    int bucket_num;
    int record_num;
    int bucket_head[];   // flexible array (여기서는 직접 안 씀)
} FileHeader;

typedef struct {
    char  name[NAME_LEN];
    char  position;        // 'B' or 'P'
    int   game;

    float batting_avg;
    int   home_runs;

    float era;
    float innings;
    int   wins;
    int   losses;
    int   saves;
    int   strikeouts;

    int   next;            // 체이닝용
} PlayerRecord;

int main(int argc, char **argv) {
    const char *filename = "players.bin";
    if (argc >= 2) {
        filename = argv[1];
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("fopen");
        return 1;
    }

    // ==== 1) 헤더 쓰기 ====
    int bucket_num = 5;
    int record_num = 3;

    // FileHeader의 고정 부분(두 개 int)만 그대로 씀
    struct {
        int bucket_num;
        int record_num;
    } header_fixed;

    header_fixed.bucket_num = bucket_num;
    header_fixed.record_num = record_num;

    if (fwrite(&header_fixed, sizeof(header_fixed), 1, file) != 1) {
        fprintf(stderr, "Error writing header\n");
        return 1;
    }

    // ==== 2) bucket_head 배열 쓰기 ====
    // 인덱스: 레코드 번호 (0,1,2)
    // 여기서는 체이닝 없이 각 버킷 하나씩만 사용
    //   bucket 0: -1
    //   bucket 1: 0 (첫 번째 선수)
    //   bucket 2: 2 (세 번째 선수)
    //   bucket 3: -1
    //   bucket 4: 1 (두 번째 선수)
    int bucket_head[5] = { -1, 0, 2, -1, 1 };

    if (fwrite(bucket_head, sizeof(int), bucket_num, file) != bucket_num) {
        fprintf(stderr, "Error writing bucket_head\n");
        return 1;
    }

    // ==== 3) PlayerRecord들 쓰기 ====
    PlayerRecord rec;
    memset(&rec, 0, sizeof(rec));

    // 레코드 0: 홍길동 (타자)
    memset(&rec, 0, sizeof(rec));
    strncpy(rec.name, "홍길동", NAME_LEN - 1);
    rec.position     = 'B';
    rec.game         = 120;
    rec.batting_avg  = 0.312f;
    rec.home_runs    = 21;
    rec.era          = 0.0f;
    rec.innings      = 0.0f;
    rec.wins         = 0;
    rec.losses       = 0;
    rec.saves        = 0;
    rec.strikeouts   = 0;
    rec.next         = -1;  // 체이닝 없음
    if (fwrite(&rec, sizeof(rec), 1, file) != 1) {
        fprintf(stderr, "Error writing record 0\n");
        return 1;
    }

    // 레코드 1: 김철수 (투수)
    memset(&rec, 0, sizeof(rec));
    strncpy(rec.name, "김철수", NAME_LEN - 1);
    rec.position     = 'P';
    rec.game         = 95;
    rec.batting_avg  = 0.0f;
    rec.home_runs    = 0;
    rec.era          = 3.28f;
    rec.innings      = 178.2f;
    rec.wins         = 13;
    rec.losses       = 7;
    rec.saves        = 0;
    rec.strikeouts   = 134;
    rec.next         = -1;
    if (fwrite(&rec, sizeof(rec), 1, file) != 1) {
        fprintf(stderr, "Error writing record 1\n");
        return 1;
    }

    // 레코드 2: 박민호 (타자)
    memset(&rec, 0, sizeof(rec));
    strncpy(rec.name, "박민호", NAME_LEN - 1);
    rec.position     = 'B';
    rec.game         = 88;
    rec.batting_avg  = 0.285f;
    rec.home_runs    = 11;
    rec.era          = 0.0f;
    rec.innings      = 0.0f;
    rec.wins         = 0;
    rec.losses       = 0;
    rec.saves        = 0;
    rec.strikeouts   = 0;
    rec.next         = -1;
    if (fwrite(&rec, sizeof(rec), 1, file) != 1) {
        fprintf(stderr, "Error writing record 2\n");
        return 1;
    }

    fclose(file);
    printf("Created test binary file: %s\n", filename);
    return 0;
}
