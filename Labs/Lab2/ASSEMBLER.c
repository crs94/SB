#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define LINE_LENGTH 560
#define TOKEN_LENGTH 101

struct fileLines {						// Struct to keep line offsets between passes
    int lineNum;						// Original line number in assembly source
    int lineMod;						// Modified line number after pass
    struct fileLines *next;				// Pointer to next node
};

struct equ_tab {						// Struct to hold symbol-value pairs defined in EQU directives
	char label[TOKEN_LENGTH];			// Symbol to be replaced
	char value[TOKEN_LENGTH];			// Symbol by which to replace label
	struct equ_tab *next;				// Pointer to next node
};

struct MNT {							// Macro Name Table. Keeps names of macros
    char label[TOKEN_LENGTH];			// Name of macro
    struct MDT *begin;					// First line of macro in Macro Definition Table
    struct MNT *next;					// Pointer to next macro
};

struct MDT {							// Macro Definition Table. Holds lines of macros
    char line[LINE_LENGTH];				// Line as defined in source code
    int lineNum;						// Line number where line occurs
    struct MDT *next;					// Pointer to next line
};

struct op_table_node {					// Struct to store a node of the operations table
	char *name;							// Name of the operation
	int opcode;							// Opcode
	int operands;						// Number of operands
};

struct replace_list_node {				// Struct to hold list of references of a symbol to be replaced later
	int *replace;						// Pointer to element that needs to be replaced
	int offset;							// Additional offset (for example ADD N1 + 1)
	struct replace_list_node *next;		// Pointer to next node
};

struct sym_table_node {					// Struct to store a node of the symbol table
	char label[TOKEN_LENGTH];			// Name of the label
	int address;						// Value of Location Counter when label was defined. -1 until defined
	int defined;						// 1 if label definition was found. 0 otherwise
	int vector; 						// Size of vector defined with SPACE directive. Zero until defined
	int sec;							// Section in which symbol is defined
	char type; 							// E for extern, p for public, n for normal
	struct replace_list_node *list; 	// This will keep offset position of undefined symbol
	struct use_list_node *useTable;		// Tabela de usos
	struct sym_table_node *next;		// Pointer to the next node in the symbol table
};

struct use_list_node {					//Struct to store the use table
	int address;
	struct use_list_node *next;
}

struct output_line {					// Struct to hold output to be written to file until pass one finishes
	int opcode;							// Opcode (0 for CONST, 15 for SPACE)
	int op[2];							// Operands
	int line;							// Line number where operation or directive occurs
	struct output_line *next;			// Pointer to next node
};

int GetLine(FILE *fp, char *lineBuffer);

int GetToken(char *lineBuffer, char *tokenBuffer, int p);

int GetToken2(char *lineBuffer, char *tokenBuffer, int *p);

int IsLabel(char *token);

int IsValid(char *token);

int IsNumber(char *token);

int IsHex(char *token);

int HexToInt(char *token);

void addLines(struct fileLines **table, int original, int modified);

struct fileLines *searchLines(struct fileLines *table, int modified);

int insertLines(struct fileLines *table, int original, int modified);

int modifyLines(struct fileLines *table, int line, int modified);

void deleteLines(struct fileLines *table);

struct equ_tab *SearchEQU(struct equ_tab *table, char *token);

void AddEQU(struct equ_tab **table, char *label, char *digit);

void DeleteEQU(struct equ_tab *table);

struct MDT *searchMNT(struct MNT *table, char *token);

struct MDT *addMDT(struct MDT **table, char *toAdd, int line);

void addMNT(struct MNT **table, char *toAdd);

void deleteMDT(struct MDT *table);

void deleteMNT(struct MNT *table);

struct sym_table_node *SearchSym(struct sym_table_node *table, char *token);

void AddSym(struct sym_table_node **table, char *name, int address, int defined, int section);

void DeleteSymTable(struct sym_table_node *table);

struct op_table_node *SearchOp(struct op_table_node *table, char *token);

void AddReplace(struct replace_list_node **node, int *n);

void ReplaceLists(struct sym_table_node *node, int *error);

void AddUse(struct use_list_node **node, int address);

void AddLine(struct output_line *line, struct output_line **head);

void DeleteOutputLines(struct output_line *first);

void WriteObjectFile(FILE *fp, struct output_line *first_text, struct output_line *first_data);

void AdjustAdresses(struct sym_table_node *table, int lc_text);

void FinalErrorCheck(struct sym_table_node *symtab, struct output_line *text, struct output_line *data, int secText, int *error);

int Preprocess(FILE *fin, FILE *fout, struct fileLines **linesTable_Head, int *error_count);

int Pass_Zero(FILE *fin, FILE *fout, struct fileLines **linesTable_Head, int *error_count);

int One_Pass(FILE *fin, FILE *fout, struct fileLines **linesTable_Head, int *error_count, int n_args);

/********************************** MAIN ********************************/

int main(int argc, char *argv[]) {

	// Error message displayed if wrong arguments are passed to the program
	char usage[] = "\n\nUsage: ./assembler module1.asm module2.asm module3.asm\n";

	char *output_file;                      // Name of output file
	char *p_str;                            // Pointer for '.asm' within string
	char pass_pre_out[TOKEN_LENGTH];		// Name of output file from preprocessing step
	char pass_zero_out[TOKEN_LENGTH];		// Name of output file from pass zero
	char pass_one_out[TOKEN_LENGTH];		// Name of output file from pass one
	int errors = 0;							// Error counting
	int n;                                  // Counter of files
	FILE *fp_in = NULL;						// Pointer to input files
	FILE *fp_out = NULL;					// Pointer to output files
	struct fileLines *line_table = NULL;	// Pointer to beginning of lines table


	if((argc < 2) || (argc > 4)) {
		printf("%s\n",usage);
		return 1;
	}

    for (n = 0; n < argc - 1; n++) {
        if ((fp_in = fopen(argv[n + 1], "r")) == NULL) {
            printf("Input file not found.\n");
            return 1;
        }

        strcpy(output_file, argv[n + 1]);
        p_str = strstr(output_file, ".asm");
        strncpy (p_str, "", 1);
        printf("%s\n", output_file);

        strcpy(pass_pre_out, output_file);
        strcat(pass_pre_out, ".pre");

        if ((fp_out = fopen(pass_pre_out, "w")) == NULL) {
            printf("Could not open intermediate file 1.\n");
            return 1;
        }

        if(Preprocess(fp_in, fp_out, &line_table, &errors)) {
            printf("Preprocessing step returned errors\n");
        }

        fclose(fp_in);
        fclose(fp_out);

        if ((fp_in = fopen(pass_pre_out, "r")) == NULL) {
            printf("Could not open intermediate file 2.\n");
            return 1;
        }

        strcpy(pass_zero_out, output_file);
        strcat(pass_zero_out, ".mcr");

        if ((fp_out = fopen(pass_zero_out, "w")) == NULL) {
            printf("Could not open intermediate file 3.\n");
            return 1;
        }

        if(Pass_Zero(fp_in, fp_out, &line_table, &errors)) {
            printf("Pass zero returned errors\n");
        }

        fclose(fp_in);
        fclose(fp_out);

        if ((fp_in = fopen(pass_zero_out, "r")) == NULL) {
            printf("Could not open intermediate file 4.\n");
            return 1;
        }

        strcpy(pass_one_out, output_file);
        strcat(pass_one_out, ".o");

        if ((fp_out = fopen(output_file, "w")) == NULL) {
            printf("Could not open output file.\n");
            return 1;
        }

        if(One_Pass(fp_in, fp_out, &line_table, &errors, argc)) {
            printf("Pass one returned errors\n");
        }

        fclose(fp_in);
        fclose(fp_out);

        return errors;
    }
}

