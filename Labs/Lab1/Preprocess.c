/***********************************************************************
*
* FILE NAME: Preprocess.c
*
*
* PURPOSE: 	This function performs the preprocessing step in the assembly 
* 			procedure. It checks if a line of the input source code contains
*			an EQU or IF directive, replaces the labels defined in EQU
*			directives throughout the code, and includes the line following
*			an IF directive only if its operand is 1.
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
* 
*
*
* ASSUMPTIONS, CONSTRAINTS, RESTRICTIONS:
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
#include "Util.h"

#define LINE_LENGTH 560
#define TOKEN_LENGTH 101

/* 
 *This structure holds the symbol/label defined in an EQU directive
 * and the value by which it is to be replaced.
 */
struct equ_tab {
	char label[TOKEN_LENGTH];
	char value[TOKEN_LENGTH];
	struct equ_tab *next;
};

// This will be gone in final code. No need to comment
static struct equ_tab *SearchEQU(struct equ_tab *table, char *token);
static void AddEQU(struct equ_tab **table, char *label, char *digit);
static void DeleteEQU(struct equ_tab *table);

int main() {

	char line[LINE_LENGTH];
	char lineOut[LINE_LENGTH];
	//char aux[LINE_LENGTH]; // Necessary?
	char token1[TOKEN_LENGTH];
	char token2[TOKEN_LENGTH];
	char input_file[] = "TestFiles/SB_test_getline.asm"; 
	char output_file[] = "TestFiles/output.txt";
	int linec = 0;
	int linem = 0;
	int linePos = 0;
	int secText = 0;
	int secData = 0;
	int endFile = 0;
	int i = 0;
	FILE *fp_in = NULL;
	FILE *fp_out = NULL;
	struct equ_tab *equTable_Head = NULL;
	struct equ_tab *tmp = NULL;
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

    line[0] = '\0';
    lineOut[0] = '\0';

    while ((GetLine(fp_in, line)) || (strlen(line) > 0)) {
    	linec++; // Increments line counter
		addLines(&linesTable_Head, linec, linec); // Adds line in line reference table
		
		/*
		 * If line is not only blanks and/or '\n'
		 * the function tries to identify its parts
		 */
	    if (strlen(line) > 0) {
	    
	    	/*
	    	 * Despite the rest of the line, if the last thing
	    	 * is a label, one must concatenate its content
	    	 * with the content found on the next valid line.
	    	 * Deal with errors on passage one
	    	 */
	    	if (line[strlen(line)-2] == ':') {
    			line[strlen(line)-1] = '\0';
    			modifyLines(linesTable_Head, linec, 0);
    			printf("%s", line);
    			do {
    				// Repeat while line == (blanks and/or '\n')
    				if (!GetLine(fp_in, lineOut)) {
    					endFile = 1;
    					break;
    				}
    				linec++;
					addLines(&linesTable_Head, linec, 0);
    			} while (strlen(lineOut) == 0);
    			if (!endFile) {
	    			strcat(line, " ");
	    			printf("%s", line);
	    			strcat(line, lineOut);
	    			printf("%s", line);
	    		}
	    		else printf("Line only has label.\n");
    		}
    		
    		/*
    		* If line has an EQU directive
    		*/
	    	if (strstr(line, " EQU ") || strstr(line, "EQU ") || strstr(line, " EQU\n")) {
	    		modifyLines(linesTable_Head, linec, 0);
	    		if(linePos = GetToken(line, token1, linePos)) {
		    		// Checks if first token is a valid label
		    		if(IsLabel(token1)) {
		    			// Checks if there is another token in the line
		    			if(linePos = GetToken(line, token2, linePos)) {
		    				// Checks if token is an EQU
							if(!strcmp(token2,"EQU")) {
								// Checks if EQU has an operand
								if(linePos = GetToken(line, token2, linePos)) {
									// Checks if operand if valid
									if(IsValid(token2)) {
										// Checks if label is already in EQU table
										tmp = SearchEQU(equTable_Head, token1);
										// Label was found in the table
										if(tmp != NULL) {
											// Redefines value by which label is to be replaced
											strcpy(tmp->value, token2);
										}
										// Line contains a new EQU definition
										else {
											/* Checks if the operand of EQU is a label previously 
											 * defined in another EQU, in which case the new label
											 * will have the same value as the previous one */
											tmp = SearchEQU(equTable_Head, token2);
											if(tmp != NULL) {
												// Replaces value of the operand
												strcpy(token2, tmp->value);
											}
											// Adds pair Label-Value to EQU table
											AddEQU(&equTable_Head, token1, token2);
										}
									}
									else {
										printf("Line %d. Error: Invalid token %s.\n", linec, token2);
									}
								}
								else {
									printf("Line %d. Error: Expected operand after EQU.\n", linec);
								}
							}
							/*else {
								tratar casos: duas labels,
							}*/
						}
						//else?
		    		}
		    		/*else {
		    			erros:  EQU com label inválida ou sem label
		    			ação a tomar:?
		    			tratar caso label = "EQU:" ou label é o nome de inst?
		    		}*/
		    	}
		    }	
		    	    
		    else {
		    	/*
    		 	 *	Else, if line has an IF directive
    		  	 */
		    	if(strstr(line, "IF ") || strstr(line, " IF ") || strstr(line, " IF\n")) {
		    		modifyLines(linesTable_Head, linec, 0);
		    		if(linePos = GetToken(line, token1, linePos)) {
		    			// Checks if first token is an IF
		    			if(!strcmp(token1, "IF")) {
		    				// Checks whether IF directive has an operand
		    				if(linePos = GetToken(line, token1, linePos)) {
		    					// Seaches for the operand in EQU table
		    					tmp = SearchEQU(equTable_Head, token1);
		    					// Operand was found in the table or is a zero or is a one
		    					if((tmp != NULL) || (!strcmp(token1, "0")) || (!strcmp(token1, "1"))) {
		    						// Checks if there is more than one operand
		    						if(!(linePos = GetToken(line, token1, linePos))) {
		    							// Skips line if operand is a zero
		    							if((!strcmp(tmp->value, "0")) || (!strcmp(token1, "0"))) {
		    								if (GetLine(fp_in, line)) {
			    								linec++;
			    								addLines(&linesTable_Head, linec, 0);
		    								}
		    							}
		    							else {
		    								// Checks if operand is not a one
		    								if((strcmp(tmp->value, "1")) || (strcmp(token1, "1"))) {
												printf("Line %d. Error: Operand of IF directive must be 0 or 1.\n", linec);
		    								}
		    							}
		    						}
		    						else {
		    							printf("Line %d. Error: Incorrect number of operands in IF directive.\n", linec);
		    						}
		    					}
		    					else {
		    						// Operand of IF was not zero, one or a label in EQU table
		    						printf("Line %d. Error: Undefined operand in IF directive.\n", linec);
		    					}
		    				}
		    				else {
		    					printf("Line %d. Error: Expected operand after IF directive.\n", linec);
		    				}
		    			}
		    			/*else {
		    				erro: IF com label
		    			}*/
		    		}
		    	}
		    	
		    	/*
    		 	 *	Insert brief explanation of this section.
    		 	 */
		    	else {		    	
					// Gets tokens one by one and searches them in EQU table
		    		while (linePos = GetToken(line, token1, linePos)) {
	    				tmp = SearchEQU(equTable_Head, token1);
	    				if(tmp != NULL) {
		    				// If token was found in table it is replaced
		    				strcpy(token1, tmp->value);
	 					}

						// Inserts tokens in output line separated by white space
	    				strcat(lineOut, token1);
	    				strcat(lineOut, " ");
				   	}
				   	lineOut[strlen(lineOut) - 1] = '\n'; //what if line is full?
		    		fprintf(fp_out, "%s", lineOut);
		    		linem++;
		    		modifyLines(linesTable_Head, linec, linem);
	    		}
	    		
	    	}
	    	
	    	i = 0;
	    	lineOut[0] = '\0';
	    	linePos = 0;
	    }
	    /*
	    * Else, if line only has blanks and/or '\n'
	    * the table that stores the references for the
	    * lines is modified so the actual reference
	    * is for a non existent line on the output file
	    */
	    else modifyLines(linesTable_Head, linec, 0);
    }

    DeleteEQU(equTable_Head);
    deleteLines(linesTable_Head);
    if ((fclose(fp_in) == 0) && (fclose(fp_out) == 0)) { // What to do if error occurs?
    	printf("\nFiles closed.");
    }

    return 0;
}

/*
 * This funcion searches the EQU table for token and returns a pointer
 * for the node in which token was found or NULL if it was not found
 */
static struct equ_tab *SearchEQU(struct equ_tab *table, char *token) {

    struct equ_tab* tmp = table;
    while ((tmp != NULL)) {
    	printf("Searching for %s. I'm here: %s\n",token,tmp->label);
        if (!strcmp(tmp->label, token)) {
        	return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

/*
 * This funcion inserts a node in EQU table with label and value
 * defined by name and digit, respectively
 */
static void AddEQU(struct equ_tab **table, char *name, char *digit) {

	int i = 0;
	struct equ_tab* new = (struct equ_tab*)malloc(sizeof(struct equ_tab));
	for (i = 0; i < strlen(name); i++) {
		if (name[i] == ':') {
			new->label[i] = '\0';
		}
		else new->label[i] = name[i];
	}
	strcpy(new->value, digit);
	new->next = *table;
	*table = new;
}

/*
 * This funcion deletes the EQU table, freeing the memory previously
 * allocated
 */
static void DeleteEQU(struct equ_tab *table) {

    struct equ_tab* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}
