#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Util.h"

struct equ_tab {
	char label[101];
	char value[101];
	struct equ_tab *next;
};

struct equ_tab *SearchEQU(struct equ_tab *table, char *token);
void AddEQU(struct equ_tab **table, char *label, char *digit);
void DeleteEQU(struct equ_tab *table);

int main() {

	char line[601], lineOut[601], aux[601];
	char token1[101], token2[101];
	char filename[] = "TestFiles/SB_test_getline.asm", output[] = "TestFiles/output.txt";
	FILE *fp = NULL;
	FILE *out = NULL;
	int linePos = 0, i = 0, secText = 0, secData = 0;
	struct equ_tab *equTable_Head = NULL;
	struct equ_tab *tmp = NULL;

	if ((fp = fopen(filename, "r")) == NULL) {
        printf("404 Not Found!");
        exit(1);
    }

	if ((out = fopen(output, "w")) == NULL) {
        printf("404 Not Found!");
        exit(1);
    }

    line[0] = '\0';
    lineOut[0] = '\0';

    while (GetLine(fp, line)) {
	    if (strlen(line) > 0) {
	    	
	    	if (line[strlen(line)-2] == ':') {
    			line[strlen(line)-1] = '\0';
    			printf("%s", line);
    			do {
    				GetLine(fp, aux); //checar EOF?
    			} while (strlen(aux) == 0);
    			strcat(line, " ");
    			printf("%s", line);
    			strcat(line, aux);
    			printf("%s", line);
    		}
    		
	    	if (strstr(line, " EQU ") || strstr(line, "EQU ") || strstr(line, " EQU\n")) {
		    	if(linePos = GetToken(line, token1, linePos)) {
		    		if(IsLabel(token1)) {
		    			linePos = GetToken(line, token2, linePos);
		    			if(!strcmp(token2,"EQU")) {
		    				if(linePos = GetToken(line, token2, linePos)) {
		    					if(IsValid(token2)) {
		    						//checa se a label já está na tabela
			    					tmp = SearchEQU(equTable_Head, token1);
			    					if(tmp != NULL) { //label já está na tabela
			    						strcpy(tmp->value, token2); //redefine label
			    					}
			    					else {
			    						tmp = SearchEQU(equTable_Head, token2);
			    						if(tmp != NULL) {
			    							//nova label tem o mesmo valor de outra label
			    							strcpy(token2, tmp->value);
			    						}
			    						AddEQU(&equTable_Head, token1, token2);
			    						printf("Added %s to EQUTable\n\n",equTable_Head->label);
									}
								}
								/*else {
									//erro: token inválido
								}*/
		    				}
		    				/*else {
		    					//erro: EQU sem operando
		    				}*/
		    			}
		    			/*else {
		    				tratar casos: duas labels,
		    			}*/
		    		}
		    		/*else {
		    			erros:  EQU com label inválida ou sem label
		    			ação a tomar:?
		    			tratar caso label = "EQU:" ou label é o nome de inst?
		    		}*/
		    	}
		    }	
		    	    
		    else {		    
		    	if(strstr(line, "IF ") || strstr(line, " IF ") || strstr(line, " IF\n")) {
		    		printf("isIF\n");
		    		if(linePos = GetToken(line, token1, linePos)) {
		    			if(!strcmp(token1, "IF")) {
		    				if(linePos = GetToken(line, token1, linePos)) {
		    					tmp = SearchEQU(equTable_Head, token1);
		    					if(tmp != NULL) { //found operand
		    						//checks if there is more than one operand
		    						if(!(linePos = GetToken(line, token1, linePos))) {
		    							if(!strcmp(tmp->value,"0")) { //skip line if 0
		    								//what if IF is at the last line?
		    								GetLine(fp, line);
		    								printf("skipping line: %s\n",line);
		    							}
		    							else {
		    								//Operando do IF igual a 1
		    								if(!strcmp(tmp->value,"1")) {
		    									printf("valid IF\n");
		    								}
		    								else {
		    								//operando diferente de 0 ou 1
		    								//indica erro operando inválido
	
		    								}
		    							}
		    						}
		    						/*else {
		    							erro: IF com número incorreto de operandos
		    						}*/
		    					}
		    					/*else {
		    						erro: IF com operando não definido
		    					}*/
		    				}
		    				/*else {
		    					erro: IF sem operando
		    				}*/
		    			}
		    			/*else {
		    				erro: IF com label
		    			}*/
		    		}
		    	}
		    	
		    	else {		    	
		    		printf("isOther\n");
		    		while (linePos = GetToken(line, token1, linePos)) {
	    				tmp = SearchEQU(equTable_Head, token1);
	    				if(tmp != NULL) {
		    				printf("found in table\n");
		    				strcpy(token1, tmp->value);
	 					}
	
	    				//printf("%s\n",token1);
	    				strcat(lineOut, token1);
	    				strcat(lineOut, " ");
				   	}
				   	strcat(lineOut, "\n");
		    		fprintf(out, "%s", lineOut);
				   	printf("%s\n", lineOut);
	    		}
	    		
	    	}
	    	
	    	i = 0;
	    	lineOut[0] = '\0';
	    	linePos = 0;
	    }
    }

    DeleteEQU(equTable_Head);
    if ((fclose(fp) == 0) && (fclose(out) == 0)) {
    	printf("\nFiles closed.");
    }

    return 0;
}

struct equ_tab *SearchEQU(struct equ_tab *table, char *token) {

    struct equ_tab* tmp = table;
    while ((tmp != NULL)) {
    	printf("Searching for %s. I'm here: %s\n",token,tmp->label);
        if (!strcmp(tmp->label, token)) {
        	return tmp;
        }
        tmp = tmp->next;
    }
    return NULL; // Label not defined(?)
}

void AddEQU(struct equ_tab **table, char *name, char *digit) {

	//struct equTab* tmp = table;
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

void DeleteEQU(struct equ_tab *table) {

    struct equ_tab* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}
