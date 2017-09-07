#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char str1;
    char filename[50] = "TestFiles/bin.asm";
    FILE *fp;

    if ((fp = fopen(filename, "r")) == NULL) {
        printf("404 Not Found!");
        exit(1);
    }

    if (fp == NULL) {
        printf("File couldn't be opened.");
    }

    while (fscanf(fp, "%c", &str1) == 1){
        printf("%c", str1);
    }
    printf("\nEOF");

    fclose(fp);

    return 0;
}
