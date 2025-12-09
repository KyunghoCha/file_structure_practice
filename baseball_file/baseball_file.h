//
// Created by ckh on 12/2/25.
//

#ifndef FILE_STRUCTURE_PRACTICE_BASEBALL_FILE_H
#define FILE_STRUCTURE_PRACTICE_BASEBALL_FILE_H

typedef enum {
    READ,
    CREATE,
    ADD,
    DELETE,
    SEARCH,
    EDIT,
    MERGE,
    HELP,
    MODE_NUM,
    UNKNOWN_MODE
} Mode;

typedef enum {
    PROGRAM_NAME,
    MODE,
    FILE_NAME,

    PLAYER,
    POSITION,
    GAME,

    BATTING_AVG,
    HOME_RUNS,

    ERA,
    INNINGS,
    WINS,
    LOSSES,
    SAVES,
    STRIKEOUTS,

    MAX_ARGC
} RecordArg;

#define LOAD_FACTOR_THRESHOLD 0.7
#define INITIAL_BUCKET_NUM    16
#define NAME_LEN              64

#define CALC_LOAD_FACTOR(record_num, bucket_num) \
    ((double)(record_num) / (double)(bucket_num))

typedef struct {
    int bucket_num;
    int record_num;
    int bucket_table_head[];
}FileHeader;

typedef struct {
    char  player[NAME_LEN];  // 이름
    char  position;          // 'B' or 'P'
    int   game;              // 출전 경기

    // 타자용
    float batting_avg;       // 평균 타율
    int   home_runs;         // 홈런 수

    // 투수용
    float era;               // 방어율
    float innings;           // 이닝 (ex: 178.2)
    int   wins;              // 승
    int   losses;            // 패
    int   saves;             // 세이브
    int   strikeouts;        // 탈삼진

    // next chain
    long next;                // next chain by name hash
} PlayerRecord;

#endif //FILE_STRUCTURE_PRACTICE_BASEBALL_FILE_H