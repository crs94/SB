/***********************************************************************
*
* FILE NAME: Util.c
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

/*
 * Write the comment text here, in complete sentences.
 * Use block comments when there is more than one sentence.
 */

#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct fileLines {
    int lineNum;
    int lineMod;
    struct fileLines *next;
};

/*
 * This function 
 * 
 */
int GetLine(FILE *fp, char *lineBuffer) {

	char c;
	int n = 0;

	c = fgetc(fp);
	while ((c != '\n') && (n < 600) && (c != EOF) && (c != ';')) {
		if (n == 0) {
			// Ignores initial blanks
			while ((c == ' ') || (c == '\t')) {
				c = fgetc(fp);
			}
			if ((c == ';') || (c == '\n')) break;
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
	if (n > 0) {
		if(lineBuffer[n-1] == ' ') n--;
		lineBuffer[n] = '\n';
		lineBuffer[++n] = '\0';
	}
	else {
		lineBuffer[0] = '\0';
	}
	if (c == EOF) printf("%s\n", lineBuffer);
	
	if (c != EOF) return 1;
	return 0;
}

//informar se token tem mais de 100 caracteres?
int GetToken(char *lineBuffer, char *tokenBuffer, int p) {

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

int IsLabel(char *token) {

	int i = 0;

	if(isdigit(token[i])) {
		return 0;
	}

	while (token[++i] != '\0') {
		if((!isalnum(token[i])) && (token[i] != '_')) {
			if((token[i] == ':') && (token[i+1] != '\0')) { // Why did we include this line?
				return 0;
			}
		}
	}

	if (token[i - 1] == ':') {
		return 1;
	}
	else {
		return 0;
	}
}

int IsValid(char *token) {

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

//deve retornar 1 para números positivos e negativos
int IsNumber(char *token) {
	int i;
	for(i = 0; i < strlen(token); i++) {
		if (!isdigit(token[i])) return 0;
	}
	return 1;
}

int IsHex(char *token) {

	int i;
	
	if((token[0] == '0') && (token[1] == 'X')) {
		for(i = 2; i<strlen(token); i++) {
			if((token[i] < 0) || (token[i] > 15)) return 0;
		}
		return 1;
	}
	
	return 0;
}

int HexToInt(char *token) {

	int i = strlen(token);
	int j = 0;
	int sum = 0;
	
	while(i > 0) {
		sum+=((int)token[i]*(pow(16, j)));
		i--;
		j++;
	}
	
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

    // Searches whole table until correct modified line is found
    // or until the end is reached
    while ((tmp != NULL)) {
    	printf("Searching for %d. I'm here: %d\n",modified,tmp->lineMod);
        if (tmp->lineMod == modified) {
        	return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

/*
 * Inserts lines in the middle of the table
 * 	It is used essentially during Pass_Zero to add the references of
 * 	the lines of a macro definition in the middle of the code
 */
int insertLines(struct fileLines *table, int original, int modified) {

    struct fileLines* new = (struct fileLines*)malloc(sizeof(struct fileLines));
    struct fileLines* tmp = NULL;
    tmp = searchLines(table, modified);

    if (tmp != NULL) {
        new->lineNum = 0;
        new->lineMod = modified;
        new->next = tmp->next;
        tmp->next = new;

        return modified;
    }
    return 0;
}

/*
 * Modifies lines in the table
 * 	It is used to modify the values of the lines as the lines of the
 * 	original file are modified throughout the program
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
