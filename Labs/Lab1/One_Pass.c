#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Util.h"

struct op_table_node {
	char *name;
	int opcode;
	int operands;
};

struct replace_list_node {
	int *replace;
	int offset;
	struct replace_list_node *next;
};

struct sym_table_node {
	char[101] label;
	int address;
	int defined;
	struct replace_list_node *list; //this will keep offset position of undefined symbol
	int vector; //zero until defined?
	//int line; ?
	struct sym_table_node *next;
};

struct output_line {
	int opcode;
	int op[2];
	struct output_line *next;
};

struct sym_table_node *SearchSym(struct sym_table_node *table, char *token);
void AddSym(struct sym_table_node **table, char *name, int address, int defined);
void DeleteSymTable(struct sym_table_node *table);
struct op_table_node *SearchOp(struct op_table_node *table, char *token);
void AddReplace(struct sym_table_node *node, struct output_line *outLine, int n);
//AddLine(lineOut, head);
//IsNumber(char *token);
void IsHex(char *token);

int main Pass_One() {

	char line[601];
	char token1[101], token2[101];
	char filename[] = "TestFiles/TestFile2.asm";
	int linePos = 0, i = 0, secText = 0, secData = 0, offset = 0, lc = 0, linec = 0, opr_count = 0;
	FILE *fp = NULL;
	struct output_line *head = NULL;
	struct output_line *lineOut; //linha atual
	struct sym_table_node *symTable = NULL;
	struct sym_table_node *tmp_sym = NULL;
	struct sym_table_node *replace = NULL;
	struct op_table_node *tmp_op = NULL;
	struct op_table_node[14] opTable = {"ADD", 1, 1,
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

	if ((fp = fopen(filename, "r")) == NULL) {
        printf("404 Not Found!");
        exit(1);
    }
    
    while (GetLine(fp, line)) {
    	linec++;
    	label_count = 0;
    	linePos = 0;
    	replace = NULL;
    	tmp_op = NULL; //why?
    	lineOut = (struct output_line*)malloc(size_of(struct output_line)); //free if finds an error?
    	lineOut->op[1] = -1; //this will be tested before writing to file
    	while(linePos = GetToken(line, token1, linePos)) {

    		if(IsLabel(token1)) { //como detectar dois ou mais rótulos na mesma linha?
				if(IsValidLabel(opTable, token1)) {
					if(label_count == 0) {
						tmp_sym = SearchSym(symTable, token1);
						if(tmp_sym == NULL) {
							AddSym(&symTable, token1, lc, 1); //found label definition
							tmp_sym = symTable; //used by SPACE later?
						}
						else {
							if(tmp_sym->defined) {
								printf("Linha %d. Erro: Label redefinida\n", linec);
							}
							else { //Encontrou definição de uma label referenciada anteriormente.
								tmp_sym->defined = 1;
								tmp_sym->address = lc;
								replace = tmp_sym; //Can't replace yet because of vector length.
							}
						}
						label_count++;
					}
					else {
						printf("Linha %d. Erro: mais de uma label na mesma linha.\n", linec);
					}
				}
				else {
					printf("Linha %d. Erro: Label Inválida %s\n", linec, token1);
				}
    		}
    		//Warning: things get REALLY confusing from here.
    		else {
				if(IsValid(token1) {
					tmp_op = SearchOp(opTable, token1);
					if(tmp_op != NULL) { //found an operation
						lc+=(tmp_op->operands+1);
						lineOut->opcode = tmp_op->opcode;
						opr_count = 0;
						tmp_sym = NULL;
						//gets operands and any other tokens in the line
						while(linePos = GetToken(line, token1, linePos)) { 
							if(IsValid(token1) || 
							((n == 0) && (!strcmp(tmp_op->name, "COPY")) && (token1[strlen(token1)-1] == ',') )) {
								if(n < tmp_op->operands) {
									tmp_sym = SearchSym(symTable, token1);
									if(tmp_sym != NULL) { //found operand in symTable
										if(tmp_sym->defined) {
											lineOut->op[n] = tmp_sym->address;
										}
										else {
											lineOut->op[n] = -1;
											AddReplace(tmp_sym, lineOut, n);	
										}
									}
									else {
										//forward reference
										AddSym(&symTable, token1, -1, 0);
										lineOut->op[n] = -1;
										AddReplace(symTable, lineOut, n);
										tmp_sym = symTable; //used in next else
									}
								}
								else {
									printf("Linha %d. Erro: número inválido de operandos em %s.\n", linec, tmp_op->name);
								}
								opr_count++; //em que ponto(s) incrementar n?
							}
							//falta tratar virgulas em COPY
							else {
								if((!strcmp(token1, "+")) && (n>0) && (n <= tmp_op->operands)) {
									if(tmp_sym != NULL) { //if previous token was an operand
										if(linePos = GetToken(line, token1, linePos)) {
											if(IsNumber(token1) { //checks if token is a number
												offset = atoi(token1);
												if(tmp_sym->defined) { //previous operand is defined in symTable
													if(offset <= tmp_sym->vetor) {
														lineOut->op[n-1]+=offset;
													}
													else {
														printf("Linha %d. Erro: offset muito grande! %s.\n", linec, token1);
													}
												}
												else {
													//puts offset at the last replace_node inserted at tmp_sym
													tmp_sym->list->offset = offset;
												}
											}
											else {
												printf("Linha %d. Erro: offset inválido %s.\n", linec, token1);
											}
										}
										else {
											printf("Linha %d. Erro: Esperado offset após \"+\".\n", linec);
										}
									}
									else {
										printf("Linha %d. Erro: \"+\" sem operando anterior.\n", linec);
									}
									tmp_sym = NULL; //so if next token is invalid doesn't enter here.
								}
								else {
									printf("Linha %d. Erro: Operando Inválido %s.\n", linec, token1);
								}
							}
						}
						
					}
					
					else {
						//Can SPACE work with vectors?
						if(!strcmp(token1, "SPACE")) {
							//incrementa LC e processa
							//inclui valor de vector no nodo apontado por replace
							if(label_count == 1) {
								n = 0;
								while(linePos = GetToken(line, token1, linePos)) {
									if(n == 0) {
										if(IsNumber(token1) { //checks if token is a number
											offset = atoi(token1);
											tmp_sym->vector = offset;
										}
										else {
											printf("Linha %d. Erro: esperava-se um número após SPACE.", linec);
										}
									}

									n++;
								}
								if(n == 0) {
									printf("Linha %d. Erro: Diretiva SPACE sem operandos.\n", linec);
								}
								else {
									if(n > 1) printf("Linha %d. Erro: número inválido de operandos.\n", linec);
								}
							}
							else {
								printf("Linha %d. Erro: Diretiva SPACE sem Label.\n", linec);
							} 
						}
						else {
							if(!strcmp(token1, "CONST")) {
								//incrementa LC e processa
								//deve aceitar números positivos e negativos (inteiros e hexadecimais)
								if(label_count == 1) {
									n = 0;
									while(linePos = GetToken(line, token1, linePos)) {
										if(n == 0) {
											if(IsNumber(token1) { //checks if token is a number
												offset = atoi(token1);
												tmp_sym->vector = offset;
											}
											else {
												if(IsHex(token1)) {
													if(IsNumber(token1) { //checks if token is a number
														offset = atoi(token1);
														tmp_sym->vector = offset;
													}
													else {
														printf("Linha %d. Erro: esperava-se um número após CONST.", linec);
													}
												}	
												else{
													printf("Linha %d. Erro: esperava-se um número após CONST.", linec);
												}
											}
										}
										n++;
									}
									if(n == 0) {
										printf("Linha %d. Erro: Diretiva SPACE sem operandos.\n", linec);
									}
									else {
										if(n > 1) printf("Linha %d. Erro: número inválido de operandos.\n", linec);
									}
								}
								else {
									printf("Linha %d. Erro: Diretiva CONST sem Label.\n", linec);
								}
							}
							else {
								if(!strcmp(token1, "SECTION")) {
								
								}
								/*else {
									erro? Misplaced token?
								}*/
							}
						}
					}
				}
				
				/*else {
					erro: token inválido?
				}*/
    		}
    		if(n == tmp_op->operands) {
				lineOut->next = NULL;
				AddLine(lineOut, head);
			}	
    	}
    	
    	if(replace != NULL) {
    		ReplaceList();
    	}
		/*else {
			else?
		}*/
    }
}


//Found an undefined operand and need include in replace_list. includes at the front of list
void AddReplace(struct sym_table_node *node, struct output_line *outLine, int n) {
	struct replace_list_node *tmp = node->list;
	struct replace_list_node *new = (struct replace_list_node*)malloc(sizeof(struct replace_list_node));
	new->replace = outLine->op[n];//does it work?
	new->offset = 0;
	new->next = node->list;
	node->list=new;//does it work?
}

struct sym_table_node *SearchSym(struct sym_table_node *table, char *token) {
	struct sym_table_node* tmp = table;
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
			
	int i = strlen(name);
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

struct op_table_node *SearchOp(struct op_table_node *table, char *token) {
	int n;
	struct op_table_node* tmp = NULL;
    for(n = 0; n < 14; n++) {
    	if(!strcmp(table[n]->name, token)) tmp = table[n];
    }
    return tmp;
}

int IsValidLabel(struct op_table_node *table[], char *token) {

	int i = strlen(token);
	
	if(token[i-1] == ':') {
		token[i-1] = '\0';
	}
	
	while (--i>=0) {
		if((!isalnum(token[i])) && (token[i] != '_')) {
			printf("Invalid\n");
			return 0;
		}
	}
	
	for(i = 0; i < 14; i++) {
		if(!strcmp(token, table[i]->name)) return 0;
	}
	
	if(!strcmp(token, "CONST") return 0;
	if(!strcmp(token, "SPACE") return 0;
	if(!strcmp(token, "SECTION") return 0;
	
	return 1;
}

void IsHex(char *token) {
	int i;
	if((token[0] == '0') && (token[1] == 'X')) {
		for(i = 0; i<strlen(token); i++) {
			token[i] = token[i+2];
		}
		return 1;
	}
	return 0;
}

