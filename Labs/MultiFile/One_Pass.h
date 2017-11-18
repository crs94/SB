//#ifndef _ONE_PASS_H_
//#define _ONE_PASS_H_

extern struct op_table_node {			// Struct to store a node of the operations table
	char *name;							// Name of the operation
	int opcode;							// Opcode
	int operands;						// Number of operands
};

extern struct replace_list_node {		//
	int *replace;						//
	int offset;							//
	struct replace_list_node *next;		//
};

extern struct sym_table_node {			// Struct to store a node of the symbol table
	char label[TOKEN_LENGTH];			// Name of the label
	int address;						// Value of Location Counter when label was defined. -1 until defined
	int defined;						// 1 if label definition was found. 0 otherwise
	struct replace_list_node *list; 	// This will keep offset position of undefined symbol
	int vector; 						// Size of vector defined with SPACE directive. Zero until defined
	struct sym_table_node *next;		// Pointer to the next node in the symbol table
};

extern struct output_line {				//
	int opcode;							//
	int op[2];							//
	struct output_line *next;			//
};

struct sym_table_node *SearchSym(struct sym_table_node *table, char *token);

void AddSym(struct sym_table_node **table, char *name, int address, int defined);

void DeleteSymTable(struct sym_table_node *table);

struct op_table_node *SearchOp(struct op_table_node *table, char *token);

void AddReplace(struct replace_list_node **node, int *n);

void ReplaceList(struct sym_table_node *node);

void AddLine(struct output_line *line, struct output_line **head);

void DeleteOutputLines(struct output_line *first);

void AdjustAdresses(struct sym_table_node *table, int lc_text);

void FinalErrorCheck(struct sym_table_node *symtab, struct output_line *text, struct output_line *data, int secText);

void WriteObjectFile(FILE *fp, struct output_line *first);

//#endif
