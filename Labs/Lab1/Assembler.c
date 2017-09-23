#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct instTab {
    char inst[7];
    char opcode[3];
    int operator;
};

struct dirTab {
	char dir[7];
	int operator;
	int size;
};

struct mnt {
	char name[101];
	struct mdt *beginning;
	struct mnt *next;
};

struct mdt {
	char line[51];
	struct mdt *next;
};

struct symTab {
	char label[101];
	int memWhere;
	struct places *list;
	int defined;
	int vector;
	struct symTab *next;
};

struct instTab **createInsts();
void openFile(char *filename);

int main() {

    char filename[50] = "TestFiles/bin.asm";
    struct instTab **headInst;

    openFile(filename);

    headInst = createInsts();
    printf("\n%s\t%s\t%d", headInst[0]->inst, headInst[0]->opcode, headInst[0]->operator);
    getchar();

    return 0;
}

struct instTab **createInsts() {

    struct instTab *ptr[2];
    ptr[0] = NULL;
    ptr[0] = (struct instTab*) malloc(sizeof(struct instTab));

    strcpy(ptr[0]->inst, "test");
    strcpy(ptr[0]->opcode, "16");
    ptr[0]->operator = 1;

    return ptr;
}


void openFile(char *filename) {
    char str1[50];
    int i = 0;
    FILE *fp;

    if ((fp = fopen(filename, "r")) == NULL) {
        printf("404 Not Found!");
        exit(1);
    }
    if (fp == NULL) {
        printf("File couldn't be opened.");
        exit(1);
    }

    /*while (fscanf(fp, "%c", &str1) == 1){*/
    while (fgets(str1, 50, fp) != NULL){
        printf("%s", str1);
        i++;
    }
    printf("\nEOF and i = %d", i);
    fclose(fp);
}
