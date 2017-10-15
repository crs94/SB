#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Util.h"

struct opTable_node {
	char *name;
	int opcode;
	int operands;
};

struct replace_list_node {
	int *replace;
	int offset;
	struct replace_list *next;
};

struct symTable_node {
	char[101] label;
	int address;
	int defined;
	struct replace_list_node *list; //this will keep offset position of undefined symbol
	int vector; //zero until defined?
	//int line; ?
};

struct outputLine {
	int opcode;
	int op[2];
	struct outputLine *next;
};

struct symTable_node *searchSym(struct symTable_node *table, char *token);
void addSym(struct symTable_node **table, char *name, int address, int defined, int vector);
void deleteSymTable(struct symTable_node *table);
struct opTable_node *searchOp(struct opTable_node *table, char *token);
void addReplace(struct symTable_node *node, struct outputLine *outLine, int n, int offset);
//addLine(lineOut, head);

int main Pass_One() {
	char line[601], lineOut[601];
	char token1[101], token2[101];
	//char *tokens[7] = {token1, token2, token3, token4, token5, token6, token7};
	char filename[] = "TestFiles/TestFile2.asm";
	FILE *fp = NULL;
	int linePos = 0, i = 0, secText = 0, secData = 0, offset = 0;
	struct outputLine *head = NULL;
	//struct outputLine *tail;
	struct outputLine *lineOut; //linha atual
	struct symTable_node *symTable = NULL;
	struct symTable_node *tmp_sym = NULL;
	struct opTable_node *tmp_op = NULL;
	struct opTable_node[14] opTable = {	"ADD", 1, 1,
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
    
    while (getLine(fp, line)) {
    	linePos = 0;
    	lineOut = (struct outputLine*)malloc(size_of(struct outputLine)); //free if finds an error?
    	if(linePos = getToken(line, token1, linePos)) {
    		if(isLabel(token1)) {
    			tmp_sym = searchSym(symTable, token1);
    			if(tmp_sym == NULL) {
    				addSym(&table, token1, lc, 1, 0); //found label definition
    				//acrescentar replaceLists() aqui? no final antes de escrever no arquivo?
    			}
    			/*else {
    				definição da label ou erro: label redefinida
    			}*/
    		}
    		else {
				if(isValid(token1) {
					tmp_op = searchOp(opTable, token1);
					if(tmp_op != NULL) { //found an operation
						lineOut->opcode = tmp_op->opcode;
						n = 0;
						while(linePos = getToken(line, token1, linePos)) { //gets operands
							//what if first operand is '+'?
							if(n <= tmp_op->operands) {
								if(isValid(token1) {
									tmp_sym = searchSym(symTable, token1);
									if(tmp_sym != NULL) { //found operand in symTable
										if(tmp_sym->defined) {
											lineOut->op[n] = tmp_sym->address;
										}
										else {
											lineOut->op[n] = -1;
											addReplace(tmp_sym, lineOut, n, offset);	
										}
									}
									else {
										//forward reference
										addSym(&symTable, token1, 0, 0, 0);
										lineOut->op[n] = -1;
										addReplace(symTable, lineOut, n, offset);
										tmp_sym = symTable;
									}
									n++;
								}
								else {
									if((!strcmp(token1, "+")) && (n>0)) {
										if(tmp_sym != NULL) { //if previous token was an operand
											if(linePos = getToken(line, token1, linePos)) {
												if(isOffset(token1) {
													offset = atoi(token1);
													if(tmp_sym->defined) {
														lineOut->op[n-1]+=offset;
													}
													else {
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
						}
						if(n == tmp_op->operands) {
							lineOut->next = NULL;
							addLine(lineOut, head);
						}
						/*else {
							else?
						}*/
					}
					else {
						if(!strcmp(token1, "SPACE")) {
						
						}
						else {
							if(!strcmp(token1, "CONST")) {
							
							}
							else {
								if(!strcmp(token1, "SECTION")) {
								
								}
								/*else {
									erro?
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
void addReplace(struct symTable_node *node, struct outputLine *outLine, int n, int offset) {
	struct replace_list_node *tmp = node->list;
	struct replace_list_node *new = (struct replace_list_node*)malloc(sizeof(struct replace_list_node));
	new->replace = outLine->op[n];//does it work?
	new->offset = offset;
	new->next = NULL;
	while(tmp->next != NULL) {
		tmp = tmp->next;
	}
	tmp->next = new;
}

struct symTable_node *searchSym(struct symTable_node *table, char *token) {
	struct symTable_node* tmp = table;
    while ((tmp != NULL)) {
    	printf("Searching for %s. I'm here: %s\n",token,tmp->label);
        if (!strcmp(tmp->label, token)) {
        	return tmp;
        }
        tmp = tmp->next;
    }
    return NULL; // Label not defined(?)
}

void addSym(struct symTable_node **table, 
			char *name, 
			int address, 
			int defined,
			int vector) {
			
	int i = 0;
	struct symTable_node* new = (struct symTable_node*)malloc(sizeof(struct symTable_node));
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

void deleteSymTable(struct symTable_node *table) {
	struct symTable_node* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}

struct opTable_node *searchOp(struct opTable_node *table, char *token) {
	int n;
	struct opTable_node* tmp = NULL;
    for(n = 0; n < 14; n++) {
    	if(!strcmp(table[n]->name, token)) tmp = table[n];
    }
    return tmp;
}

