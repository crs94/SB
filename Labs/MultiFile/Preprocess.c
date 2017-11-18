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
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "Preprocess.h"

/*
 *This structure holds the symbol/label defined in an EQU directive
 * and the value by which it is to be replaced.
 */
struct equ_table_node {
	char label[TOKEN_LENGTH];
	char value[TOKEN_LENGTH];
	struct equ_table_node *next;
};

//int Preprocess(FILE *fp_in, FILE *fp_out, struct fileLines **linesTable_Head, int *error_count) {
int main(int argc, char *argv[]) {

	int error_count = 0;

	char line[LINE_LENGTH];
	char lineOut[LINE_LENGTH];
	char token[TOKEN_LENGTH];
	char *input_file = argv[1];
	char output_file[] = "TestFiles/output_pre.txt";
	int line_count = 0; // Counts the lines
	int linem = 0; // Counts the lines that will be on the output file
	int linePos;
	int sec = -1;
	//int endFile = 0;
	int label_count;
	int equ_count;
	int if_count;
	int token_count;
	int write = 1;
	FILE *fp_in = NULL;
	FILE *fp_out = NULL;
	struct equ_table_node *equTable = NULL;
	struct equ_table_node *tmp = NULL;
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
    //line[0] = '\0';
    //lineOut[0] = '\0';

    while(GetLine(fp_in, line)) {
    
    	line_count++
    	linePos = 0;
    		//Se linha anterior não tinha apenas label:
    		tmp = NULL;
    		token_count = 0;
    		lineOut[0] = '\0';
    		modifyLines(*linesTable_Head, linec, 0);
    	equ_count = 0;
    	if_count = 0;
    	label_count = 0;
		
    	while(GetToken(line, token, &linePos)) {
    		token_count++;
    		if(IsLabel(token)) {
    			if(!label_count) {
					AddEqu(&equTable, token, "\0");
					tmp = table;
					label_count++;
					strcat(lineOut, token);
					strcat(lineOut, " ");
				}
				else {
					//erro: duas labels
					//pula linha?
				}
    		}
    		else if(!strcmp(token, "EQU")) {
    			modifyLines(*linesTable_Head, linec, 0);
    			equ_count++;
    			write = 0;
    		}
    		else if(!strcmp(token, "IF")) {
    			modifyLines(*linesTable_Head, linec, 0);
    			if_count++;
    			write = 0;
    		}
    		else if(if_count {
    			if(label_count) {
    				//erro
    			}
    			if(!IsNumber(token)) {
					//erro
				}
				if(GetToken(line, token, &linePos)) {
    				//erro
    			}
				else if((atoi(token)) == 0) {
					line_count++;
					addLines(linesTable_Head, linec, linec);
					if(!GetLine(fp_in, line)) {
						//erro
					}
					if_count = 0;
				}
    		}
    		else if(equ_count) {			 
    			if(!label_count) {
    				//erro
    			}
    			else if(label_count > 1) {
    				//erro
    			}
    			else if(GetToken(line, token, &linePos)) {
    				//erro
    			}
				else if(!IsValid(token)) {
					//erro
				}
				else {
					//if(tmp->value == '\0') ?
					//tmp2 = SearchEqu(equTable, token);
					//if(tmp2 != NULL) strcpy(token, tmp2->value);
					strcpy(tmp->value, token);					
					equ_count = 0;
				}
    		}
    		else {
    			tmp = SearchEQU(equTable_Head, token1);
				if(tmp != NULL) {
					// If token was found in table it is replaced
					strcpy(token1, tmp->value);
				}
				if(!strcmp(token1, "SECTION")) {
					sec = 0;
				}
				else {
					if(!sec) {
						if(!strcmp(token1, "TEXT")) {
							sec = 1;
						}
						else if(!strcmp(token1, "DATA")) {
							sec = 2;
						}
						else sec = 3; //unknown section
					}
				}
				strcat(lineOut, token1);
				strcat(lineOut, " ");
    		}
    	}
    	
    	if(if_count || equ_count) {
    		//erro: falta operando
		}
		
		if(write && (token_count == label_count)) {
			strcat(lineOut, "\n");
			fprintf(fp_out, "%s", lineOut);
		}
    }
    
	/*
	 * After its use during preprocess passage, the EQU
	 * table will not be used anymore and can be deleted
	 */
    DeleteEQU(equTable);
    printf("End of Preprocess.\n\n");
    fclose(fin);
    fclose(fout);

    return error_count;
    
}

/*
 * This function searches the EQU table for token and returns a pointer
 * for the node in which token was found or NULL if it was not found
 */
struct equ_table_node *SearchEQU(struct equ_table_node *table, char *token) {

    struct equ_table_node* tmp = table;
    while ((tmp != NULL)) {
    	if (!strcmp(tmp->label, token)) {
        	return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

/*
 * This function inserts a node in EQU table with label and value
 * defined by name and digit, respectively
 */
void AddEQU(struct equ_table_node **table, char *name, char *digit) {

	int i = 0;
	struct equ_table_node* new = (struct equ_table_node*)malloc(sizeof(struct equ_table_node));
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
 * This function deletes the EQU table, freeing the memory previously
 * allocated
 */
void DeleteEQU(struct equ_table_node *table) {

    struct equ_table_node* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}
