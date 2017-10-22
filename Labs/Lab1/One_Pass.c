/***********************************************************************
*
* FILE NAME: One_Pass.c
*
*
* PURPOSE:
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
//#include "One_Pass.h"

struct op_table_node {					// Struct to store a node of the operations table
	char *name;							// Name of the operation
	int opcode;							// Opcode
	int operands;						// Number of operands
};

struct replace_list_node {				//
	int *replace;						//
	int offset;							//
	struct replace_list_node *next;		//
};

struct sym_table_node {					// Struct to store a node of the symbol table
	char label[TOKEN_LENGTH];			// Name of the label
	int address;						// Value of Location Counter when label was defined. -1 until defined
	int defined;						// 1 if label definition was found. 0 otherwise
	struct replace_list_node *list; 	// This will keep offset position of undefined symbol
	int vector; 						// Size of vector defined with SPACE directive. Zero until defined
	struct sym_table_node *next;		// Pointer to the next node in the symbol table
};

struct output_line {					//
	int opcode;							//
	int op[2];							//
	struct output_line *next;			//
};

// To be removed in final code
struct sym_table_node *SearchSym(struct sym_table_node *table, char *token);
void AddSym(struct sym_table_node **table, char *name, int address, int defined);
void DeleteSymTable(struct sym_table_node *table);
struct op_table_node *SearchOp(struct op_table_node *table, char *token);
int IsValidLabel(struct op_table_node table[], char *token);
void AddReplace(struct replace_list_node **node, int *n);
void ReplaceList(struct sym_table_node *node);
void AddLine(struct output_line *line, struct output_line **head);
void DeleteOutputLines(struct output_line *first);
void WriteObjectFile(FILE *fp, struct output_line *first);


int main() {

	char line[LINE_LENGTH];
	char token1[TOKEN_LENGTH];
	char input_file[] = "TestFiles/teste1.asm";
	char output_file[] = "TestFiles/Object.txt";
	char *dirTable[] = {"SPACE", "CONST", "SECTION"};
	int linePos = 0;
	int i = 0;
	int sec_text = 0;
	int sec_data = 0; 
	int offset = 0;
	int lc = 0;
	int line_count = 0;
	int label_count = 0;
	int opr_count = 0;
	int dir = -1;
	FILE *fp_in = NULL;
	FILE *fp_out = NULL;
	struct output_line *head = NULL;
	struct output_line *lineOut = NULL; //linha atual
	struct sym_table_node *symTable = NULL;
	struct sym_table_node *tmp_sym = NULL;
	struct sym_table_node *replace = NULL;
	struct op_table_node *tmp_op = NULL;
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

	if ((fp_in = fopen(input_file, "r")) == NULL) {
		printf("File not found.\n");
		return 0;
	} 

	if ((fp_out = fopen(output_file, "w")) == NULL) {
        printf("File not found!\n");
        return 0;
    }
    
    while (GetLine(fp_in, line)) {
    	printf("In first while\n");
    	line_count++;
    	label_count = 0;
    	linePos = 0;
    	dir = -1;
    	replace = NULL;
    	//tmp_op = NULL; //why?
    	lineOut = (struct output_line*)malloc(sizeof(struct output_line)); //free if finds an error?
    	lineOut->opcode = -1;
    	lineOut->op[0] = -1;
    	lineOut->op[1] = -1; //this will be tested before writing to file
    	lineOut->next = NULL;
    	while(GetToken2(line, token1, &linePos)) {
    		printf("In second while\n");
			if(token1[strlen(token1) - 1] == ':') {
    			printf("Found Label in line %d\n", line_count);
				if(IsValidLabel(opTable, token1)) {
					if(label_count == 0) {
						tmp_sym = SearchSym(symTable, token1);
						if(tmp_sym == NULL) {
							AddSym(&symTable, token1, lc, 1); //found label definition
							printf("Added label %s to symtable\n", token1);
							tmp_sym = symTable; //used by SPACE later?
						}
						else {
							if(tmp_sym->defined) {
								printf("Linha %d. Erro: Label redefinida.\n", line_count);
							}
							else { //Encontrou definição de uma label referenciada anteriormente.
								printf("Found definition of previous forward reference\n");
								tmp_sym->defined = 1;
								tmp_sym->address = lc;
								replace = tmp_sym; 
								//Can't replace yet because of vector length. will replace after the line is processed.
							}
						}
						label_count++;
					}
					else {
						printf("Linha %d. Erro: mais de uma label na mesma linha.\n", line_count);
						//como tratar este caso?
					}
				}
				else {
					printf("Linha %d. Erro: Label Inválida %s\n", line_count, token1);
				}
    		}
    		//Warning: things get REALLY confusing from here.
    		else {
				if(IsValid(token1)) {
					tmp_op = SearchOp(opTable, token1);
					if(tmp_op != NULL) { //found an operation
						printf("Found operation in line %d\n", line_count);
						if(sec_data) {
							printf("Linha %d. Erro: Instrução dentro da seção de dados.\n", line_count);
						}
						else {
							if(!sec_text) {
								printf("Linha %d. Erro: Instrução fora da seção de texto.\n", line_count);
							}
						}
						lc+=(tmp_op->operands+1);
						lineOut->opcode = tmp_op->opcode;
						opr_count = 0;
						// Gets operands and any other tokens in the line
						while(GetToken2(line, token1, &linePos)) {
							printf("In third while\n");
							if(IsValid(token1) || 
							// I think this won't work when firt operand of COPY has offset
							((opr_count == 0) && (!strcmp(tmp_op->name, "COPY")) && (token1[strlen(token1)-1] == ',') )) {
								if(opr_count < tmp_op->operands) {
									printf("Gonna search symbol!\n");
									tmp_sym = SearchSym(symTable, token1);
									if(tmp_sym != NULL) { //found operand in symTable
										if(tmp_sym->defined) {
											lineOut->op[opr_count] = tmp_sym->address;
										}
										else {
											lineOut->op[opr_count] = -1;
											AddReplace(&tmp_sym->list, &lineOut->op[opr_count]);	
										}
									}
									else {
										// First forward reference
										printf("Found forward reference to %s in line %d\n", token1, line_count);
										AddSym(&symTable, token1, -1, 0);
										printf("Added forward reference to SymTable\n");
										lineOut->op[opr_count] = -1;
										AddReplace(&symTable->list, &lineOut->op[opr_count]);
										tmp_sym = symTable; //used in next else
										printf("Added pointer for replacement of %s in line %d\n", token1, line_count);
									}
								}
								else {
									printf("Linha %d. Erro: número inválido de operandos em %s.\n", line_count, tmp_op->name);
								}
								opr_count++; //em que ponto(s) incrementar?
							}
							else {
								if((!strcmp(token1, "+")) && (opr_count > 0) && (opr_count <= tmp_op->operands)) {
									if(tmp_sym != NULL) { //if previous token was an operand
										if(GetToken2(line, token1, &linePos)) {
											if(IsNumber(token1)) { //checks if token is a number
												offset = atoi(token1);
												if(tmp_sym->defined) { //previous operand is defined in symTable
													if(offset <= tmp_sym->vector) {
														lineOut->op[opr_count-1]+=offset;
													}
													else {
														printf("Linha %d. Erro: offset muito grande! %s.\n", line_count);
													}
												}
												else {
													//puts offset at the last replace_node inserted at tmp_sym
													tmp_sym->list->offset = offset;
												}
											}
											else {
												printf("Linha %d. Erro: offset inválido %s.\n", line_count, token1);
											}
										}
										else {
											printf("Linha %d. Erro: Esperado offset após \"+\".\n", line_count);
										}
									}
									else {
										printf("Linha %d. Erro: \"+\" sem operando anterior.\n", line_count);
									}
									tmp_sym = NULL; //so if next token is invalid doesn't enter here.
								}
								else {
									printf("Linha %d. Erro: Operando Inválido %s.\n", line_count, token1);
								}
							}
						}
						
					}
					else {
						for(i = 0; i < 3; i++) {
							if(!strcmp(dirTable[i], token1)) {
								dir = i;
							}
						}
						if(dir > -1) {
							printf("Found directive %s in line %d\n", token1, line_count);
							if(dir != 2) {
								if(sec_text) {
										printf("Linha %d. Erro: Diretiva dentro da seção de texto.\n", line_count);
								}
								else {
									if(!sec_data) {
										printf("Linha %d. Erro: Diretiva fora da seção de dados.\n", line_count);
									}
								}
							}
							if(dir == 0) { //SPACE
								//incrementa LC e processa
								//inclui valor de vector no nodo apontado por replace
								if(label_count == 1) {
									i = 0;
									while(GetToken2(line, token1, &linePos)) {
										if(i == 0) {
											if(IsNumber(token1)) { //checks if token is a number
												offset = atoi(token1);
												lc += offset;
												tmp_sym->vector = offset;
												lineOut->opcode = 15;
												lineOut->op[0] = offset;
											}
											else {
												printf("Linha %d. Erro: esperava-se um número após SPACE.", line_count);
											}
										}
										i++;
									}
									if(i > 1) {
										printf("Linha %d. Erro: número inválido de operandos.\n", line_count);
									}
									else {
										if(i == 0) {
											lc += 1;
											lineOut->opcode = 15;
											lineOut->op[0] = 1;
										}
									}
								}
								else {
									printf("Linha %d. Erro: Diretiva SPACE sem Label.\n", line_count);
								} 
							}
							else {
								if(dir == 1) { // CONST
									//incrementa LC e processa
									//deve aceitar números positivos e negativos (inteiros e hexadecimais)
									if(label_count == 1) {
										i = 0;
										while(GetToken2(line, token1, &linePos)) {
											if(i == 0) {
												if(IsHex(token1)) {
													offset = HexToInt(token1);
													lc += offset;
													lineOut->opcode = 0;
													lineOut->op[0] = offset;
												}
												else {
													if(IsNumber(token1)) { //checks if token is a number
														offset = atoi(token1);
														lc += offset;
														lineOut->opcode = 0;
														lineOut->op[0] = offset;
													}
													else {
														printf("Linha %d. Erro: esperava-se um número após CONST.", line_count);
													}
												}
											}
											i++;
										}
										if(i == 0) {
											printf("Linha %d. Erro: Diretiva CONST sem operandos.\n", line_count);
										}
										else {
											if(i > 1) printf("Linha %d. Erro: número inválido de operandos.\n", line_count);
										}
									}
									else {
										printf("Linha %d. Erro: Diretiva CONST sem Label.\n", line_count);
									}
								}
								else {
									if(dir == 2) {
										if(label_count > 0) {
											printf("Linha %d. Erro: Diretiva SECTION com Label.\n", line_count);
										}
										else {	
											i = 0;
											while(GetToken2(line, token1, &linePos)) {
												if(i == 0) {
													if(IsValid(token1)) { //checks if token is a number
														if(!strcmp(token1, "TEXT")) {
															if(!sec_text) {	
																sec_text = 1;
																sec_data = 0;
																printf("Found TEXT Section.\n");
															}
															else {
																printf("Linha %d. Erro: mais de uma seção TEXT.\n", line_count);
																//O que fazer com o resto da seção?
															}
														}
														else {
															if(!strcmp(token1, "DATA")) {
																if(!sec_data) {	
																	sec_data = 1;
																	sec_text = 0;
																	printf("Found DATA Section.\n");
																}
																else {
																	printf("Linha %d. Erro: mais de uma seção DATA.\n", line_count);
																	//O que fazer com o resto da seção?
																}
															}
															else {
																printf("Linha %d. Erro: Operando da diretiva SECTION inválido.\n",line_count);
															}	
														}
													}
													else {
														printf("Linha %d. Erro: token inválido.", line_count);
													}
												}

												i++;
											}
											if(i == 0) {
												printf("Linha %d. Erro: Diretiva SPACE sem operandos.\n", line_count);
											}
											else {
												if(i > 1) {
													printf("Linha %d. Erro: número inválido de operandos.\n", line_count);
												}
											}
										} 
									}
									else { //primeiro token após label(se houver) não é instrução ou diretiva
										printf("Linha %d. Erro: Token desconhecido %s. Esperava-se instrução ou diretiva.\n", line_count, token1);
									}
								}
							}
						}
					}
				}
				else {
					printf("Linha %d. Erro: token inválido %s.\n", line_count, token1);
				}
    		}	
    	}
    	
    	if(lineOut->opcode != -1) {
    		printf("Adding line to output\n");
			AddLine(lineOut, &head);
		}
		else {
			free(lineOut);
		}
    	
    	if(replace != NULL) {
    		printf("Replacing list\n");
    		ReplaceList(replace);
    	}
    	printf("To next line o/\n");
    }
    //checar se há labels indefinidas na symTable antes?
    printf("Writing to object file\n");
    WriteObjectFile(fp_out, head);
    printf("Deleting output lines\n");
    DeleteOutputLines(head);
    printf("Deleting symbol table\n");
    DeleteSymTable(symTable);
}


//Found an undefined operand and need to include it in replace_list. Includes at the front of list
void AddReplace(struct replace_list_node **node, int *n) {
	struct replace_list_node *new = (struct replace_list_node*)malloc(sizeof(struct replace_list_node));
	new->replace = n;//does it work?
	new->offset = 0;
	new->next = *node;
	*node = new;//does it work?
}

/*
 * Replaces replace list of a node in SymTable and deletes the replace list
 */
