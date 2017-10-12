#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Util.h"

struct equTab {
	char label[101];
	char value[101];
	struct equTab *next;
};

int isEQULabel(char *token);
struct equTab *searchEQU(struct equTab *table, char *token);
void addEQU(struct equTab **table, char *label, char *digit);
void deleteEQU(struct equTab *table);

int main() {

	char line[601], lineOut[601];
	char token1[101], token2[101];
	//char *tokens[7] = {token1, token2, token3, token4, token5, token6, token7};
	char filename[] = "TestFiles/TestFile2.asm";
	FILE *fp = NULL;
	int linePos = 0, i = 0, secText = 0, secData = 0;
	struct equTab *equTable_Head = NULL;
	struct equTab *tmp = NULL;

	if ((fp = fopen(filename, "r")) == NULL) {
        printf("404 Not Found!");
        exit(1);
    }

    while (getLine(fp, line)) {
    	i = countSpaces(line);
    	//eliminates line break after label
    	/*if(i == 1) {
    		getLine(fp, lineOut);
    		line[strlen(line)-1] = ' ';
    		strcat(line, lineOut);
    	}*/
    	if (strstr(line, " EQU ") || strstr(line, "EQU ") || strstr(line, " EQU\n")) {
			/*while ((linePos = getToken(line, tokens[i], linePos)) && (i < 7)) {
	    		i++;
	    	}
	    	if (isEQULabel(tokens[0]) && (strcmp(tokens[1], "EQU")) == 0) {
	    		printf("isEQU\n ");
	    		addEQU(&equTable_Head, tokens[0], tokens[2]);
	    	}
	    	printf("%s\n",equTable->label);*/
	    	if(linePos = getToken(line, token1, linePos)) {
	    		if(isEQULabel(token1)) {
	    			linePos = getToken(line, token2, linePos);
	    			if (!strcmp(token2,"EQU")) {
	    				if(linePos = getToken(line, token2, linePos)) {
	    					if(isValid(token2)) {
	    						//checa se a label já está na tabela
		    					tmp = searchEQU(equTable_Head, token1);
		    					if(tmp != NULL) { //label já está na tabela
		    						strcpy(tmp->value, token2); //redefine label
		    					}
		    					else {
		    						tmp = searchEQU(equTable_Head, token2);
		    						if(tmp != NULL) {
		    							//nova label tem o mesmo valor de outra label
		    							strcpy(token2, tmp->value);
		    						}
		    						addEQU(&equTable_Head, token1, token2);
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
			linePos = 0;
	    }
	    else {
	    	if(strstr(line, "IF ") || strstr(line, " IF ") || strstr(line, " IF\n")) {
	    		printf("isIF\n");
	    		if(linePos = getToken(line, token1, linePos)) {
	    			if(!strcmp(token1, "IF")) {
	    				if(linePos = getToken(line, token1, linePos)) {
	    					tmp = searchEQU(equTable_Head, token1);
	    					if(tmp != NULL) { //found operand
	    						//checks if there is more than one operand
	    						if(!(linePos = getToken(line, token1, linePos))) {
	    							if(!strcmp(tmp->value,"0")) { //skip line if 0
	    								//what if IF is at the last line?
	    								getLine(fp, line);
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
    			while (linePos = getToken(line, token1, linePos)) {
    				tmp = searchEQU(equTable_Head, token1);
    				if(tmp != NULL) {
	    				printf("found in table\n");
	    				strcpy(token1, tmp->value);
 					}

    				//printf("%s\n",token1);
    				strcat(lineOut, token1);
    				strcat(lineOut, " ");
			   	}
			   	strcat(lineOut, "\n");
			   	printf("%s\n", lineOut);
    		}
    	}
    	i = 0;
    	lineOut[0] = '\0';
    }

    deleteEQU(equTable_Head);
    if (fclose(fp) == 0) {
    	printf("\nFile closed.");
    }

    return 0;
}

int isEQULabel(char *token) {

	int i = 0;
	if(isdigit(token[i])) {
		return 0;
	}
	i++;
	while (token[i] != '\0') {
		if((!isalnum(token[i])) && (token[i] != '_')) {
			if((token[i] == ':') && (token[i+1] != '\0')) {
				return 0;
			}
		}
		i++;
	}

	if (token[i - 1] == ':') {
		return 1;
	}
	else {
		return 0;
	}
}

struct equTab *searchEQU(struct equTab *table, char *token) {

    struct equTab* tmp = table;
    while ((tmp != NULL)) {
    	printf("Searching for %s. I'm here: %s\n",token,tmp->label);
        if (!strcmp(tmp->label, token)) {
        	return tmp;
        }
        tmp = tmp->next;
    }
    return NULL; // Label not defined(?)
}

void addEQU(struct equTab **table, char *name, char *digit) {

	//struct equTab* tmp = table;
	int i = 0;
	struct equTab* new = (struct equTab*)malloc(sizeof(struct equTab));
	for (i = 0; i < strlen(name); i++) {
		if (name[i] == ':') {
			new->label[i] = '\0';
		}
		else new->label[i] = name[i];
	}
	strcpy(new->value, digit);
	new->next = *table;
	*table = new;

	/*if(table==NULL) {
		printf("first!");
		table = new;
	}
	else {
		while (tmp != NULL) {
			tmp = tmp->next;
		}
		tmp->next = new;
	}*/
}

void deleteEQU(struct equTab *table) {

    struct equTab* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}