/***************************** PREPROCESS *********************************/

int Preprocess(FILE *fin, FILE *fout, struct fileLines **linesTable_Head, int *error_count) {

	char line[LINE_LENGTH];					//
	char lineOut[LINE_LENGTH];				//
	char token1[TOKEN_LENGTH];				//
	char token2[TOKEN_LENGTH];				//
	int linec = 0; 							// Counts the line
	int linem = 0; 							// Counts the lines that will be on the output file
	int linePos = 0;						//
	int sec = -1;							//
	int endFile = 0;						//
	struct equ_tab *equTable_Head = NULL;	//
	struct equ_tab *tmp = NULL;				//

    while(GetLine(fin, line)) {

    	// Increments line counter
    	linec++;

    	// Adds line in line reference table
    	addLines(linesTable_Head, linec, linec);

	    lineOut[0] = '\0';
	    linePos = 0;

		/*
		 * If line is not only blanks and/or '\n'
		 * the function tries to identify its parts
		 */
	    if (strlen(line) > 0) {

	    	/*
	    	 * Despite the rest of the line, if the last thing
	    	 * is a label, one must concatenate its content
	    	 * with the content found on the next valid line.
	    	 */
	    	if (line[strlen(line)-2] == ':') {
    			line[strlen(line)-1] = '\0';
    			modifyLines(*linesTable_Head, linec, 0);
    			do {
    				// Repeat while line == (blanks and/or '\n')
    				if (!GetLine(fin, lineOut)) {
    					endFile = 1;
    					break;
    				}
    				linec++;
    				addLines(linesTable_Head, linec, 0);
				} while (strlen(lineOut) == 0);
    			if (!endFile) {
	    			strcat(line, " ");
	    			strcat(line, lineOut);
	    		}
	    		else {
	    			printf("Preprocess: ");
	    			printf("Line %d. Sintatic Error: Label refers to no instruction or directive.\n", linec);
	    			(*error_count)++;
	    		}
    		}

    		/*
    		* If line has an EQU directive
    		*/
	    	if (strstr(line, " EQU ") || strstr(line, "EQU ") || strstr(line, " EQU\n")) {
	    		modifyLines(*linesTable_Head, linec, 0);
	    		if((linePos = GetToken(line, token1, linePos))) {
		    		// Checks if first token is a valid label
		    		if(IsLabel(token1)) {
                    	// Checks if there is another token in the line
                        if((linePos = GetToken(line, token2, linePos))) {
                            // Checks if token is an EQU
                            if(!strcmp(token2,"EQU")) {
                            	if(sec > 0) {
                            		printf("Preprocess: ");
                            		printf("Line %d. Warning: EQU inside a section.\n", linec);
                            	}
                                // Checks if EQU has an operand
                                if((linePos = GetToken(line, token2, linePos))) {
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
                                        if((linePos = GetToken(line, token2, linePos))) {
                                        	printf("Preprocess: ");
                                        	printf("Line %d. Sintatic Error: Incorrect number of operands in EQU.\n", linec);
                                        	(*error_count)++;
                                        }
                                    }
                                    else {
                                    	printf("Preprocess: ");
                                        printf("Line %d. Lexical Error: Invalid token %s.\n", linec, token2);
                                        (*error_count)++;
                                    }
                                }
                                else {
                                	printf("Preprocess: ");
                                    printf("Line %d. Sintatic Error: Expected operand after EQU.\n", linec);
                                    (*error_count)++;
                                }
                            }
                            else {
                            	printf("Preprocess: ");
                                printf("Line %d. Sintatic Error: Unexpected token %s after label.\n", linec, token2);
                                (*error_count)++;
                            }
                        }
					}
		    		else {
		    			printf("Preprocess: ");
		    			printf("Line %d. Lexical Error: Invalid label %s.\n", linec, token1);
		    			(*error_count)++;
		    		}
		    	}
		    }

		    else {
		    	/*
    		 	 *	Else, if line has an IF directive
    		  	 */
		    	if(strstr(line, "IF ") || strstr(line, " IF ") || strstr(line, " IF\n")) {
		    		modifyLines(*linesTable_Head, linec, 0);
		    		if((linePos = GetToken(line, token1, linePos))) {
		    			// Checks if first token is an IF
		    			if(!strcmp(token1, "IF")) {
		    				if(sec != 1) {
		    					printf("Preprocess: ");
		    					printf("Line %d. Warning: IF directive outside TEXT section.\n", linec);
		    				}
		    				// Checks whether IF directive has an operand
		    				if((linePos = GetToken(line, token1, linePos))) {
		    					// Searches for the operand in EQU table
		    					tmp = SearchEQU(equTable_Head, token1);
		    					// Operand was found in the table or is a number
		    					if(tmp != NULL) {
	    							// Skips line if operand is a zero
	    							if((atoi(tmp->value) == 0)) {
	    								if (GetLine(fin, line)) {
		    								linec++;
		    								addLines(linesTable_Head, linec, 0);
		    							}
	    								else {
	    									printf("Preprocess: ");
	    									printf("Line %d. Semantic Error: IF in last line of file.\n", linec);
	    									(*error_count)++;
	    								}
	    							}
		    						if((linePos = GetToken(line, token1, linePos))) {
		    							printf("Preprocess: ");
		    							printf("Line %d. Sintatic Error: Incorrect number of operands in IF.\n", linec);
		    							(*error_count)++;
		    						}
		    					}
		    					else {
		    						if(IsNumber(token1)) {
		    							if(atoi(token1) == 0) {
											if (GetLine(fin, line)) {
												linec++;
												addLines(linesTable_Head, linec, 0);
											}
											else {
												printf("Preprocess: ");
												printf("Line %d. Semantic Error: IF in last line of file.\n", linec);
												(*error_count)++;
											}
										}
		    						}
		    						else {
		    							// Operand of IF was not a number or a label in EQU table
		    							printf("Preprocess: ");
		    							printf("Line %d. Sintatic Error: Undefined operand in IF directive.\n", linec);
		    							(*error_count)++;
		    							GetLine(fin, line);
		    						}
		    					}
		    				}
		    				else {
		    					printf("Preprocess: ");
		    					printf("Line %d. Sintatic Error: Expected operand after IF directive.\n", linec);
		    					(*error_count)++;
		    				}
		    			}
		    			else {
		    				printf("Preprocess: ");
		    				printf("Line %d. Sintatic Error: Unexpected label.\n", linec);
		    				(*error_count)++;
		    			}
		    		}
		    	}

		    	/*
    		 	 *	Else, line has neither EQU nor IF directives
    		 	 */
		    	else {
					// Gets tokens one by one and searches them in EQU table
		    		while ((linePos = GetToken(line, token1, linePos))) {
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
	 							else sec = 3; // SECTION with invalid operand
	 						}
	 					}

						// Inserts tokens in output line separated by white space
						strcat(lineOut, token1);
						strcat(lineOut, " ");
				   	}

				   	lineOut[strlen(lineOut) - 1] = '\n';

		    		fprintf(fout, "%s", lineOut);

		    		linem++;
		    		modifyLines(*linesTable_Head, linec, linem);
	    		}
	    	}
	    }
	    /*
	    * Else, if line only has blanks and/or '\n'
	    * the table that stores the references for the
	    * lines is modified so the actual reference
	    * is for a non existent line on the output file
	    */
	    else modifyLines(*linesTable_Head, linec, 0);
    }

    /*
    * After its use during preprocess passage, the EQU
    * table will not be used anymore and can be deleted
    */
    DeleteEQU(equTable_Head);

    return (*error_count);
}

