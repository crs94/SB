/******************************************************************
* NAME:
*
*
*
* PURPOSE:
*
*
*
* GLOBAL VARIABLES:
*
*
*
* Variable		Type		Description
* --------		----		-----------
*
*
*
* DEVELOPMENT HISTORY:
*
*
*
* Date	Author	Change Id	Release		Description Of Change
* ----	------	---------	-------		---------------------
*
*
*
*******************************************************************/

/*
* Write the comment text here, in complete sentences.
* Use block comments when there is more than one sentence.
*/

/*
* File opening. Attempts to open the file indicated by 
* filename and displays an error message if unsuccessful.
*/
FILE *OpenFile(char *filename) {

    FILE *fp;

    if ((fp = fopen(filename, "r")) == NULL) {
        printf("File not found: %s\n",filename);
        exit(1);
    }
    
    return fp;
}

/*
* Function GetLine
* Working:
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


