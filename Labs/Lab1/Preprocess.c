#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct equTab {
	char label[101];
	int value;
	struct equTab *next;
};

int getLine(FILE *fp, char *lineBuffer);
int getToken(char *lineBuffer, char *tokenBuffer, int p);

int main() {

	char line[501], token[101];
	char filename[] = "TestFiles/SB_test_getline.asm";
	FILE *fp;
	int linePos = 0;

	if ((fp = fopen(filename, "r")) == NULL) {
        printf("404 Not Found!");
        exit(1);
    }

    while (getLine(fp, line)) {
    	while (linePos = getToken(line, token, linePos)) {
			;
    	}
    }

    if (fclose(fp) == 0) {
    	printf("\nFile closed.");
    }
    getchar();

    return 0;
}

int getLine(FILE *fp, char *lineBuffer) {

	char c;
	int n = 0;

	do {
		while (((c = fgetc(fp)) != '\n') && (n <= 499) && (c != EOF)) {
			if (n == 0) {
				// Ignores initial blanks
				while ((c == ' ') || (c == '\t')) {
					c = fgetc(fp);
				}
			}
			if (c != ';') {
				if ((lineBuffer[n - 1] == ' ') && (n != 0)) {
					// Ignores useless blanks in line
					while ((c == ' ') || (c == '\t')) {
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
			else {
				while (((c = fgetc(fp)) != '\n') && (c != EOF)) {
					;
				}
			}
		}
		if (n <= 499) {
			lineBuffer[n] = '\n';		
			lineBuffer[++n] = '\0';
		}
		//printf("stuck here?");
	} while ((lineBuffer[0] == '\n') && (c != EOF));

	if (c != EOF) {
		return 1;
	}
	else {
		return 0;
	}
}

int getToken(char *lineBuffer, char *tokenBuffer, int p) {

	int n = 0;
	
	while ((!isspace(lineBuffer[p])) && (n < 101)) {
		tokenBuffer[n] = lineBuffer[p];
		n++;
		p++;
	}
	tokenBuffer[n] = '\0';
	printf("%s ", tokenBuffer);

	if (lineBuffer[p] == '\n') {
		return 0;
	}
	p++;
	return p;
}

/*void clearArray(char *buffer) {

	for (int n = 0; n <= )

}*/