void ReplaceList(struct sym_table_node *node) {
	struct replace_list_node *tmp = node->list;
	while (tmp != NULL) {
		printf("Replacing reference\n");
		*(tmp->replace) = (node->address + tmp->offset);
        tmp = tmp->next; //does it need to go after replacing? 
	}
	tmp = node->list;
	printf("Deleting list of replaces in node %s\n", node->label);
    while(node->list != NULL) {
    	tmp = node->list;
    	node->list = node->list->next;
    	free(tmp);
    }
}

struct sym_table_node *SearchSym(struct sym_table_node *table, char *token) {

	struct sym_table_node *tmp = table;
	
    while ((tmp != NULL)) {
    	printf("Searching for %s. I'm here: %s\n",token,tmp->label);
        if (!strcmp(tmp->label, token)) {
        	return tmp;
        }
        tmp = tmp->next;
    }
    return NULL; // Label not defined(?)
}

void AddSym(struct sym_table_node **table, char *name, int address, int defined) {
			
	int i = 0;
	struct sym_table_node* new = (struct sym_table_node*)malloc(sizeof(struct sym_table_node));
	for (i = 0; i < strlen(name); i++) {
		if (name[i] == ':') {
			new->label[i] = '\0';
		}
		else new->label[i] = name[i];
	}
	new->address = address;
	new->defined = defined;
	new->list = NULL;
	new->vector = 0;
	new->next = *table;
	*table = new;
}

