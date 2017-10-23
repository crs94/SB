/***********************************************************************
*
* FILE NAME: utils.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "utils.h"

#define LINE_LENGTH 560
#define TOKEN_LENGTH 101

struct fileLines {
    int lineNum;
    int lineMod;
    struct fileLines *next;
};

// To be removed in final code
int GetLine(FILE *fp, char *lineBuffer);

int GetToken(char *lineBuffer, char *tokenBuffer, int p);

int GetToken2(char *lineBuffer, char *tokenBuffer, int *p);

int IsLabel(char *token);

int IsValid(char *token);

int IsNumber(char *token);

int IsHex(char *token);

int HexToInt(char *token);

void addLines(struct fileLines **table, int original, int modified);

struct fileLines *searchLines(struct fileLines *table, int modified);

int insertLines(struct fileLines *table, int original, int modified);

int modifyLines(struct fileLines *table, int line, int modified);

void deleteLines(struct fileLines *table);

/*
 * This function reads one line from the file pointed by fp
 * and returns a string with all comments and unnecessary white
 * spaces removed and all letters capitalized
 */
int GetLine(FILE *fp, char *lineBuffer) {

	char c;		// Keeps character read from file
	int n = 0;	// Index of current position in lineBuffer

	c = fgetc(fp);
	while ((c != '\n') && (n < 600) && (c != EOF) && (c != ';')) {
		if (n == 0) {
			// Ignores initial blanks
			while ((c == ' ') || (c == '\t')) {
				c = fgetc(fp);
			}
			// Breaks if line is empty or only has comments
			if ((c == ';') || (c == '\n')) break;
		}

		// Changes tabs to spaces to keep lines uniform
		if (c == '\t') {
			c = ' ';
		}

		// Ignores multiple spaces between tokens
		if ((lineBuffer[n - 1] == ' ') && (n != 0)) {
			while ((c == ' ') || (c == '\t')) {
				c = fgetc(fp);
			}
			// Breaks if newline was found after multiple spaces
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
		// If a comment is identified, the rest of the line is ignored
		while ((c = fgetc(fp)) != '\n');
	}

	// If line wasn't empty
	if (n > 0) {
		if(lineBuffer[n-1] == ' ') n--;
		lineBuffer[n] = '\n';
		lineBuffer[++n] = '\0';
	}

	else {
		lineBuffer[0] = '\0';
	}

	if (c != EOF) return 1;
	return 0;
}

/*
 * This function gets one token from the line pointed by lineBuffer
 * and returns it in tokenBuffer. p keeps the position of next token
 * in lineBuffer for the next call to GetToken
 */
int GetToken(char *lineBuffer, char *tokenBuffer, int p) {

	int n = 0;
	if (lineBuffer[p] == '\n') return 0; // No token left in line

	while ((!isspace(lineBuffer[p])) && (n < 100)) {
		tokenBuffer[n] = lineBuffer[p];
		n++;
		p++;
	}
	tokenBuffer[n] = '\0';

	if (lineBuffer[p] == '\n') return p;
	return ++p;
}

/*
 * Alternative version of GetToken
 */
int GetToken2(char *lineBuffer, char *tokenBuffer, int *p) {
	printf("In GetToken2\n");
	int n = 0;
	if (lineBuffer[(*p)] == '\n') return 0;

	while ((!isspace(lineBuffer[(*p)])) && (n < 100)) {
		tokenBuffer[n] = lineBuffer[(*p)];
		n++;
		(*p)++;
	}
	tokenBuffer[n] = '\0';
	printf("Token: %s\n", tokenBuffer);
	if (lineBuffer[(*p)] != '\n') (*p)++;
	return 1;
}

/*
 * This function tests if token is a label
 */
int IsLabel(char *token) {

	int i = 0;

	// If first character is a digit return zero
	if(isdigit(token[i])) {
		return 0;
	}

	while (token[++i] != '\0') {

		 /* A token is valid if it contains only
		  * alphanumeric characters or underscores */
		if((!isalnum(token[i])) && (token[i] != '_')) {
			/* Return 0 if colon is found and it is not
			 * at the last position in token */
			if((token[i] == ':') && (token[i+1] != '\0')) {
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

/*
 * This function tests if token is valid
 * It is assumed it is not a label
 */
int IsValid(char *token) {

	int i = 0;

	while (token[i] != '\0') {
		if((!isalnum(token[i])) && (token[i] != '_')) {
			return 0;
		}
		i++;
	}
	return 1;
}

/*
 * This function tests if token is a decimal number
 */
int IsNumber(char *token) {

	int i = 0;
	int negative = 0;

	if(token[i] == '-') {
		negative = 1;
		i++;
	}

	while( i < strlen(token)) {
		if (!isdigit(token[i])) {
			return 0;
		}
		i++;
	}

	if(negative) {
		return -1;
	}

	return 1;
}

/*
 * This function tests if token is a hexadecimal number
 */
int IsHex(char *token) {

	int i;

	if((token[0] == '0') && (token[1] == 'X')) {
		for(i = 2; i<strlen(token); i++) {
			if((token[i] < 48) || ((token[i] > 57) && (token[i] < 65)) || (token[i] > 70)) return 0;
		}
		return 1;
	}

	return 0;
}

/*
 * This function converts a token that was tested with
 * IsHex to a decimal number
 */
int HexToInt(char *token) {

	//int i = strlen(token);
	//int j = 0;
	int num = 0;

	/*while(i > 0) {
		sum+=(token[i]*(pow(16, j)));
		i--;
		j++;
	}*/
	sscanf(token, "%x", num);
	sprintf(token, "%d", num);

	return num;
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

    /*
    * Searches whole table until correct modified line is found
    * or until the end is reached
    */
    while ((tmp != NULL)) {
    	if (tmp->lineMod == modified) {
        	// If found, returns a pointer to the line in the table
            return tmp;
        }
        tmp = tmp->next;
    }
    // Else, returns a NULL pointer
    return NULL;
}

/*
 * Inserts lines in the middle of the table
 * 	It is used essentially during Pass_Zero to add the references of
 * 	the lines of a macro definition in the middle of the code
 */
int insertLines(struct fileLines *table, int original, int modified) {

	int n = 0, update = 0;
    struct fileLines* new = (struct fileLines*)malloc(sizeof(struct fileLines));
    struct fileLines* tmp = NULL;
    struct fileLines* b4 = NULL;
    tmp = searchLines(table, modified);

    /*
    * The lines found by searchLines is the line prior the
    * macro calling. Therefore, a new line will be inserted
    * in between teh previous line and the macro calling.
    */
    if (tmp != NULL) {
        new->lineNum = original;
        new->lineMod = modified + 1;
        new->next = tmp->next;
        tmp->next = new;

        update = modified + 1;
        b4 = new;
		tmp = new->next;

		/*
		* The rest of the table needs to be updated so the
		* lineMod variable will match the new order imposed
		* by the insertion of the new line.
		*/
        while (tmp != NULL) {
        	/*
        	* If it is the first line to be modified,
        	* it means that it is the macro calling and
        	* the line will not be on the final output
        	* file. Therefore, its lineMod value must be 0.
        	*/
        	if (n == 0) {
        		tmp->lineMod = 0;
        		b4 = tmp;
        		tmp = tmp->next;
        	}
        	else {
                /*
                * Guarantees that only the lines that were
                * not equal 0 are being modified.
                */
                if (tmp->lineMod) {
                    update++;
                    tmp->lineMod = update;
                    b4 = tmp;
                    tmp = tmp->next;
                }
	        }
	        n++;
        }
        /*
        * If insertion runs smoothly, returns the
        * value of the new line that has just been
        * inserted into the table
        */
        return modified + 1;
    }
    // Else, returns 0
    return 0;
}

/*
 * Modifies lines in the table
 * 	It is used to modify the values of the lines as the lines
 * 	of the original file are modified throughout the program
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
