#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "Pass_Zero.h"

struct MNT_node {
    char label[TOKEN_LENGTH];
    struct MDT_node *begin;
    struct MNT_node *next;
};

struct MDT_node {
    char line[LINE_LENGTH];
    struct MDT_node *next;
};

struct arg_list_node {
	char arg[TOKEN_LENGTH];
	struct arg_list_node *next;
};

//int Pass_Zero(FILE *fp_in, FILE *fp_out, struct fileLines **linesTable_Head, int *error_count) {
int main() {

    char line[LINE_LENGTH];
    char lineOut[LINE_LENGTH];
	char token1[TOKEN_LENGTH];
	char input_file[] = argv[1];
	char output_file[] = "macros.mcr";
	char mode = 'N';
	int line_count = 0;
	int linem = 0;
	int linePos = 0;
	int sec = 0;
	FILE *fp_in = NULL;
	FILE *fp_out = NULL;
	struct MDT_node *MDT = NULL;
    struct MDT_node *tmpMDT = NULL;
    struct MNT_node *MNT = NULL;
	struct MNT_node *tmpMNT = NULL;

    if ((fp_in = fopen(input_file, "r")) == NULL) {
		printf("File not found.\n");
		return 0;
	}

	if ((fp_out = fopen(output_file, "w")) == NULL) {
        printf("File not found!\n");
        return 0;
    }

    while (getLine(fp, line)){
    	
    }

    deleteMDT(MDT);
    deleteMNT(MNT);
    if (fclose(fp) == 0) {
    	printf("\nFile closed.");
    }

    return 0;
}

struct MNT_node *searchMNT(struct MNT_node *table, char *token) {

    struct MNT_node* tmp = table;
    while ((tmp != NULL)) {
    	printf("Searching for %s. I'm here: %s\n",token,tmp->label);
        if (!strcmp(tmp->label, token)) {
        	return tmp;
        }
        tmp = tmp->next;
    }
    return NULL; // Label not defined(?)
}

struct MDT_node *addMDT(struct MDT_node **table, char *toAdd) {

	int i = 0;
	struct MDT_node* tmp = *table;
	struct MDT_node* new = (struct MDT_node*)malloc(sizeof(struct MDT_node));
	strcpy(new->line, toAdd);
	new->next = NULL;
	if (tmp == NULL) {
        *table = new;
        return *table;
    }
    while (tmp->next != NULL) {
        tmp = tmp->next;
	}
	//*tmp = &(*new);
    tmp->next = new;
    return tmp;
}

void addMNT(struct MNT_node **table, char *toAdd, struct MDT_node *first) {

	int i = 0;
	struct MNT_node* new = (struct MNT_node*)malloc(sizeof(struct MNT_node));
	for (i = 0; i < strlen(toAdd); i++) {
		if (toAdd[i] == ':') {
			new->label[i] = '\0';
		}
		else new->label[i] = toAdd[i];
	}
	new->begin = first;
	new->next = *table;
	*table = new;
}

void deleteMDT(struct MDT_node *table) {

    struct MDT_node* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}

void deleteMNT(struct MNT_node *table) {

    struct MNT_node* tmp;
    while(table != NULL) {
    	tmp = table;
    	table = table->next;
    	free(tmp);
    }
}

