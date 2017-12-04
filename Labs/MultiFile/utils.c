/***********************************************************************
*
* FILE NAME: utils.c
*
*
* PURPOSE: 	Contains miscellaneous functions used by many other files in
*			the assembler code
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
#include "utils.h"


struct fileLines {				?
    int lineNum;
    int lineMod;
    struct fileLines *next;
};

/*
 * This function reads one line from the file pointed by fp
 * and returns a string with all comments and unnecessary white
 * spaces removed and all letters capitalized
 */
int GetLine(FILE *fp, char *lineBuffer) {
	
	char c;		// Keeps character read from file
	int n = 0;	// Index of current position in lineBuffer

	c = fgetc(fp);
	while ((c != '\n') && (n < LINE_LENGTH) && (c != EOF) && (c != ';')) {
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
	
	printf("Line: %s\n", lineBuffer);
	
	if (c != EOF) return 1;
	return 0;
}

/*
 * This function gets one token from the line pointed by lineBuffer
 * and returns it in tokenBuffer. p keeps the position of next token
 * in lineBuffer for the next call to GetToken
 */
int GetToken(char *lineBuffer, char *tokenBuffer, int *p) {
	printf("In GetToken\n");
	int n = 0;
	if (lineBuffer[(*p)] == '\n') return 0;

	while((!isspace(lineBuffer[(*p)])) && (n < 100)) {
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
 * This function tests if token is a valid label
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

	if(!strcmp(token, "EQU:")) {
		return 0;
	}
	else if(!strcmp(token, "IF:")) {
		return 0;
	}
	else if(!strcmp(token, "MACRO:")) {
		return 0;
	}
	else if(!strcmp(token, "ENDMACRO:")) {
		return 0;
	}
	else if(!strcmp(token, "SECTION:")) {
		return 0;
	}
	else if(!strcmp(token, "SPACE:")) {
		return 0;
	}
	else if(!strcmp(token, "CONST:")) {
		return 0;
	}
	else if(!strcmp(token, "BEGIN:")) {
		return 0;
	}
	else if(!strcmp(token, "END:")) {
		return 0;
	}
	else if(!strcmp(token, "EXTERN:")) {
		return 0;
	}
	else if(!strcmp(token, "PUBLIC:")) {
		return 0;
	}
	else if(!strcmp(token, "ADD:")) {
		return 0;
	}
	else if(!strcmp(token, "SUB:")) {
		return 0;
	}
	else if(!strcmp(token, "MULT:")) {
		return 0;
	}
	else if(!strcmp(token, "DIV:")) {
		return 0;
	}
	else if(!strcmp(token, "JMP:")) {
		return 0;
	}
	else if(!strcmp(token, "JMPN:")) {
		return 0;
	}
	else if(!strcmp(token, "JMPP:")) {
		return 0;
	}
	else if(!strcmp(token, "JMPZ:")) {
		return 0;
	}
	else if(!strcmp(token, "COPY:")) {
		return 0;
	}
	else if(!strcmp(token, "LOAD:")) {
		return 0;
	}
	else if(!strcmp(token, "STORE:")) {
		return 0;
	}
	else if(!strcmp(token, "INPUT:")) {
		return 0;
	}
	else if(!strcmp(token, "OUTPUT:")) {
		return 0;
	}
	else if(!strcmp(token, "STOP:")) {
		return 0;
	}

	if (token[i - 1] == ':') {
		token[i - 1] = '\0';
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
	
	// If first character is a digit return zero
	if(isdigit(token[i])) {
		return 0;
	}
	/* A token is valid if it contains only
	 * alphanumeric characters or underscores */
	while (token[++i] != '\0') {
		if((!isalnum(token[i])) && (token[i] != '_')) {
			return 0;
		}
	}
	return 1;
}

/*
 * This function tests if token is a decimal number
 */
int IsNumber(char *token) {

	int i = 0;
	int negative = 0;

	// Flags a negative number
	if(token[i] == '-') {
		negative = 1;
		i++;
	}

	while( i < strlen(token)) {
		// Test if character is a decimal digit
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

	int len = strlen(token);
	int i = 0;
	
	if(len > 2) { // Token is not just "0X"
		if(token[i] == '-') {
			if(len <= 3) return 0;
			negative = 1;
			i++;
		}
		if ((token[i] == '0') && (token[++i] == 'X')) {
			while( i < len ) {
				i++;
				/* Return 0 if character is not a digit and it is
				 * not a letter from A to F */
				if((token[i] < 48) || ((token[i] > 57) && 
				(token[i] < 65)) || (token[i] > 70)) {
					return 0;
				}
			}
		}
		else return 0;

		if(negative) {
			return -1;
		}

		return 1;
	}
	else return 0;
}

/* Power: raise base to n-th power; n >= 0 */
int Power(int base, int n) {

	int i;
	int p = 1;

	for (i = 1; i <= n; ++i) {
		p = p * base;
	}

	return p;
}

/*
 * This function converts a token that was tested with
 * IsHex to a decimal number
 */
int HexToInt(char *token) {

	int len = (strlen(token) - 1);
	int j = 0;
	int digit = 0;
	int sum = 0;
	char aux = token[len];

	while ((len > 0) && (aux != 'X')) {
		// if token[len] is between 0 and 9, obtain numeric value
		if ((aux > 47) && (aux < 58)) digit = aux - 48;
		// else if token[len] is between A and F, obtain numeric value
		else if ((aux > 64) && (aux < 71)) digit = aux - 55;
		sum+=(digit*(Power(16, j)));
		len--;
		j++;
		aux = token[len];
	}

	if (token[0] == '-') sum *= -1;

	return sum;
}


/*
 * Add lines at the end of the table
 * 	It is essentially used during Preprocess to create the table
 */
void AddLines(struct fileLines **table, int original, int modified) {

	struct fileLines* tmp = *table;
	struct fileLines* new = (struct fileLines*)malloc(sizeof(struct fileLines));

    new->lineNum = original;
    new->lineMod = modified;
    new->next = NULL;

    // If the table is empty, insert first node
	if (*table == NULL) {
        *table = new;
    }
	else {
    	// Else, search for empty space
    	while (tmp->next != NULL) {
    		tmp = tmp->next;
		}
   		tmp->next = new;
   	}
}

/*
 * Searches for lines in the table
 * 	It is used to find a given line addressed by the variable modified
 */
struct fileLines *SearchLines(struct fileLines *table, int modified) {

    struct fileLines* tmp = table;

    /* Searches whole table until correct modified line is found
     * or until the end is reached */
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
    struct fileLines* new = NULL;
    struct fileLines* tmp = NULL;
    struct fileLines* b4 = NULL;
    tmp = searchLines(table, modified);

    /*
    * The line found by searchLines is the line prior to the
    * macro calling. Therefore, a new line will be inserted
    * in between the previous line and the macro calling.
    */
    if(tmp != NULL) {
    	new = (struct fileLines*)malloc(sizeof(struct fileLines));
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
        while(tmp != NULL) {
        	/*
        	* If it is the first line to be modified,
        	* it means that it is the macro calling and
        	* the line will not be on the final output
        	* file. Therefore, its lineMod value must be 0.
        	*/
        	if(n == 0) {
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
