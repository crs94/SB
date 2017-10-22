/***********************************************************************
*
* FILE NAME: Pass_Zero.c
*
*
* PURPOSE: 	This function performs macro expansion in the assembly
*			procedure. It is designed to be executed after the original
*			source code was preprocessed using Preprocess.c
*
*
* FILE REFERENCES:
*
*
* EXTERNAL VARIABLES:
*
* Name				Type		IO		Description
* -------------		-------		--		-----------------------------
*
*
* EXTERNAL REFERENCES:
*
* Name					Description
* -------------			-----------------------------
*
*
* ABNORMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES:
*	none
*
*
* ASSUMPTIONS, CONSTRAINTS, RESTRICTIONS: none
*
*
* NOTES:
*
*
***********************************************************************/

//#pragma warning(disable: 4996)
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include "Util.h"

//#define LINE_LENGTH 560
//#define TOKEN_LENGTH 101

struct MNT {
    char label[101];
    struct MDT *begin;
    struct MNT *next;
};

struct MDT {
    char line[601];
    int lineNum;
    struct MDT *next;
};

struct MDT *searchMNT(struct MNT *table, char *token);
struct MDT *addMDT(struct MDT **table, char *toAdd, int line);
void addMNT(struct MNT **table, char *toAdd, struct MDT *first);
void deleteMDT(struct MDT *table);
void deleteMNT(struct MNT *table);

int theZero(char *input_file, struct fileLines **lines_Head) {

    char line[LINE_LENGTH];
	char lineOut[LINE_LENGTH];
	char token1[TOKEN_LENGTH];
	char token2[TOKEN_LENGTH];
	//[] = "TestFiles/fatorial.asm"; // To be replaced by output from preprocess
	char output_file[] = "TestFiles/outzero.txt";
	int linec = 0; // Counts the line
    int linem = 0; // Counts the lines that will be on the output file
	int linePos = 0;
	int secText = 0;
	int secData = 0;
    int tmpLine = 0;
	int i = 0;
	int inMacro = 0;
	int firstMacro = 0;
	FILE *fp_in = NULL;
	FILE *fp_out = NULL;
	struct MDT *mdtTable_Head = NULL;
    struct MDT *tmpMDT = NULL;
    struct MNT *mntTable_Head = NULL;
	struct MNT *tmpMNT = NULL;
    struct fileLines *linesTable_Head = *lines_Head;
    struct fileLines *linesTmp = NULL;

    if ((fp_in = fopen(input_file, "r")) == NULL) {
		printf("File not found.\n");
		return 0;
	}

	if ((fp_out = fopen(output_file, "w")) == NULL) {
        printf("File not found!\n");
        return 0;
    }
    printf("HELLO AGAIN!\n");

    while ((GetLine(fp_in, line)) || (strlen(line) > 0)){
        linec++; // Increments line counter
        
        printf("%s", line);
        linePos = 0;

        /*
        * If line has MACRO directive
        */
        if (strstr(line, " MACRO ") || strstr(line, "MACRO ") || strstr(line, " MACRO\n")) {
            inMacro = 1;
            if (linePos = GetToken(line, token1, linePos)) {
                if (IsLabel(token1)) {
                    firstMacro = 1;
                    //printf("MACRO: %s\n", token1);
                    addMNT(&mntTable_Head, token1, NULL);
                }
            }
            modifyLines(linesTable_Head, linec, 0);
        }
        else if (strstr(line, "END ") || strstr(line, "END\n")) {
            // TODO Think of a new way of comparing this
            //printf("finished MACRO!\n");
            tmpMDT = addMDT(&mdtTable_Head, line, linec);
            inMacro = 0;
            modifyLines(linesTable_Head, linec, 0);
        }
        else {
            if ((inMacro) && (firstMacro)) {
                //printf("first MACRO\n");
                tmpMDT = addMDT(&mdtTable_Head, line, linec);
                mntTable_Head->begin = tmpMDT;
                tmpMDT = NULL;
                firstMacro = 0;
                modifyLines(linesTable_Head, linec, 0);
            }
            else if ((inMacro) && (!firstMacro)) {
                //printf("in MACRO\n");
                tmpMDT = addMDT(&mdtTable_Head, line, linec);
                modifyLines(linesTable_Head, linec, 0);
            }
            else {
                //printf("not MACRO\n");
                if (linePos = GetToken(line, token1, linePos)) {
                    tmpMDT = searchMNT(mntTable_Head, token1);
                    if (tmpMDT != NULL) {
                        //printf("Found in table!\n");
                        while ((strcmp(tmpMDT->line, "END")) && (strcmp(tmpMDT->line, "END "))) {
                            fprintf(fp_out, "%s\n", tmpMDT->line);
                            linem = insertLines(linesTable_Head, tmpMDT->lineNum, linem);
                            linec = linem;
                            tmpMDT = tmpMDT->next;
                        }
                        //printf("End of MACRO\n");
                        //modifyLines(linesTable_Head, ++linem, 0);
                    }
                    else {
                        printf("%d\n", linem);
                        fprintf(fp_out, "%s", line);
                        linem++;
                        modifyLines(linesTable_Head, linec, linem);
                        /*linesTmp = linesTable_Head; // to rmv
                        printf("This is the line table:\n");
                        while (linesTmp != NULL) {
                            printf("%d\t%d\n", linesTmp->lineNum, linesTmp->lineMod);
                            linesTmp = linesTmp->next;
                        }   */                     
                    }
                }
            }
        }
        //printf("EOL\n\n");
    }

    linesTmp = linesTable_Head;
    printf("This is the line table:\n");
    while (linesTmp != NULL) {
        printf("%d\t%d\n", linesTmp->lineNum, linesTmp->lineMod);
        linesTmp = linesTmp->next;
    }

    deleteMDT(mdtTable_Head);
    deleteMNT(mntTable_Head);
    if ((fclose(fp_in) == 0) && (fclose(fp_out) == 0)) {
    	printf("\nFiles closed.");
    }

    return 0;
}

struct MDT *searchMNT(struct MNT *table, char *token) {

    struct MNT* tmp = table;

    // Searches whole table intil correct label is found
    // or until the end is reached
    while ((tmp != NULL)) {
    	//printf("Searching for %s. I'm here: %s\n",token,tmp->label);
        if (!strcmp(tmp->label, token)) {
        	return tmp->begin;
        }
        tmp = tmp->next;
    }
    return NULL;
}

struct MDT *addMDT(struct MDT **table, char *toAdd, int line) {

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
    new->lineNum = line;
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
