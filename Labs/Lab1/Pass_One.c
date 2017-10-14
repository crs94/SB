#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct opTable_node {
	char *name;
	int opcode;
	int operands;
};

//Is it really necessary?
/*struct dirTable_node {
	char *name;
	int operands;
};*/

struct replace_list {
	int *replace;
	int offset;
	struct replace_list *next;
};

struct symTable_node {
	char[101] label;
	int address;
	int defined;
	struct replace_list *list; //this will keep offset position of undefined symbol
	int vector;
	int line;
};

struct outputLine {
	int opcode;
	int op1;
	int op2;
	struct outputLine *next;
};

int getLine(FILE *fp, char *lineBuffer); //implementar um getline mais simples para esta passagem?
int getToken(char *lineBuffer, char *tokenBuffer, int p);
int countSpaces(char *line);
int isLabel(char *token);
int isValid(char *token);
struct symTable_node *searchSym(struct symTable_node *table, char *token);
void addSym(struct symTable_node **table, char *name, int address, int defined, int vector, int line);
void deleteSymTable(struct symTable_node *table);
struct opTable_node *searchOp(struct opTable_node *table, char *token);
//struct dirTable_node *searchOp(struct dirTable_node *table, char *token);

int main Pass_One() {
	char line[601], lineOut[601];
	char token1[101], token2[101];
	//char *tokens[7] = {token1, token2, token3, token4, token5, token6, token7};
	char filename[] = "TestFiles/TestFile2.asm";
	FILE *fp = NULL;
	int linePos = 0, i = 0, secText = 0, secData = 0;
	struct outputLine *head;
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
    	if(linePos = getToken(line, token1, linePos) {
    		if(isLabel(token1)) {
    			tmp_sym = searchSym(table, token1);
    			if(tmp_sym == NULL) {
    				addSym(&table, token1, lc, 1, 0, lineC);
    				if(linePos = getToken(line, token1, linePos) {
    					tmp_op = searchOp(opTable, token1);
    					if(tmp_op != NULL) {
    						lineOut = (struct outputLine*)malloc(size_of());
    						lineOut->opcode = tmp_op->opcode;
    						for(n = 0; n < tmp_op->operands ; n++) {
    							
    						}
    					}
    				}
    				/*else {
    					erro:
    				}*/
    			}
    			/*else {
    				erro: label redefinida
    			}*/
    		}
    	}
    }
}

int getLine(FILE *fp, char *lineBuffer) {

	char c;
	int n = 0;

	c = fgetc(fp);
	while ((c != '\n') && (n < 600) && (c != EOF) && (c != ';')) {
		if (n == 0) {
			// Ignores initial blanks
			while ((c == ' ') || (c == '\t')) {
				c = fgetc(fp);
			}
		}
		if (c == '\t') {
			c = ' ';
		}
		if ((lineBuffer[n - 1] == ' ') && (n != 0)) {
			// Ignores useless blanks in line
			while ((c == ' ') || (c == '\t')) {
				c = fgetc(fp);
			}
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
		// If a comment is identified,
		// the rest of the line is ignored
		while ((c = fgetc(fp)) != '\n');
	}
	lineBuffer[n] = '\n';
	lineBuffer[++n] = '\0';

	printf("%s",lineBuffer);

	if (c != EOF) return 1;
	return 0;
}

int getToken(char *lineBuffer, char *tokenBuffer, int p) {

	int n = 0;
	if (lineBuffer[p] == '\n') return 0;

	while ((!isspace(lineBuffer[p])) && (n < 100)) {
		tokenBuffer[n] = lineBuffer[p];
		n++;
		p++;
	}
	tokenBuffer[n] = '\0';

	if (lineBuffer[p] == '\n') return p;
	return ++p;
}

int isLabel(char *token) {

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

int countSpaces(char *line) {

	int n, count = 0;
	for(n = 0; line[n] != '\n'; n++) {
		if(isspace(line[n])) {
			count++;
		}
	}
	return(count+1);
}

int isValid(char *token) {

	int i = 0;
	while (token[i] != '\0') {
		if((!isalnum(token[i])) && (token[i] != '_')) {
			printf("Invalid\n");
			return 0;
		}
		i++;
	}
	return 1;
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
			FILE *fp, 
			int vector, 
			int line) {
			
	int i = 0;
	struct symTable_node* new = (struct symTable_node*)malloc(sizeof(struct symTable_node));
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

/*struct dirTable_node *searchOp(struct dirTable_node *table, char *token) {
	struct dirTable_node* tmp = table;
    while ((tmp != NULL)) {
    	printf("Searching for %s. I'm here: %s\n",token,tmp->name);
        if (!strcmp(tmp->name, token)) {
        	return tmp;
        }
        tmp = tmp->next;
    }
    return NULL; // Label not defined(?)
}*/


