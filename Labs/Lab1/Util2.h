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

#define LINE_LENGTH 560
#define TOKEN_LENGTH 101


int GetLine(FILE *fp, char *lineBuffer);

//int GetToken(char *lineBuffer, char *tokenBuffer, int p);
int GetToken2(char *lineBuffer, char *tokenBuffer, int *p);

int IsLabel(char *token);

int IsValid(char *token);

int IsNumber(char *token);

int IsHex(char *token);

int HexToInt(char *token);

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
	//if ((n == 1) && (c == ';')) lineBuffer[0] = '\0';

	if (c != EOF) return 1;
	return 0;
}

//informar se token tem mais de 100 caracteres?
/*
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
}*/

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
		if(!isdigit(token[i])) {
			return 0;
		}
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