/****************************************** PASS ZERO *************************************/

int Pass_Zero(FILE *fin, FILE *fout, struct fileLines **linesTable_Head, int *error_count) {

    char line[LINE_LENGTH];				//
	char lineOut[LINE_LENGTH];			//
	char token1[TOKEN_LENGTH];			//
	char token2[TOKEN_LENGTH];			//
	int linec = 0; 						// Counts the line
    int linem = 0; 						// Counts the lines that will be on the output file
    int lineo = 0;                      // Stores the number of the line in the original file
	int linePos = 0;					//
	int sec = -1;						//
	int inMacro = 0;					//
	int firstMacro = 0;					//
	struct MDT *mdtTable_Head = NULL;	//
    struct MDT *tmpMDT = NULL;			//
    struct MNT *mntTable_Head = NULL;	//
    struct fileLines *linesTmp = NULL;	//


    while (GetLine(fin, line)) {
        linec++; // Increments line counter
		linesTmp = searchLines(*linesTable_Head, linec);
		if(linesTmp != NULL) lineo = linesTmp->lineNum;
        linePos = 0;
        lineOut[0] = '\0';

        /*
        * If line has MACRO directive
        */
        if (strstr(line, " MACRO ") || strstr(line, "MACRO ") || strstr(line, " MACRO\n")) {
            // Checks whether code has a MACRO within another MACRO
            if (!inMacro) {
                if ((linePos = GetToken(line, token1, linePos))) {
                    if (IsLabel(token1)) {
		                if((linePos = GetToken(line, token2, linePos))) {
		                	if(!strcmp(token2, "MACRO")) {
		                		if(sec != 1) {
									printf("Pass Zero: ");
									printf("Line %d. Warning: MACRO directive outside TEXT section.\n", lineo);
								}
			                	inMacro++;
			                    firstMacro = 1;
			                    addMNT(&mntTable_Head, token1);
			                    modifyLines(*linesTable_Head, linec, 0);
			                }
			                else {
			                	printf("Pass zero: ");
			                	printf("Line %d. Sintatic error: Unexpected token %s\n", lineo, token2);
                    			(*error_count)++;
			                }
		                }
                    }
                    else {
                    	printf("Pass zero: ");
                    	printf("Line %d. Sintatic error: invalid label.\n", lineo);
                    	(*error_count)++;
                    }
                }
            }
            else {
            	printf("Pass Zero: ");
            	printf("Line %d. Error: Nested MACRO.\n", lineo);
            	(*error_count)++;
            }
        }

        /*
        * Else, if line has END directive
        */
        else if ((!strcmp(line, "END\n")) || (!strcmp(line, " END\n"))) {
            // Checks whether END happens within a MACRO
		    if (inMacro) {
		        tmpMDT = addMDT(&mdtTable_Head, line, lineo);
		        inMacro = 0;
		        modifyLines(*linesTable_Head, linec, 0);
		    }
		    else {
		        linesTmp = searchLines(*linesTable_Head, linec);
		        printf("Pass zero: ");
		        printf("Line %d. Semantic error: ENDMACRO without MACRO.\n", lineo);
		        (*error_count)++;
		    }
        }

        /*
        * Else, line has neither MACRO nor END directive
        */
        else {
            // If line is the first line in MACRO definition
            if ((inMacro) && (firstMacro)) {
                tmpMDT = addMDT(&mdtTable_Head, line, lineo);
                mntTable_Head->begin = tmpMDT;
                tmpMDT = NULL;
                firstMacro = 0;
                modifyLines(*linesTable_Head, linec, 0);
            }

            // Else, if line is still in MACRO, but it's not longer the first
            else if ((inMacro) && (!firstMacro)) {
                tmpMDT = addMDT(&mdtTable_Head, line, lineo);
                modifyLines(*linesTable_Head, linec, 0);
            }

            // Else, line is not inside of MACRO definition
            else {
                while((linePos = GetToken(line, token1, linePos))) {
                    tmpMDT = searchMNT(mntTable_Head, token1);

                    // If token1 is found on the MDT, found a MACRO
                    if(tmpMDT != NULL) {
                        while (((strcmp(tmpMDT->line, "END")) && (strcmp(tmpMDT->line, "END ")) && (tmpMDT != NULL))) {
                            fprintf(fout, "%s\n", tmpMDT->line);
                            linem = insertLines(*linesTable_Head, tmpMDT->lineNum, linem);
                            linec = linem;
                            tmpMDT = tmpMDT->next;
                        }
                    }
                    else {
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

                lineOut[strlen(lineOut) - 1] = '\n';
    			fprintf(fout, "%s", lineOut);
                linem++;
                modifyLines(*linesTable_Head, linec, linem);
            }
        }
    }
    // If some MACRO was not finished by the end of the code
    if (inMacro) {
    	printf("Pass zero: ");
    	printf("Semantic Error: MACRO is missing END.\n");
    	(*error_count)++;
    }

    /*
    * After its use during passage zero, MNT and MDT will
    * not be used anymore and can be deleted
    */
    deleteMDT(mdtTable_Head);
    deleteMNT(mntTable_Head);

    return (*error_count);
}

/***************************************** PASS ONE **************************************/

int One_Pass(FILE *fin, FILE *fout, struct fileLines **linesTable_Head, int *error_count, int n_args) {

	char line[LINE_LENGTH];								//
	char token1[TOKEN_LENGTH];							//
	char *dirTable[] = {"SPACE",
                        "CONST",
                        "SECTION",
                        "EXTERN",
                        "PUBLIC",
                        "BEGIN",
                        "END"};
	int linePos = 0;									//
	int i = 0;											//
	int sec = 0;										//
	int flagB = 0;                                      //
	int flagE = 0;                                      //
	int offset = 0;										//
	int lc[3] = {0, 0, 0};								//
	int line_count = 0;									//
	int line_original = 0;                              //
	int label_count = 0;								//
	int opr_count = 0;									//
	int dir = -1;										//
	struct output_line *head_text = NULL;				//
	struct output_line *head_data = NULL;				//
	struct output_line *lineOut = NULL;					//
	struct sym_table_node *symTable = NULL;				//
	struct sym_table_node *tmp_sym = NULL;				//
	struct fileLines *tmp_line = NULL;					//
	struct op_table_node *tmp_op = NULL;				//

	// Operation Table
	struct op_table_node opTable[] = {	"ADD", 1, 1,
										"SUB", 2, 1,
										"MULT", 3, 1,
										"DIV", 4, 1,
										"JMP", 5, 1,
										"JMPN", 6, 1,
										"JMPP", 7, 1,
										"JMPZ", 8, 1,
										"COPY", 9, 2,
										"LOAD", 10, 1,
										"STORE", 11, 1,
										"INPUT", 12, 1,
										"OUTPUT", 13, 1,
										"STOP", 14, 0
										};

    while(GetLine(fin, line)) {

    	line_count++;
    	tmp_line = searchLines(*linesTable_Head, line_count);
    	line_original = tmp_line->lineNum;
    	label_count = 0;
    	linePos = 0;
    	dir = -1;
    	lineOut = (struct output_line*)malloc(sizeof(struct output_line));
    	lineOut->opcode = -1;
    	lineOut->op[0] = -1;
    	lineOut->op[1] = -1; //this will be tested before writing to file
    	lineOut->line = line_original;
    	lineOut->next = NULL;

    	while(GetToken2(line, token1, &linePos)) {
    		// Checks whether token found is a label
			if(token1[strlen(token1) - 1] == ':') {
                // Checks whether label is valid
    			if(IsLabel(token1)) {
    			    // Checks whether line has more than one label
					if(label_count == 0) {
						tmp_sym = SearchSym(symTable, token1);

						// If label is not on symTable
						if(tmp_sym == NULL) {
							AddSym(&symTable, token1, lc[sec], 1, sec); //found label definition
							tmp_sym = symTable;
						}

						// Else, if label is on symTable
						else {
                            // If label has already been defined
							if(tmp_sym->defined) {
								printf("Line %d. Semantic error: Label redefinition\n", line_original);
								(*error_count)++;
							}

                            // Else, if label has already been found, but it is yet to be defined
							else {
								tmp_sym->defined = 1;
								tmp_sym->address = lc[sec];
								tmp_sym->sec = sec;
							}
						}
						label_count++;
					}
					else {
						printf("Line %d. Sintatic error: More than one label in the same line\n", line_original);
						(*error_count)++;
					}
				}
				else {
					printf("Line %d. Lexical error: Invalid label %s\n", line_original, token1);
					(*error_count)++;
				}
    		}

    		// Else, if token found is not a label
    		else {
                // Checks whether token is valid
				if(IsValid(token1)) {
					tmp_op = SearchOp(opTable, token1);
                    // Checks whether token is an instruction
					if(tmp_op != NULL) {
                        // If instruction is in DATA
						if(sec == 2) {
							printf("Line %d. Semantic error: Operation inside DATA section\n", line_original);
							(*error_count)++;
						}
                        // Else, if instruction is outside of TEXT and DATA
						else {
							if(!sec) {
								printf("Line %d. Semantic error: Operation outside TEXT section\n", line_original);
								(*error_count)++;
							}
						}

						lc[1]+=(tmp_op->operands+1);
						lineOut->opcode = tmp_op->opcode;
						opr_count = 0;

						// Gets operands and any other tokens in the line
						while(GetToken2(line, token1, &linePos)) {
							// Checks whether token found is valid or if it is the first argument of COPY
							if(IsValid(token1) ||
							((opr_count == 0) && (!strcmp(tmp_op->name, "COPY")) && (token1[strlen(token1)-1] == ',') )) {
							    // Removes the comma after the argument
								if(token1[strlen(token1)-1] == ',') {
									token1[strlen(token1)-1] = '\0';
								}

								// Checks whether the number of arguments is right
								if(opr_count < tmp_op->operands) {
									tmp_sym = SearchSym(symTable, token1);

									// If token is in symTable
									if(tmp_sym != NULL) {
                                        if((tmp_sym->defined) && (tmp_sym->sec == 1)) {
											lineOut->op[opr_count] = tmp_sym->address;
										}
                                        else {
											lineOut->op[opr_count] = -1;
											AddReplace(&tmp_sym->list, &lineOut->op[opr_count]);
										}
									}

									// Else, if token is yet to be defined
									else {
										AddSym(&symTable, token1, -1, 0, 0);
										lineOut->op[opr_count] = -1;
										AddReplace(&symTable->list, &lineOut->op[opr_count]);
										tmp_sym = symTable;
									}

									// If token in symTable is extern
									if (tmp_sym->type == 'e') {
										// Add address to use table
										AddUse(&tmp_sym->useTable, lc[sec]);

										// If use table have not been initialized before
										if (tmp_sym->useTable == NULL) {
											tmp_sym->useTable = tmp_use;
										}
									}
								}
								else {
									printf("Line %d. Sintatic error: invalid number of operands in %s\n", line_original, tmp_op->name);
									(*error_count)++;
								}
								opr_count++;
							}
							else {
							    // Checks whether and offset will be added to the argument
								if((!strcmp(token1, "+")) && (opr_count > 0) && (opr_count <= tmp_op->operands)) {
                                    // Checks whether previous token is an argument
									if(tmp_sym != NULL) {
									    // Checks whether + is followed by an offset
										if(GetToken2(line, token1, &linePos)) {
											// Checks whether offset is a number
											if(IsNumber(token1)) {
												offset = atoi(token1);
												// If token has already been defined
												if((tmp_sym->defined) && (tmp_sym->sec == 1)) {
                                                    // If defined offset is within the range of memory allocated for the label
													if((offset < (tmp_sym->vector))) {
														lineOut->op[opr_count-1] += offset;
													}
                                                    // Else, if offset goes beyond the delimited space reserved for its label
													else {
														printf("Line %d. Semantic error: offset %s too large\n", line_original, token1);
														(*error_count)++;
													}
												}
                                                // Else, if previous tokes is yet to be defined
												else {
													// Add offset to correspondent reference list node
													tmp_sym->list->offset = offset;
												}
											}
											else {
												printf("Line %d. Semantic error: invalid offset %s\n", line_original, token1);
												(*error_count)++;
											}
										}
										else {
											printf("Line %d. Sintatic error: Expected offset after \"+\"\n", line_original);
											(*error_count)++;
										}
									}
									else {
										printf("Line %d. Sintatic error: \"+\" without previous operand\n", line_original);
										(*error_count)++;
									}
									tmp_sym = NULL; //so if next token is invalid doesn't enter here.
								}
								else {
									printf("Line %d. Sintatic error: Invalid operand %s.\n", line_original, token1);
									(*error_count)++;
								}
							}
						}
						if(opr_count < tmp_op->operands) {
							printf("Line %d. Sintatic error: missing operands in %s\n", line_original, tmp_op->name);
							(*error_count)++;
						}
					}

					// If token is not an instruction
					else {
                        // Searches directives table
						for(i = 0; i < 7; i++) {
							if(!strcmp(dirTable[i], token1)) {
								dir = i;
							}
						}
                        // If token is a directive
						if(dir > -1) {
							// Checks whether directive is not SECTION, PUBLIC, EXTERN, BEGIN or END
							if((dir < 2)) {
                                // If directive is within TEXT
								if(sec == 1) {
									printf("Line %d. Semantic error: Directive inside TEXT section\n", line_original);
									(*error_count)++;
								}
                                // Else, if directive is outside of TEXT and DATA
								else {
									if(!sec) {
										printf("Line %d. Semantic error: Directive outside of DATA section\n", line_original);
										(*error_count)++;
									}
								}
							}
							// Checks whether directive is SPACE
							if(dir == 0) {
								// Checks whether SPACE follows a label
								if(label_count == 1) {
									i = 0;
									// Gets next token in line
									while(GetToken2(line, token1, &linePos)) {
										if(i == 0) {
                                            // If token is a number
											if(IsNumber(token1)) {
                                                // There is a vector associated to the label
												offset = atoi(token1);
												lc[2] += offset;
												tmp_sym->vector = offset;
												lineOut->opcode = 15;
												lineOut->op[0] = offset;
											}
                                            // Else, if token is not a number
											else {
												printf("Line %d. Sintatic error: Expected a number after SPACE\n", line_original);
												(*error_count)++;
											}
										}
										i++;
									}
									// If there is more than one argument in SPACE
									if(i > 1) {
										printf("Line %d. Sintatic error: Too many operands in SPACE\n", line_original);
										(*error_count)++;
									}
									// Else, if SPACE had no operands
									else {
										if(i == 0) {
											lc[2]++;
											lineOut->opcode = 15;
											lineOut->op[0] = 1;
										}
									}
								}
								else {
									printf("Line %d. Sintatic error: Expected one label before directive SPACE\n", line_original);
									(*error_count)++;
								}
							}

							// Else, if directive is CONST
							else if(dir == 1) {
								// Checks whether CONST follows a label
								if(label_count == 1) {
									i = 0;
									// Gets next token in line
									while(GetToken2(line, token1, &linePos)) {
										i++;
									}
									if(i == 0) {
                                        // If token is an hexadecimal number
										if(IsHex(token1)) {
											offset = HexToInt(token1);
											lc[2]++;
											lineOut->opcode = 0;
											lineOut->op[0] = offset;
										}
                                        // Else, if token is a number
										else {
											if(IsNumber(token1)) {
												offset = atoi(token1);
												lc[2]++;
												lineOut->opcode = 0;
												lineOut->op[0] = offset;
											}
											// Else, if token is neither a decimal nor an hexadecimal
											else {
												printf("Line %d. Sintatic error: Expected a number after CONST\n", line_original);
												(*error_count)++;
											}
										}
									}
									// If CONST has no arguments
									if(i == 0) {
										printf("Line %d. Sintatic error: Expected a number after CONST\n", line_original);
										(*error_count)++;
									}
									// Else, if CONST has more than one argument
									else {
										if(i > 1) {
											printf("Line %d. Sintatic error: Too many operands in CONST\n", line_original);
											(*error_count)++;
										}
									}
								}
								else {
									printf("Line %d. Sintatic error: Expected one label before directive CONST\n", line_original);
									(*error_count)++;
								}
							}

							// Else, if directive is SECTION
							else if(dir == 2) {
                                // Checks whether the line has a label
								if(label_count > 0) {
									printf("Line %d. Sintatic error: Label before SECTION\n", line_original);
									(*error_count)++;
								}
								else {
									i = 0;
                                    // Gets next token
									while(GetToken2(line, token1, &linePos)) {
										if(i == 0) {
                                            // Checks whether token is valid
											if(IsValid(token1)) {
											    // If token is TEXT
												if(!strcmp(token1, "TEXT")) {
												    // If not yet inside of TEXT
													if(sec != 1) {
														sec = 1;
													}
                                                    else {
														printf("Line %d. Semantic error: More than one TEXT section\n", line_original);
														(*error_count)++;
													}
												}
												// Else, if token is DATA
												else {
													if(!strcmp(token1, "DATA")) {
														// If not yet inside of DATA
														if(sec != 2) {
															sec = 2;
														}
														else {
															printf("Line %d. Semantic error: More than one DATA section\n", line_original);
															(*error_count)++;
														}
													}
													// Else, if neither TEXT nor DATA
													else {
														printf("Line %d. Sintatic error: Invalid operand in directive SECTION\n",line_original);
														(*error_count)++;
													}
												}
											}
											else {
												printf("Line %d. Lexical error: Invalido token\n", line_original);
												(*error_count)++;
											}
										}

										i++;
									}
									// If no arguments follow SECTION
									if(i == 0) {
										printf("Line %d. Sintatic error: Expected one operand after SECTION\n", line_original);
										(*error_count)++;
									}
									// Else, if SECTION has more than one argument
									else {
										if(i > 1) {
											printf("Line %d. Sintatic error: Expected one operand after SECTION\n", line_original);
											(*error_count)++;
										}
									}
								}
							}

                            // Else, if directive is EXTERN
                            else if(dir == 3) {
                                // Checks whether CONST follows a label
                                if(label_count == 1) {
                                    i = 0;
                                    // Gets next token in line
                                    while(GetToken2(line, token1, &linePos)) {
                                        i++;
                                    }
                                    // If EXTERN has no arguments
                                    if(i == 0) {
                                        // Search for label in symtable
                                        linePos = 0;
										GetToken2(line, token1, &linePos)
										tmp_sym = SearchSym(symTable, token1);
										// Modify label type to extern
										tmp_sym->type = 'e';
                                    }
                                    // Else, if EXTERN has arguments
                                    else {
                                        if(i > 0) {
                                            printf("Line %d. Sintatic error: Too many operands in EXTERN\n", line_original);
                                            (*error_count)++;
                                        }
                                    }
                                }
                                else {
                                    printf("Line %d. Sintatic error: Expected one label before directive EXTERN\n", line_original);
                                    (*error_count)++;
                                }
                            }

                            // Else, if directive is PUBLIC
                            else if(dir == 4) {
                                // Checks whether the line has a label
                                if(label_count > 0) {
                                    printf("Line %d. Sintatic error: Label before PUBLIC\n", line_original);
                                    (*error_count)++;
                                }
                                else {
                                    i = 0;
                                    // Gets next token
                                    while(GetToken2(line, token1, &linePos)) {
                                        i++;
                                    }
                                    // if PUBLIC has one argument
                                    if(i == 1) {
										// Checks whether token is valid
										if(IsValid(token1)) {
											// Search for token in symtable
											tmp_sym = SearchSym(symTable, token1);

                                            // If token is not in table
                                            if (tmp_sym == NULL) {
                                            	// Add label
												AddSym(&symTable, token1, 0, 0, sec);
												tmp_sym = symTable;
                                            }
                                            // Modify label type to public
											tmp_sym->type = 'p';                                  			
									    }
									    else {
									        printf("Line %d. Lexical error: Invalid token\n", line_original);
									        (*error_count)++;
									    }
									}
                                    // If no arguments follow PUBLIC
                                    if(i == 0) {
                                        printf("Line %d. Sintatic error: Expected one operand after PUBLIC\n", line_original);
                                        (*error_count)++;
                                    }
                                    // Else, if PUBLIC has more than one argument
                                    else {
                                        if(i > 1) {
                                            printf("Line %d. Sintatic error: Expected one operand after PUBLIC\n", line_original);
                                            (*error_count)++;
                                        }
                                    }
                                }
                            }

                            // Else, if directive is BEGIN
                            else if(dir == 5) {
                                // Checks whether BEGIN follows a label
                                if(label_count == 1) {
                                    i = 0;
                                    // Gets next token in line
                                    while(GetToken2(line, token1, &linePos)) {
                                        i++;
                                    }
                                    // If BEGIN has no arguments
                                    if(i == 0) {
                                        // Checks whether there are multiple BEGINs
                                        if (++flagB > 1) {
                                            printf("Line %d. Semantic error: Multiple BEGIN directives in file\n", line_original);
                                            (*error_count)++;
                                        }
                                        if (n_args < 3) {
                                            printf("Line %d. Semantic error: BEGIN used with single module\n", line_original);
                                            (*error_count)++;
                                        }
                                    }
                                    // Else, if BEGIN has arguments
                                    else {
                                        if(i > 0) {
                                            printf("Line %d. Sintatic error: Too many operands in BEGIN\n", line_original);
                                            (*error_count)++;
                                        }
                                    }
                                }
                                else {
                                    printf("Line %d. Sintatic error: Expected one label before directive BEGIN\n", line_original);
                                    (*error_count)++;
                                }
                            }

                            // Else, if directive is END
                            else if(dir == 6) {
                                // Checks whether the line has a label
                                if(label_count > 0) {
                                    printf("Line %d. Sintatic error: Label before END\n", line_original);
                                    (*error_count)++;
                                }
                                else {
                                    i = 0;
                                    // Gets next token
                                    while(GetToken2(line, token1, &linePos)) {
                                        i++;
                                    }
                                    // If no arguments follow END
                                    if(i == 0) {
                                        // Checks whether there are multiple ENDs
                                        if (++flagE > 1) {
                                            printf("Line %d. Semantic error: Multiple END directives in file\n", line_original);
                                            (*error_count)++;
                                        }
                                        if (n_args < 3) {
                                            printf("Line %d. Semantic error: END used with single module\n", line_original);
                                            (*error_count)++;
                                        }
                                    }
                                    // Else, if END has arguments
                                    else {
                                        if(i > 1) {
                                            printf("Line %d. Sintatic error: Expected no operand after END\n", line_original);
                                            (*error_count)++;
                                        }
                                    }
                                }
                            }

                            // Else, if neither instruction nor directive follow label
                            else {
                                printf("Line %d. Sintatic error: Token %s. Expected instruction or directive\n", line_original, token1);
                                (*error_count)++;
                            }
						}
					}
				}
				else {
					printf("Line %d. Lexical error: Invalid token %s\n", line_original, token1);
					(*error_count)++;
				}
    		}
    	}

    	if ((flagB != 1) || (flagE != 1)) {
            if ((!flagB) && (!flagE) && (n_args > 2)) {
                printf("Semantic error: File is missing BEGIN and END\n");
                (*error_count)++;
            }
            else if (flagB > flagE) {
                printf("Semantic error: BEGIN directive is missing END\n");
                (*error_count)++;
            }
            else if (flagB < flagE) {
                printf("Semantic error: END directive is missing BEGIN\n");
                (*error_count)++;
            }
            else {
                printf("Semantic error: Multiple BEGIN and END directives in one file\n");
                (*error_count)++;
            }
    	}

    	if(lineOut->opcode != -1) {
    		if(sec == 2) {
    			AddLine(lineOut, &head_data);
			}
			else {
				if(sec == 1 ) {
					AddLine(lineOut, &head_text);
				}
			}
		}
		else {
			free(lineOut);
		}
    }
    printf("lc1: %d; lc2: %d\n", lc[1], lc[2]);
    // Acusar erro se lc[1] = 0? (no section TEXT)
    //checar se há labels indefinidas na symTable antes?
    AdjustAdresses(symTable, lc[1]);
    ReplaceLists(symTable, error_count);
    i = 0;
    FinalErrorCheck(symTable, head_text, head_data, lc[1], &i);
    (*error_count) += i;
    printf("Writing to object file\n");
    WriteObjectFile(fout, head_text, head_data);
    printf("Deleting output lines\n");
    DeleteOutputLines(head_text);
    DeleteOutputLines(head_data);
    printf("Deleting symbol table\n");
    DeleteSymTable(symTable);
    printf("FINISHED!\n");

    return (*error_count);
}

/*************************** PREPROCESSING FUNCTIONS ***************************/

/*
 * This function searches the EQU table for token and returns a pointer
 * for the node in which token was found or NULL if it was not found
 */
struct equ_tab *SearchEQU(struct equ_tab *table, char *token) {

    struct equ_tab* tmp = table;
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
void AddEQU(struct equ_tab **table, char *name, char *digit) {

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
 * This function deletes the EQU table, freeing the memory previously
 * allocated
 */
void DeleteEQU(struct equ_tab *table) {

    struct equ_tab* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}

/****************************** MACRO FUNCTIONS ******************************/

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

/************************************ PASS ONE FUNCTION ***************************************/

//Found an undefined operand and need to include it in replace_list. Includes at the front of list
void AddReplace(struct replace_list_node **node, int *n) {
	struct replace_list_node *new = (struct replace_list_node*)malloc(sizeof(struct replace_list_node));
	new->replace = n;
	new->offset = 0;
	new->next = *node;
	*node = new;
}

/*
 * Replaces replace list of each node in SymTable and deletes the replace list
 */
void ReplaceLists(struct sym_table_node *node, int *error) {

	struct replace_list_node *tmp = NULL;
	struct sym_table_node *temp = node;

	while(temp != NULL) {
		tmp = temp->list;
		printf("Replacing references for %s\n", temp->label);
		while (tmp != NULL) {
			printf("InWhile\n");
			printf("Replacing %d with %d\n",*tmp->replace, (temp->address + tmp->offset));
			if(tmp->offset <= temp->vector) {
				(*tmp->replace) = (temp->address + tmp->offset);
				printf("Replaced\n");
			}
			else {
				(*tmp->replace) = temp->address;
				printf("Error: offset too large in operand %s\n", temp->label);
				(*error)++;
			}
		    tmp = tmp->next;
		}

		printf("Deleting list of replaces in node %s\n", temp->label);
		while(temp->list != NULL) {
			tmp = temp->list;
			temp->list = temp->list->next;
			printf("Deleting\n");
			free(tmp);
		}
		temp = temp->next;
	}
}

// Found an extern operand and need to include it in use_list. Includes at the front of list
void AddUse(struct use_list_node **node, int address) {
	struct use_list_node *new = (struct use_list_node*)malloc(sizeof(struct use_list_node));
	new->address = address;
	new->next = *node;
	*node = new;
}

/*
 * Function to search for a label in the symbol table.
 * Returns a pointer to the node where the label was found or
 * NULL if label is not in the table
 */
struct sym_table_node *SearchSym(struct sym_table_node *table, char *token) {

	struct sym_table_node *tmp = table;

    while ((tmp != NULL)) {
    	printf("Searching for %s. I'm here: %s\n",token,tmp->label);
        if (!strcmp(tmp->label, token)) {
        	return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

// Function to add a node in the symbol table.
void AddSym(struct sym_table_node **table, char *name, int address, int defined, int section) {

	struct sym_table_node* new = (struct sym_table_node*)malloc(sizeof(struct sym_table_node));
	strcpy(new->label, name);
	new->address = address;
	new->defined = defined;
	new->list = NULL;
	new->vector = 0;
	new->sec = section;
	new->next = *table;
	*table = new;
}

// Deletes Symbol Table
void DeleteSymTable(struct sym_table_node *table) {
	struct sym_table_node* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}

/*
 * This function checks if token is an operation in op_table.
 * It return a pointer to the node where the operation was found or
 * NULL otherwise
 */
struct op_table_node *SearchOp(struct op_table_node table[], char *token) {
	int n;
	struct op_table_node* tmp = NULL;
    for(n = 0; n < 14; n++) {
    	if(!strcmp(table[n].name, token)) tmp = &table[n];
    }
    return tmp;
}

/*
 * This function adds a line to the data structure holding the output
 * lines prior to writing them to the output file
 */
void AddLine(struct output_line *line, struct output_line **first) {
	struct output_line *tmp = *first;
	if (tmp == NULL) {
        *first = line;
    }
    else {
		while (tmp->next != NULL) {
    	    tmp = tmp->next;
		}
    	tmp->next = line;
    }
}

// Deletes list holding output lines
void DeleteOutputLines(struct output_line *first) {
	struct output_line* tmp;
    while(first != NULL) {
    	tmp = first;
    	first = first->next;
    	free(tmp);
    }
}

void AdjustAdresses(struct sym_table_node *table, int lc_text) {
	printf("Adjusting addresses\n");
	struct sym_table_node *tmp = table;
	while(tmp != NULL) {
		if(tmp->sec == 2) {
			printf("Adding %d to %d in node %s\n", (lc_text + 1), tmp->address, tmp->label);
			tmp->address += (lc_text + 1);
		}
		tmp = tmp->next;
	}
}

void FinalErrorCheck(struct sym_table_node *symtab, struct output_line *text, struct output_line *data, int secText, int *error) {

	printf("Checking errors\n");
	struct sym_table_node *tmp = symtab;
	struct output_line *temp = text;
	struct output_line *temp2 = data;
	int i = (secText + 1);				// First address of data section

	while(tmp != NULL) {
		if(tmp->defined == 0) {
			printf("Semantic error: Undefined label %s\n", tmp->label);
			(*error)++;
		}
		tmp = tmp->next;
	}

	while(temp != NULL) {

		printf("Checking condition 1\n");
		if((temp->opcode > 4) && (temp->opcode < 9)) {
			if(temp->op[0] > i) {
				printf("Line %d. Semantic error: Jump to invalid section\n", temp->line);
				(*error)++;
			}
		}
		else {

			printf("Checking condition 2\n");
			if(((temp->opcode < 14) && (temp->opcode > 0) && (temp->op[0] < i)) ||
				((temp->op[1] > -1) && (temp->op[1] < i))) {

				printf("Linha %d. Erro: Instrução referencia seção inválida\n", temp->line);
				(*error)++;
			}

			printf("Checking condition 6\n");
			if(temp->opcode == 9) {
				while((i < temp->op[1]) && (temp2 != NULL)) {
					if(temp2->opcode == 15) {
						i += temp2->op[0];
					}
					else {
						i++;
					}
					temp2 = temp2->next;
				}
				if((temp2->opcode == 0) && (temp2->op[0] == 0)) {
					printf("Linha %d. Erro: Modificação de valor constante\n", temp->line);
					(*error)++;
				}
				i = (secText + 1);
				temp2 = data;
			}
			else {
				printf("Checking condition 5\n");
				if((temp->opcode ==11) || (temp->opcode == 12)) {
					while((i < temp->op[0]) && (temp2 != NULL)) {
						if(temp2->opcode == 15) {
							i += temp2->op[0];
						}
						else {
							i++;
						}
						temp2 = temp2->next;
					}
					if((temp2->opcode == 0) && (temp2->op[0] == 0)) {
						printf("Linha %d. Erro: Modificação de valor constante\n", temp->line);
						(*error)++;
					}
					i = (secText + 1);
					temp2 = data;
				}
				else {
					printf("Checking condition 4\n");
					if(temp->opcode == 4) {
						while((i < temp->op[0]) && (temp2 != NULL)) {
							if(temp2->opcode == 15) {
								i += temp2->op[0];
							}
							else {
								i++;
							}
							temp2 = temp2->next;
						}
						if((temp2->opcode == 0) && (temp2->op[0] == 0)) {
							printf("Line %d. Semantic error: division by zero\n", temp->line);
							(*error)++;
						}
						i = (secText + 1);
						temp2 = data;
					}
				}
			}
		}

		temp = temp->next;
	}
}

void WriteObjectFile(FILE *fp, struct output_line *first_text, struct output_line *first_data) {

	int n;
	struct output_line *tmp = first_text;

	while(tmp != NULL) {
    	if(tmp->opcode > -1) {
    		if(tmp->opcode == 14) {
    			fprintf(fp, "%d\n", tmp->opcode);
			}
			else {
				fprintf(fp, "%d %d ", tmp->opcode, tmp->op[0]);
				if(tmp->op[1] > -1) {
					fprintf(fp, "%d ", tmp->op[1]);
				}
				fprintf(fp, "\n");
			}
    	}
    	tmp = tmp->next;
    }

    tmp = first_data;

    while(tmp != NULL) {
    	if(tmp->opcode == 0) { //CONST
    			fprintf(fp, "%d\n", tmp->op[0]);
		}
		else {
			if(tmp->opcode == 15) { //SPACE
				for(n = 0; n < tmp->op[0]; n++) {
					fprintf(fp, "0 ");
				}
				fprintf(fp, "\n");
			}
		}
    	tmp = tmp->next;
    }
}

/************************************* MISCELANEOUS FUNCTIONS *************************************/

/*
 * This function reads one line from the file pointed by fp
 * and returns a string with all comments and unnecessary white
 * spaces removed and all letters capitalized
 */
int GetLine(FILE *fp, char *lineBuffer) {

	char c;		// Keeps character read from file
	int n = 0;	// Index of current position in lineBuffer

	c = fgetc(fp);
	while ((c != '\n') && (n < 600) && (c != EOF) && (c != ';')) {
		if (n == 0) {
			// Ignores initial blanks
			while ((c == ' ') || (c == '\t')) {
				c = fgetc(fp);
			}
			// Breaks if line is empty or only has comments
			if ((c == ';') || (c == '\n')) break;
		}

		// Changes tabs to spaces to keep lines uniform
		if (c == '\t') {
			c = ' ';
		}

		// Ignores multiple spaces between tokens
		if ((lineBuffer[n - 1] == ' ') && (n != 0)) {
			while ((c == ' ') || (c == '\t')) {
				c = fgetc(fp);
			}
			// Breaks if newline was found after multiple spaces
			if(c == '\n') {
				n--;
				break;
			}
		}

		if (islower(c)) {
			// Converts char to upper case
			c = toupper(c);
		}

		lineBuffer[n] = c;
		n++;
		c = fgetc(fp);
	}

	if (c == ';') {
		// If a comment is identified, the rest of the line is ignored
		while ((c = fgetc(fp)) != '\n');
	}

	// If line wasn't empty
	if (n > 0) {
		if(lineBuffer[n-1] == ' ') n--;
		lineBuffer[n] = '\n';
		lineBuffer[++n] = '\0';
	}

	else {
		lineBuffer[0] = '\0';
	}

	if (c != EOF) return 1;
	return 0;
}

/*
 * This function gets one token from the line pointed by lineBuffer
 * and returns it in tokenBuffer. p keeps the position of next token
 * in lineBuffer for the next call to GetToken
 */
int GetToken(char *lineBuffer, char *tokenBuffer, int p) {

	int n = 0;
	if (lineBuffer[p] == '\n') return 0; // No token left in line

	while ((!isspace(lineBuffer[p])) && (n < 100)) {
		tokenBuffer[n] = lineBuffer[p];
		n++;
		p++;
	}
	tokenBuffer[n] = '\0';

	if (lineBuffer[p] == '\n') return p;
	return ++p;
}

/*
 * Alternative version of GetToken
 */
int GetToken2(char *lineBuffer, char *tokenBuffer, int *p) {
	printf("In GetToken2\n");
	int n = 0;
	if (lineBuffer[(*p)] == '\n') return 0;

	while ((!isspace(lineBuffer[(*p)])) && (n < 100)) {
		tokenBuffer[n] = lineBuffer[(*p)];
		n++;
		(*p)++;
	}
	tokenBuffer[n] = '\0';
	printf("Token: %s\n", tokenBuffer);
	if (lineBuffer[(*p)] != '\n') (*p)++;
	return 1;
}

/*
 * This function tests if token is a label
 */
int IsLabel(char *token) {

	int i = 0;

	// If first character is a digit return zero
	if(isdigit(token[i])) {
		return 0;
	}

	while (token[++i] != '\0') {

		 /* A token is valid if it contains only
		  * alphanumeric characters or underscores */
		if((!isalnum(token[i])) && (token[i] != '_')) {
			/* Return 0 if colon is found and it is not
			 * at the last position in token */
			if((token[i] == ':') && (token[i+1] != '\0')) {
				return 0;
			}
		}
	}

	if(!strcmp(token, "EQU:")) {
		return 0;
	}
	else if(!strcmp(token, "IF:")) {
		return 0;
	}
	else if(!strcmp(token, "MACRO:")) {
		return 0;
	}
	else if(!strcmp(token, "END:")) {
		return 0;
	}
	else if(!strcmp(token, "SECTION:")) {
		return 0;
	}
	else if(!strcmp(token, "SPACE:")) {
		return 0;
	}
	else if(!strcmp(token, "CONST:")) {
		return 0;
	}
	else if(!strcmp(token, "ADD:")) {
		return 0;
	}
	else if(!strcmp(token, "SUB:")) {
		return 0;
	}
	else if(!strcmp(token, "MULT:")) {
		return 0;
	}
	else if(!strcmp(token, "DIV:")) {
		return 0;
	}
	else if(!strcmp(token, "JMP:")) {
		return 0;
	}
	else if(!strcmp(token, "JMPN:")) {
		return 0;
	}
	else if(!strcmp(token, "JMPP:")) {
		return 0;
	}
	else if(!strcmp(token, "JMPZ:")) {
		return 0;
	}
	else if(!strcmp(token, "COPY:")) {
		return 0;
	}
	else if(!strcmp(token, "LOAD:")) {
		return 0;
	}
	else if(!strcmp(token, "STORE:")) {
		return 0;
	}
	else if(!strcmp(token, "INPUT:")) {
		return 0;
	}
	else if(!strcmp(token, "OUTPUT:")) {
		return 0;
	}
	else if(!strcmp(token, "STOP:")) {
		return 0;
	}

	if (token[i - 1] == ':') {
		token[i - 1] = '\0';
		return 1;
	}
	else {
		return 0;
	}
}

/*
 * This function tests if token is valid
 * It is assumed it is not a label
 */
int IsValid(char *token) {

	int i = 0;

	while (token[i] != '\0') {
		if((!isalnum(token[i])) && (token[i] != '_')) {
			return 0;
		}
		i++;
	}
	return 1;
}

/*
 * This function tests if token is a decimal number
 */
int IsNumber(char *token) {

	int i = 0;
	int negative = 0;

	if(token[i] == '-') {
		negative = 1;
		i++;
	}

	while( i < strlen(token)) {
		if (!isdigit(token[i])) {
			return 0;
		}
		i++;
	}

	if(negative) {
		return -1;
	}

	return 1;
}

/*
 * This function tests if token is a hexadecimal number
 */
int IsHex(char *token) {

	int i;

	if ((token[0] == '0') && (token[1] == 'X')) {
		for(i = 2; i<strlen(token); i++) {
			if((token[i] < 48) || ((token[i] > 57) && (token[i] < 65)) || (token[i] > 70)) return 0;
		}
		return 1;
	}
	else if ((token[0] == '-') && (token[1] == '0') && (token[2] == 'X')) {
		for(i = 3; i<strlen(token); i++) {
			if((token[i] < 48) || ((token[i] > 57) && (token[i] < 65)) || (token[i] > 70)) return 0;
		}
		return 1;
	}

	return 0;
}

/*
 * This function converts a token that was tested with
 * IsHex to a decimal number
 */
int HexToInt(char *token) {

	int i = strlen(token) - 1;
	int j = 0;
	int digit = 0;
	int sum = 0;
	char aux = token[i];

	while ((i > 0) && (aux != 'X')) {
		if ((aux > 47) && (aux < 58)) digit = aux - 48;
		else if ((aux > 64) && (aux < 71)) digit = aux - 55;
		sum+=(digit*(pow(16, j)));
		i--;
		j++;
		aux = token[i];
	}

	if (token[0] == '-') sum *= -1;

	return sum;
}


/*
 * Add lines at the end of the table
 * 	It is essentially used during Preprocess to create the table
 */
void addLines(struct fileLines **table, int original, int modified) {

	struct fileLines* tmp = *table;
    struct fileLines* before = NULL;
	struct fileLines* new = (struct fileLines*)malloc(sizeof(struct fileLines));

    new->lineNum = original;
    new->lineMod = modified;
    new->next = NULL;

    // If the table is empty, insert first node
	if (*table == NULL) {
        *table = new;
        return;
    }

    // Else, search for empty space
    while (tmp != NULL) {
        before = tmp;
        tmp = tmp->next;
	}
    before->next = new;
}

/*
 * Searches for lines in the table
 * 	It is used to find a given line addressed by the variable modified
 */
struct fileLines *searchLines(struct fileLines *table, int modified) {

    struct fileLines* tmp = table;

    /*
    * Searches whole table until correct modified line is found
    * or until the end is reached
    */
    while ((tmp != NULL)) {
    	if (tmp->lineMod == modified) {
        	// If found, returns a pointer to the line in the table
            return tmp;
        }
        tmp = tmp->next;
    }
    // Else, returns a NULL pointer
    return NULL;
}

/*
 * Inserts lines in the middle of the table
 * 	It is used essentially during Pass_Zero to add the references of
 * 	the lines of a macro definition in the middle of the code
 */
int insertLines(struct fileLines *table, int original, int modified) {

	int n = 0, update = 0;
    struct fileLines* new = (struct fileLines*)malloc(sizeof(struct fileLines));
    struct fileLines* tmp = NULL;
    struct fileLines* b4 = NULL;
    tmp = searchLines(table, modified);

    /*
    * The lines found by searchLines is the line prior the
    * macro calling. Therefore, a new line will be inserted
    * in between the previous line and the macro calling.
    */
    if (tmp != NULL) {
        new->lineNum = original;
        new->lineMod = modified + 1;
        new->next = tmp->next;
        tmp->next = new;

        update = modified + 1;
        b4 = new;
		tmp = new->next;

		/*
		* The rest of the table needs to be updated so the
		* lineMod variable will match the new order imposed
		* by the insertion of the new line.
		*/
        while (tmp != NULL) {
        	/*
        	* If it is the first line to be modified,
        	* it means that it is the macro calling and
        	* the line will not be on the final output
        	* file. Therefore, its lineMod value must be 0.
        	*/
        	if (n == 0) {
        		tmp->lineMod = 0;
        		b4 = tmp;
        		tmp = tmp->next;
        	}
        	else {
                /*
                * Guarantees that only the lines that were
                * not equal 0 are being modified.
                */
                if (tmp->lineMod) {
                    update++;
                    tmp->lineMod = update;
                    b4 = tmp;
                    tmp = tmp->next;
                }
	        }
	        n++;
        }
        /*
        * If insertion runs smoothly, returns the
        * value of the new line that has just been
        * inserted into the table
        */
        return modified + 1;
    }
    // Else, returns 0
    return 0;
}

/*
 * Modifies lines in the table
 * 	It is used to modify the values of the lines as the lines
 * 	of the original file are modified throughout the program
 */
int modifyLines(struct fileLines *table, int line, int modified) {

    struct fileLines* tmp = NULL;
    tmp = searchLines(table, line);

    if (tmp != NULL) {
        tmp->lineMod = modified;
        return modified;
    }
    return -1;
}

// Deletes the table
void deleteLines(struct fileLines *table) {

    struct fileLines* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}
