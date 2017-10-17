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

struct sym_table_node *searchSym(struct sym_table_node *table, char *token);
void AddSym(struct sym_table_node **table, char *name, int address, int defined, int vector);
void DeleteSymTable(struct sym_table_node *table);
struct op_table_node *searchOp(struct op_table_node *table, char *token);
void AddReplace(struct sym_table_node *node, struct output_line *outLine, int n);
//addLine(lineOut, head);
//isOffset();

int main Pass_One() {

	char line[601], lineOut[601];
	char token1[101], token2[101];
	char filename[] = "TestFiles/TestFile2.asm";
	int linePos = 0, i = 0, secText = 0, secData = 0, offset = 0;
	FILE *fp = NULL;
	struct output_line *head = NULL;
	struct output_line *lineOut; //linha atual
	struct sym_table_node *symTable = NULL;
	struct sym_table_node *tmp_sym = NULL;
	struct op_table_node *tmp_op = NULL;
	struct op_table_node[14] opTable = {	"ADD", 1, 1,
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
    	linePos = 0;
    	lineOut = (struct output_line*)malloc(size_of(struct output_line)); //free if finds an error?
    	lineOut->op[1] = -1; //this will be tested before writing to file
    	if(linePos = GetToken(line, token1, linePos)) {
    	
    		if(IsLabel(token1)) {
    			tmp_sym = SearchSym(symTable, token1);
    			if(tmp_sym == NULL) {
    				AddSym(&table, token1, lc, 1, 0); //found label definition
    				//acrescentar replaceLists() aqui? no final antes de escrever no arquivo?
    			}
    			/*else {
    				definição da label ou erro: label redefinida
    			}*/
    		}
    		
    		else {
				if(IsValid(token1) {
					tmp_op = SearchOp(opTable, token1);
					if(tmp_op != NULL) { //found an operation
						lc+=(tmp_op->operands+1);
						lineOut->opcode = tmp_op->opcode;
						n = 0;
						tmp_sym = NULL;
						while(linePos = GetToken(line, token1, linePos)) { //gets operands
							//what if first operand is '+'?
							if(IsValid(token1) {
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
										AddSym(&symTable, token1, 0, 0, 0);
										lineOut->op[n] = -1;
										AddReplace(symTable, lineOut, n);
										tmp_sym = symTable;
									}
								}
								n++; //em que ponto(s) incrementar n?
							}
							else {
								if((!strcmp(token1, "+")) && (n>0) &&(n <= tmp_op->operands)) {
									if(tmp_sym != NULL) { //if previous token was an operand
										if(linePos = GetToken(line, token1, linePos)) {
											if(IsOffset(token1) { //checks if token is a number
												offset = atoi(token1);
												if(tmp_sym->defined) {
													if(offset <= tmp_sym->vetor) {
														lineOut->op[n-1]+=offset;
													}
												}
												else {
													//checar se não excede tamanho do vetor?
													tmp_sym->list->offset = offset;
												}
											}
											/*else {
												error: invalid offset
											}*/
										}
										/*else {
											error: missing offset number after '+'	
										}*/
									}
									/*else {
										error?
									}*/
								}
								/*else {
									error: invalid operand
								}*/
							}
						}
						if(n == tmp_op->operands) {
							lineOut->next = NULL;
							AddLine(lineOut, head);
						}
						/*else {
							else?
						}*/
					}
					
					else {
						if(!strcmp(token1, "SPACE")) {
							//incrementa LC e processa
						
						}
						else {
							if(!strcmp(token1, "CONST")) {
								//incrementa LC e processa
								//deve aceitar números positivos e negativos (inteiros e hexadecimais)
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
    	}
    }
}


//Found an undefined operand and need include in replace_list
void AddReplace(struct sym_table_node *node, struct output_line *outLine, int n) {
	struct replace_list_node *tmp = node->list;
	struct replace_list_node *new = (struct replace_list_node*)malloc(sizeof(struct replace_list_node));
	new->replace = outLine->op[n];//does it work?
	new->next = NULL;
	while(tmp->next != NULL) {
		tmp = tmp->next;
	}
	tmp->next = new;
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

void AddSym(struct sym_table_node **table, 
			char *name, 
			int address, 
			int defined,
			int vector) {
			
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
	new->vector = vector;
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

