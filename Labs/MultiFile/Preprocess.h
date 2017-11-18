//#ifndef _PREPROCESS_H_
//#define _PREPROCESS_H_

extern struct equ_table_node {
	char label[TOKEN_LENGTH];
	char value[TOKEN_LENGTH];
	struct equ_table_node *next;
};

struct equ_table_node *SearchEQU(struct equ_table_node *table, char *token);

void AddEQU(struct equ_table_node **table, char *label, char *digit);

void DeleteEQU(struct equ_table_node *table); 

//#endif
