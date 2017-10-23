
//#ifndef _PREPROCESS_H_
//#define _PREPROCESS_H_

extern struct fileLines {
    int lineNum;
    int lineMod;
    struct fileLines *next;
};

struct equ_tab *SearchEQU(struct equ_tab *table, char *token);

void AddEQU(struct equ_tab **table, char *label, char *digit);

void DeleteEQU(struct equ_tab *table); 

//#endif
