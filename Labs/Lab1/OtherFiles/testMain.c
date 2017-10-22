#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Util.h"
#include "Preprocess.h"
#include "Pass_Zero.h"

int main() {

	char input_file[] = "TestFiles/fatorial.asm";
	FILE *fp_in = NULL;
    struct fileLines *linesTable_Head = NULL;

    linesTable_Head = thePre(input_file, &linesTable_Head);
    if (linesTable_Head == NULL) printf("FUCK!");
    theZero("TestFiles/outpre.txt", &linesTable_Head);

    deleteLines(linesTable_Head);

	return 0;
}