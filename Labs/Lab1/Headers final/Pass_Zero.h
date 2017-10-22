//#ifndef _PASS_ZERO_H_
//#define _PASS_ZERO_H_

extern struct MNT {
    char label[101];
    struct MDT *begin;
    struct MNT *next;
};

extern struct MDT {
    char line[601];
    int lineNum;
    struct MDT *next;
};

struct MDT *searchMNT(struct MNT *table, char *token);

struct MDT *addMDT(struct MDT **table, char *toAdd, int line);

void addMNT(struct MNT **table, char *toAdd, struct MDT *first);

void deleteMDT(struct MDT *table);

void deleteMNT(struct MNT *table);


//#endif
