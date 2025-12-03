//
// Created by ckh on 12/2/25.
//

#ifndef FILE_STRUCTURE_PRACTICE_BASEBALL_FILE_H
#define FILE_STRUCTURE_PRACTICE_BASEBALL_FILE_H

typedef enum {
    READ,
    WRITE,
    ADD,
    DELETE,
    SEARCH,
    EDIT,
    MERGE,
    HELP,
    MODE_NUM
} Mode;

#define BUCKET_NUM 5   // 임시
#define NAME_LEN   64

typedef struct {
    int header_size;
    int bucket_head[];
}FileHeader, *pFileHeader;

typedef struct {
    char  name[NAME_LEN];      // 이름
    char  position;      // 'B' or 'P'
    int   game;          // 출전 경기

    // 타자용
    float batting_avg;   // 평균 타율
    int   home_runs;     // 홈런 수

    // 투수용
    float era;           // 방어율
    float innings;       // 이닝 (ex: 178.2)
    int   wins;          // 승
    int   losses;        // 패
    int   saves;         // 세이브
    int   strikeouts;    // 탈삼진

    // next chain
    int next;
} PlayerRecord, *pPlayerRecord;

#endif //FILE_STRUCTURE_PRACTICE_BASEBALL_FILE_H