void DeleteSymTable(struct sym_table_node *table) {
	struct sym_table_node* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}

struct op_table_node *SearchOp(struct op_table_node table[], char *token) {
	int n;
	struct op_table_node* tmp = NULL;
    for(n = 0; n < 14; n++) {
    	if(!strcmp(table[n].name, token)) tmp = &table[n];
    }
    return tmp;
}

int IsValidLabel(struct op_table_node table[], char *token) {
	
	int i = (strlen(token) - 1);

	if(isdigit(token[0])) {
		return 0;
	}
	printf("IsValidLabel: first character not digit\n");
	if(token[i] == ':') {
		token[i] = '\0';
	}
	i--;
	while (i >= 0) {
		if((!isalnum(token[i])) && (token[i] != '_')) {
			return 0;
		}
		i--;
	}
	printf("IsValidLabel: no forbidden characters in label\n");
	for(i = 0; i < 14; i++) {
		if(!strcmp(token, table[i].name)) {
			return 0;
		}
	}
	printf("IsValidLabel: label is not an operation\n");
	if(!strcmp(token, "CONST")) {
		return 0;
	}
	else {
		if(!strcmp(token, "SPACE")) {
			return 0;
		}
		else {
			if(!strcmp(token, "SECTION")) {
				return 0;
			}
		}
	}
	printf("IsValidLabel: label is not a directive\n");
	return 1;
}

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

void DeleteOutputLines(struct output_line *first) {
	struct output_line* tmp;
    while(first != NULL) {
    	tmp = first;
    	first = first->next;
    	free(tmp);
    }
}

void WriteObjectFile(FILE *fp, struct output_line *first) {
	int n;
	struct output_line *tmp = first;
	while(tmp != NULL) {
    	if(tmp->opcode > -1) {
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
    			else {
    				fprintf(fp, "%d %d ", tmp->opcode, tmp->op[0]);
    				if(tmp->op[1] > -1) {
    					fprintf(fp, "%d ", tmp->op[1]);
    				}
    				fprintf(fp, "\n");
    			}
    		}
    	}
    	tmp = tmp->next;
    }
}

