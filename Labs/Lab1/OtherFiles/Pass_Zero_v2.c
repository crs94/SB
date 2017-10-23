#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"


struct MNT_node {
    char label[101];
    struct MDT_node *begin;
    struct MNT_node *next;
};

struct MDT_node {
    char line[601];
    struct MDT_node *next;
};

struct MNT_node *searchMNT(struct MNT_node *table, char *token);
struct MDT_node *addMDT(struct MDT_node **table, char *toAdd);
void addMNT(struct MNT_node **table, char *toAdd, struct MDT_node *first);
void deleteMDT(struct MDT_node *table);
void deleteMNT(struct MNT_node *table);

int main() {

    char line[601], lineOut[601];
	char token1[101], token2[101];
	char filename[] = "TestFiles/fatorial.asm";
	FILE *fp = NULL;
	int linePos = 0, i = 0, secText = 0, secData = 0, inMacro = 0, firstMacro = 0;
	struct MDT_node *MDT = NULL;
    struct MDT_node *tmpMDT = NULL;
    struct MNT_node *MNT = NULL;
	struct MNT_node *tmpMNT = NULL;

    if ((fp = fopen(filename, "r")) == NULL) {
        printf("404 Not Found!");
        exit(1);
    }

    while (getLine(fp, line)){
        printf("%s", line);
        linePos = 0;
        if (strstr(line, " MACRO ") || strstr(line, "MACRO ") || strstr(line, " MACRO\n")) {
            inMacro = 1;
            if(linePos = getToken(line, token1, linePos)) {
                if(isLabel(token1)) {
                    //firstMacro = 1;
                    printf("MACRO: %s\n", token1);
                    /*if(linePos = getToken(line, token2, linePos)) {
                    	erro: tokens após a macro                  	
                    }*/
                    //linePos = 0;
                    if(getLine(fp, line)) {
                    	tmpMDT = addMDT(&MDT, line);
                    	if(tmpMDT != NULL) {
                    		addMNT(&MNT, token1, tmpMDT);
                    		//checa erros?
                    		printf("Added first line in MACRO %s\n",token1);
                    	}
                    	/*else {
                    		erro
                    	}*/
                    }
                    /*else {
                    	erro: macro no final do arquivo
                    }*/
                   
                }
                /*else {
                	erro: label inválido
                }*/
            }
        }
        else {
        	if (inMacro) {
                printf("in MACRO\n");
                //tratar erro em que END não é primeiro token da linha?
                while(linePos = getToken(line, token1, linePos)) {
                		if(!strcmp(token1, "END")) {
                		inMacro = 0;
                		printf("Found END\n");
                	}
                }
                tmpMDT = addMDT(&MDT, line);
                printf("Added line to MDT\n");
            }
            else {
                printf("not MACRO\n");
                //while?
                if(linePos = getToken(line, token1, linePos)) {
                    tmpMNT = searchMNT(MNT, token1);
                    if (tmpMNT != NULL) {
                        tmpMDT = tmpMNT->begin;
                        /*while (!tmp) {
                            printf("im in!\n");
                            tmp = tmp->next;
                        }*/
                        printf("Found %s in MNT!\n",token1);
                        while ((strcmp(tmpMDT->line, "END\n")) && (strcmp(tmpMDT->line, "END "))) {
                            printf("%s\n", tmpMDT->line);
                            tmpMDT = tmpMDT->next;
                        }
                        printf("End of MACRO\n");
                    }
                }
            }
        }
        printf("EOL\n\n");
    }

    deleteMDT(MDT);
    deleteMNT(MNT);
    if (fclose(fp) == 0) {
    	printf("\nFile closed.");
    }

    return 0;
}

struct MNT_node *searchMNT(struct MNT_node *table, char *token) {

    struct MNT_node* tmp = table;
    while ((tmp != NULL)) {
    	printf("Searching for %s. I'm here: %s\n",token,tmp->label);
        if (!strcmp(tmp->label, token)) {
        	return tmp;
        }
        tmp = tmp->next;
    }
    return NULL; // Label not defined(?)
}

struct MDT_node *addMDT(struct MDT_node **table, char *toAdd) {

	int i = 0;
	struct MDT_node* tmp = *table;
	struct MDT_node* new = (struct MDT_node*)malloc(sizeof(struct MDT_node));
	strcpy(new->line, toAdd);
	new->next = NULL;
	if (tmp == NULL) {
        *table = new;
        return *table;
    }
    while (tmp->next != NULL) {
        tmp = tmp->next;
	}
	//*tmp = &(*new);
    tmp->next = new;
    return tmp;
}

void addMNT(struct MNT_node **table, char *toAdd, struct MDT_node *first) {

	int i = 0;
	struct MNT_node* new = (struct MNT_node*)malloc(sizeof(struct MNT_node));
	for (i = 0; i < strlen(toAdd); i++) {
		if (toAdd[i] == ':') {
			new->label[i] = '\0';
		}
		else new->label[i] = toAdd[i];
	}
	new->begin = first;
	new->next = *table;
	*table = new;
}

void deleteMDT(struct MDT_node *table) {

    struct MDT_node* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}

void deleteMNT(struct MNT_node *table) {

    struct MNT_node* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}

