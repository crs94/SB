#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct instTab {
    char name[7];
    char opCode[3];
    int opNum;
    struct instTable *pointer;
};

struct instTab *createInsts();
void openFile(char *filename);

int main() {

    char filename[50] = "TestFiles/bin.asm";
    struct instTab *headInst;

    openFile(filename);

    headInst = createInsts();
    printf("\n%s\t%s\t%d", headInst->name, headInst->opCode, headInst->opNum);
    getchar();

    return 0;
}

struct instTab *createInsts() {

    struct instTab *head = NULL;
    head = (struct instTab*) malloc(sizeof(struct instTab));

    strcpy(head->name, "test");
    strcpy(head->opCode, "16");
    head->opNum = 1;

    return head;
}


void openFile(char *filename) {
    char str1;
    FILE *fp;

    if ((fp = fopen(filename, "r")) == NULL) {
        printf("404 Not Found!");
        exit(1);
    }
    if (fp == NULL) {
        printf("File couldn't be opened.");
    }

    while (fscanf(fp, "%c", &str1) == 1){
        printf("%c", str1);
    }
    printf("\nEOF");
    fclose(fp);
}
