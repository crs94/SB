#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct equTab {
	char label[101];
	char value[101];
	struct equTab *next;
};

int getLine(FILE *fp, char *lineBuffer);
int getToken(char *lineBuffer, char *tokenBuffer, int p);
int isEQULabel(char *token);
int searchEQU(struct equTab *table, char *token);
void addEQU(struct equTab **table, char *label, char *digit);
void deleteEQU(struct equTab *table);

int main() {

	char line[601], lineOut[601];
	char token1[101], token2[101], token3[101], token4[101], token5[101], token6[101], token7[101];
	char *tokens[7] = {token1, token2, token3, token4, token5, token6, token7};
	char filename[] = "TestFiles/SB_test_getline.asm";
	FILE *fp = NULL;
	int linePos = 0, i = 0;
	struct equTab *equTable_Head = NULL;

	if ((fp = fopen(filename, "r")) == NULL) {
        printf("404 Not Found!");
        exit(1);
    }

    while (getLine(fp, line)) {
    	lineOut[0] = '\0';
    	if (strstr(line, " EQU ")) {
			while ((linePos = getToken(line, tokens[i], linePos)) && (i < 7)) {
	    		i++;
	    	}
	    	if (isEQULabel(tokens[0]) && (strcmp(tokens[1], "EQU")) == 0) {
	    		printf("isEQU\n ");
	    		addEQU(&equTable_Head, tokens[0], tokens[2]);
	    	}
	    	//printf("%s\n",equTable->label);

	    }
	    else {
    		while (linePos = getToken(line, tokens[0], linePos)) {
    			if (searchEQU(equTable_Head, tokens[0])) {
    				printf("found in table\n");
    			}
    			strcat(lineOut, tokens[0]);
    			strcat(lineOut, " ");
    		}
		    printf("%s\n", lineOut);
    	}
    	i = 0;
    }

    deleteEQU(equTable_Head);
    if (fclose(fp) == 0) {
    	printf("\nFile closed.");
    }
    getchar();

    return 0;
}

int getLine(FILE *fp, char *lineBuffer) {
    // TODO Treat the case in which a label and the instruction
    //      are in different lines

	char c;
	int n = 0;

	while (((c = fgetc(fp)) != '\n') && (n <= 499) && (c != EOF) && (c != ';')) {
		if (n == 0) {
			// Ignores initial blanks
			while ((c == ' ') || (c == '\t')) {
				c = fgetc(fp);
			}
		}
		if (c == '\t') {
			c = ' ';
		}
		if (((lineBuffer[n - 1] == ' ') || (lineBuffer[n - 1] == '\t')) && (n != 0)) {
			// Ignores useless blanks in line
			while (c == ' ') {
				c = fgetc(fp);
			}
		}
		if (islower(c)) {
			// Converts char to upper case
			c = toupper(c);
		}
		lineBuffer[n] = c;
		n++;
	}
	if (c == ';') {
		// If a comment is identified,
		// the rest of the line is ignored
		while ((c = fgetc(fp)) != '\n');
	}
	lineBuffer[n] = '\n';
	lineBuffer[++n] = '\0';

	printf("%s", lineBuffer);

	if (c != EOF) return 1;
	else return 0;
}

int getToken(char *lineBuffer, char *tokenBuffer, int p) {

	int n = 0;

	while ((!isspace(lineBuffer[p])) && (n < 101)) {
		tokenBuffer[n] = lineBuffer[p];
		n++;
		p++;
	}
	tokenBuffer[n] = '\0';

	if (lineBuffer[p] == '\n') return 0;
	p++;
	return p;
}

int isEQULabel(char *token) {

	int i = 0;

	while (token[i] != '\0') i++;

	if (token[i - 1] == ':') return 1;
	else return 0;
}

int searchEQU(struct equTab *table, char *token) {

    struct equTab* tmp = table;
    while ((tmp != NULL)) {
    	printf("I'm here!\n");
        if (!strcmp(tmp->label, token)) {
        	strcpy(token, tmp->value);
        	return 1;
        }
        tmp = tmp->next;
    }
    return 0; // Label not defined(?)
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
