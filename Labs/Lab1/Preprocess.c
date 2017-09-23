#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct equTab {
	char label[101];
	int value;
	struct equTab *next;
};

void getLine(FILE *fp, char *lineBuffer);
void getToken(char *lineBuffer, char *tokenBuffer);

int main() {

	char line[501], token[101];
	char filename[] = "TestFiles/bin.asm";
	FILE *fp;

	if ((fp = fopen(filename, "r")) == NULL) {
        printf("404 Not Found!");
        exit(1);
    }

    getLine(fp, line);

    if (fclose(fp) == 0) {
    	printf("\nFile closed.");
    }
    getchar();

    return 0;
}

void getLine(FILE *fp, char *lineBuffer) {

	char c;
	int n = 0;

	while (((c = fgetc(fp)) != '\n') && (n <= 500) && (c != EOF)) {
		if (n == 0) {
			// Ignores initial blanks
			while ((c == ' ') || (c == '\t')) {
				c = fgetc(fp);
			}
		}
		if ((lineBuffer[n - 1] = ' ') && (n != 0)) {
			// Ignores blanks in line
			while ((c == ' ') || (c == '\t')) {
				c = fgetc(fp);
			}
		}
		if (islower(c)) {
			c = toupper(c);
		}
		lineBuffer[n] = c;
		printf("%c", lineBuffer[n]);
		n++;
	}

	if (n <= 500) {
		lineBuffer[++n] = '\0';		
	}

}

void getToken(char *lineBuffer, char *tokenBuffer) {



}