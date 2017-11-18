//#ifndef UTIL_H_   /* Include guard */
//#define UTIL_H_

#define LINE_LENGTH 560
#define TOKEN_LENGTH 101

extern struct fileLines {
    int lineNum;
    int lineMod;
    struct fileLines *next;
};


int GetLine(FILE *fp, char *lineBuffer);

int GetToken(char *lineBuffer, char *tokenBuffer, int *p);

int IsLabel(char *token);

int IsValid(char *token);

int IsNumber(char *token);

int IsHex(char *token);

int Power(int base, int n);

int HexToInt(char *token);

void addLines(struct fileLines **table, int original, int modified);

struct fileLines *searchLines(struct fileLines *table, int modified);

int insertLines(struct fileLines *table, int original, int modified);

int modifyLines(struct fileLines *table, int line, int modified);

void deleteLines(struct fileLines *table);

#endif // UTIL_H_

