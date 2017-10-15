#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Util.h"

struct MNT {
    char label[101];
    struct MDT *begin;
    struct MNT *next;
};

struct MDT {
    char line[601];
    struct MDT *next;
};

struct MDT *searchMNT(struct MNT *table, char *token);
struct MDT *addMDT(struct MDT **table, char *toAdd);
void addMNT(struct MNT **table, char *toAdd, struct MDT *first);
void deleteMDT(struct MDT *table);
void deleteMNT(struct MNT *table);

int main() {

    char line[601], lineOut[601];
	char token1[101], token2[101];
	char filename[] = "TestFiles/fatorial.asm", output[] = "TestFiles/output.txt";
    FILE *fp = NULL;
	FILE *out = NULL;
	int linePos = 0, i = 0, secText = 0, secData = 0, inMacro = 0, firstMacro = 0;
	struct MDT *mdtTable_Head = NULL;
    struct MDT *tmpMDT = NULL;
    struct MNT *mntTable_Head = NULL;
	struct MNT *tmpMNT = NULL;

    if ((fp = fopen(filename, "r")) == NULL) {
        printf("404 Not Found!");
        exit(1);
    }

    if ((out = fopen(output, "w")) == NULL) {
        printf("Could open!");
        exit(1);
    }

    while (getLine(fp, line)){
        printf("%s", line);
        linePos = 0;
        if (strstr(line, " MACRO ") || strstr(line, "MACRO ") || strstr(line, " MACRO\n")) {
            inMacro = 1;
            if (linePos = getToken(line, token1, linePos)) {
                if (isLabel(token1)) {
                    firstMacro = 1;
                    printf("MACRO: %s\n", token1);
                    addMNT(&mntTable_Head, token1, NULL);
                }
            }
        }
        else if (strstr(line, "END ") || strstr(line, "END\n")) {
            // TODO Think of a new way of comparing this
            printf("finished MACRO!\n");
            tmpMDT = addMDT(&mdtTable_Head, line);
            inMacro = 0;
        }
        else {
            if ((inMacro) && (firstMacro)) {
                printf("first MACRO\n");
                tmpMDT = addMDT(&mdtTable_Head, line);
                mntTable_Head->begin = tmpMDT;
                tmpMDT = NULL;
                firstMacro = 0;
            }
            else if ((inMacro) && (!firstMacro)) {
                printf("in MACRO\n");
                tmpMDT = addMDT(&mdtTable_Head, line);
            }
            else {
                printf("not MACRO\n");
                if (linePos = getToken(line, token1, linePos)) {
                    tmpMDT = searchMNT(mntTable_Head, token1);
                    if (tmpMDT != NULL) {
                        printf("Found in table!\n");
                        while ((strcmp(tmpMDT->line, "END")) && (strcmp(tmpMDT->line, "END "))) {
                            fprintf(out, "%s\n", tmpMDT->line);
                            tmpMDT = tmpMDT->next;
                        }
                        printf("End of MACRO\n");
                    }
                    else fprintf(out, "%s", line);
                }
            }
        }
        printf("EOL\n\n");
    }

    deleteMDT(mdtTable_Head);
    deleteMNT(mntTable_Head);
    if ((fclose(fp) == 0) && (fclose(out) == 0)) {
    	printf("\nFiles closed.");
    }

    return 0;
}

struct MDT *searchMNT(struct MNT *table, char *token) {

    struct MNT* tmp = table;

    // Searches whole table intil correct label is found
    // or until the end is reached
    while ((tmp != NULL)) {
    	printf("Searching for %s. I'm here: %s\n",token,tmp->label);
        if (!strcmp(tmp->label, token)) {
        	return tmp->begin;
        }
        tmp = tmp->next;
    }
    return NULL;
}

struct MDT *addMDT(struct MDT **table, char *toAdd) {

	int i = 0;
	struct MDT* tmp = *table;
    struct MDT* before = NULL;
	struct MDT* new = (struct MDT*)malloc(sizeof(struct MDT));
	
    // Removes LF from lines before inserting into table
    for (i = 0; i < strlen(toAdd); i++) {
        if (toAdd[i] == '\n') {
            new->line[i] = '\0';
        }
        else new->line[i] = toAdd[i];
    }
    new->next = NULL;

    // If the table is empty, insert first node
	if (*table == NULL) {
        *table = new;
        return *table;
    }

    // Else, search for empty space
    while (tmp != NULL) {
        before = tmp;
        tmp = tmp->next;
	}
    before->next = new;
    return tmp;
}

void addMNT(struct MNT **table, char *toAdd, struct MDT *first) {

	int i = 0;
	struct MNT* new = (struct MNT*)malloc(sizeof(struct MNT));
	for (i = 0; i < strlen(toAdd); i++) {
		if (toAdd[i] == ':') {
			new->label[i] = '\0';
		}
		else new->label[i] = toAdd[i];
	}
	new->begin = first;
	new->next = *table;
	*table = new;
}

void deleteMDT(struct MDT *table) {

    struct MDT* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}

void deleteMNT(struct MNT *table) {

    struct MNT* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}
