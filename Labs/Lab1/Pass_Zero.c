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

#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
//#include "Pass_Zero.h"

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

//To be removed in final code?
struct MDT *searchMNT(struct MNT *table, char *token);
struct MDT *addMDT(struct MDT **table, char *toAdd, int line);
void addMNT(struct MNT **table, char *toAdd);
void deleteMDT(struct MDT *table);
void deleteMNT(struct MNT *table);

int main() {

    char line[LINE_LENGTH];
	char lineOut[LINE_LENGTH];
	char token1[TOKEN_LENGTH];
	char token2[TOKEN_LENGTH];
	char input_file[] = "TestFiles/fatorial.asm";
	char output_file[] = "TestFiles/output_macro.txt";
	int linec = 0; // Counts the line
    int linem = 0; // Counts the lines that will be on the output file
	int linePos = 0;
	int secText = 0;
	int secData = 0;
	int i = 0;
	int inMacro = 0;
	int firstMacro = 0;
	FILE *fp_in = NULL;
	FILE *fp_out = NULL;
	struct MDT *mdtTable_Head = NULL;
    struct MDT *tmpMDT = NULL;
    struct MNT *mntTable_Head = NULL;
	struct MNT *tmpMNT = NULL;
    struct fileLines *linesTable_Head = NULL;
    struct fileLines *linesTmp = NULL;

    if ((fp_in = fopen(input_file, "r")) == NULL) {
		printf("File not found.\n");
		return 0;
	}

	if ((fp_out = fopen(output_file, "w")) == NULL) {
        printf("File not found!\n");
        return 0;
    }

    while ((GetLine(fp_in, line)) || (strlen(line) > 0)){
        linec++; // Increments line counter

        printf("%s", line);
        linePos = 0;

        /*
        * If line has MACRO directive
        */
        if (strstr(line, " MACRO ") || strstr(line, "MACRO ") || strstr(line, " MACRO\n")) {
            // Checks whether code has a MACRO within another MACRO
            if (!inMacro) {
                inMacro++;
                if (linePos = GetToken(line, token1, linePos)) {
                    if (IsLabel(token1)) {
                        firstMacro = 1;
                        addMNT(&mntTable_Head, token1);
                    }
                }
                modifyLines(linesTable_Head, linec, 0);
            }
            else printf("Line %d. Error: Nested MACRO.\n", linec);
        }

        /*
        * Else, if line has END directive
        */
        else if (!strcmp(line, "END\n")) {
            // To be correctly defined, the line must contain only END
            tmpMDT = addMDT(&mdtTable_Head, line, linec);
            inMacro--;
            modifyLines(linesTable_Head, linec, 0);
        }

        /*
        * Else, line has neither MACRO nor END directive
        */
        else {
            // If line is the first line in MACRO definition
            if ((inMacro) && (firstMacro)) {
                tmpMDT = addMDT(&mdtTable_Head, line, linec);
                mntTable_Head->begin = tmpMDT;
                tmpMDT = NULL;
                firstMacro = 0;
                modify(linesTable_Head, linec, 0);
            }

            // Else, if line is still in MACRO, but it's not longer the first
            else if ((inMacro) && (!firstMacro)) {
                tmpMDT = addMDT(&mdtTable_Head, line, linec);
                modifyLines(linesTable_Head, linec, 0);
            }

            // Else, line is not inside of MACRO definition
            else {
                if (linePos = GetToken(line, token1, linePos)) {
                    tmpMDT = searchMNT(mntTable_Head, token1);

                    // If token1 is found on the MDT, found a MACRO
                    if (tmpMDT != NULL) {
                        while ((strcmp(tmpMDT->line, "END")) && (strcmp(tmpMDT->line, "END "))) {
                            fprintf(fp_out, "%s\n", tmpMDT->line);
                            linem = insertLines(linesTable_Head, tmpMDT->lineNum, linem);
                            linec = linem;
                            tmpMDT = tmpMDT->next;
                        }
                    }

                    // Else, it is not a MACRO
                    else {
                        fprintf(fp_out, "%s", line);
                        linem++;
                        modifyLines(linesTable_Head, linec, linem);
                    }
                }
            }
        }
    }
    // If some MACRO was not finished by the end of the code
    if (inMacro) printf("Error: MACRO is missing END.\n");

    /*
    * After its use during passage zero, MNT and MDT will
    * not be used anymore and can be deleted
    */
    deleteMDT(mdtTable_Head);
    deleteMNT(mntTable_Head);

    if ((fclose(fp_in) == 0) && (fclose(fp_out) == 0)) {
    	printf("\nFiles closed.");
    }

    return 0;
}

/*
* Searches for labels in the MNT
*   It is used to find a label given by the variable token
*/
struct MDT *searchMNT(struct MNT *table, char *token) {

    struct MNT* tmp = table;

    /*
    * Searches whole table until correct label is found
    * or until the end is reached
    */
    while ((tmp != NULL)) {
    	if (!strcmp(tmp->label, token)) {
        	/*
            * If the label was found in the MNT table,
            * returns the pointer for the beginning of
            * the macro definition on the MDT
            */
            return tmp->begin;
        }
        tmp = tmp->next;
    }
    // Else, returns a NULL pointer
    return NULL;
}

/*
* Add the new line to the MDT
*   It is used to add a new line to the MDT, storing the
*   line itself and the number of the line in which it
*   appeared on the original file
*/
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
    /*
    * Whether the line inserted is the first one or just
    * some random line of the table, the function must return
    * a pointer to the line. Then, if the line is the first one
    * in the macro definition, a reference can be added in the MNT.
    */
}

/*
* Add the new label to the MNT
*   It is used to add a new label to the MDT, storing
*   also a pointer that refers to the first line of the macro
*   definition in the MDT.
*/
void addMNT(struct MNT **table, char *toAdd) {

	int i = 0;
	struct MNT* new = (struct MNT*)malloc(sizeof(struct MNT));
	for (i = 0; i < strlen(toAdd); i++) {
		if (toAdd[i] == ':') {
			new->label[i] = '\0';
		}
		else new->label[i] = toAdd[i];
	}
	/*
	* Since the macro definition is yet to be reached,
	* the pointer "begin" is set to NULL by default.
	*/
	new->begin = NULL;
	new->next = *table;
	*table = new;
}

// Deletes the MDT
void deleteMDT(struct MDT *table) {

    struct MDT* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}

// Deletes the MNT
void deleteMNT(struct MNT *table) {

    struct MNT* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}
