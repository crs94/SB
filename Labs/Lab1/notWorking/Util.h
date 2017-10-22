
#ifndef UTIL_H_   /* Include guard */
#define UTIL_H_

extern struct fileLines {
    int lineNum;
    int lineMod;
    struct fileLines *next;
};

FILE *OpenFile(char *filename);

int GetLine(FILE *fp, char *lineBuffer);

int GetToken(char *lineBuffer, char *tokenBuffer, int p);

int IsLabel(char *token);

int IsValid(char *token);

int IsNumber(char *token);

int IsHex(char *token);

int HexToInt(char *token);

void addLines(struct fileLines **table, int original, int modified);

struct fileLines *searchLines(struct fileLines *table, int modified);

int insertLines(struct fileLines *table, int modified, int macroDef);

int modifyLines(struct fileLines *table, int line, int modified);

void deleteLines(struct fileLines *table);

#endif // UTIL_H_
