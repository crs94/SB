#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct list_node {
	int value;
	struct list_node *next;
};

struct table_node {
	char name[101];
	int value;
	struct table_node *next;
};

int search_def_table(struct table_node *table, char *name) {
	struct table_node *tmp = table;
	while(tmp != NULL) {
		if(!strcmp(tmp->name, name)) return tmp->value;
		tmp = tmp->next;
	}
	return 0;
}

int main(int argc, char *argv[]) {

	FILE *fp_in;
	FILE *fp_out;
	struct list_node *list = NULL;
	struct list_node *list_tail = NULL;
	struct list_node *new_list = NULL;
	struct list_node *tmp_list = NULL;
	struct table_node *use_table = NULL;
	struct table_node *def_table = NULL;
	struct table_node *new_tab = NULL;
	struct table_node *tmp_tab = NULL;
	int size_U = 0;
	int size_D = 0;
	int size = 0;
	int size_prev = 0;
	int i = 0;
	int n = 0;
	char **map;
	char header[3];
	char name[101]; 

	if ((fp_out = fopen(argv[1], "w")) == NULL) {
        printf("Coudn't open output file!\n");
        return 0;
    }
    
    map = (char**)malloc((argc-1)*sizeof(char*));

	for(i = 2; i < argc; i++) {
		size_prev += size;
		
		if ((fp_in = fopen(argv[i], "r")) == NULL) {
			printf("File %s not found.\n", argv[i]);
			return 0;
		}
		
		while(fscanf(fp_in, "%s", header) != EOF) {
			if(!strcmp(header, "N:")) {
				fscanf(fp_in, "%s", name);
				//testar se nome confere com argv[i]?
			}
			else if(!strcmp(header, "S:")) {
				fscanf(fp_in, "%d", &size);
			}
			else if(!strcmp(header, "U:")) {
				fscanf(fp_in, "%d", &size_U);
				for(n = 0; n < size_U; n++) {
					new_tab = (struct table_node*)malloc(sizeof(struct table_node));
					//checar erros em malloc?
					fscanf(fp_in, "%s %d", &new_tab->name, &new_tab->value);
					new_tab->value += size_prev;
					new_tab->next = NULL;
					if(use_table == NULL) {
						use_table = new_tab;
					}
					else {
						tmp_tab = use_table;
						while (tmp_tab->next != NULL) {
							tmp_tab = tmp_tab->next;
						}
						tmp_tab->next = new_tab;
					}
					tmp_tab = NULL;
				}
			}
			else if(!strcmp(header, "D:")) {
				fscanf(fp_in, "%d", &size_D);
				for(n = 0; n < size_D; n++) {
					new_tab = (struct table_node*)malloc(sizeof(struct table_node));
					//checar erros em malloc
					fscanf(fp_in, "%s %d", &new_tab->name, &new_tab->value);
					// Ao terminar de ler todos os arquivos, já teremos a 
					// tabela global de definições
					new_tab->value += size_prev;
					new_tab->next = NULL;
					if(def_table == NULL) {
						def_table = new_tab;
					}
					else {
						tmp_tab = def_table;
						while (tmp_tab->next != NULL) {
							tmp_tab = tmp_tab->next;
						}
						tmp_tab->next = new_tab;
					}
					tmp_tab = NULL;
				}
			}
			else if(!strcmp(header, "M:")) {
				map[i-2] = (char*)malloc(size*sizeof(char));
				fscanf(fp_in, "%s", map[i-2]);
			}
			else if(!strcmp(header, "T:")) {
				for(n = 0; n < size; n++) {
					new_list = (struct list_node*)malloc(sizeof(struct list_node));
					//checar erros em malloc
					fscanf(fp_in, "%d", &new_list->value);
					if(map[i-2][n] == '1') {
						new_list->value += size_prev;
					}
					new_list->next = NULL;
					if(list == NULL) {
						list = new_list;
						list_tail = new_list;
					}
					else {
						list_tail->next = new_list;
						list_tail = list_tail->next;
					}
				}
			}
		}
		//don't free map 
		fclose(fp_in);
	}
	
	tmp_tab = use_table;
	while(tmp_tab != NULL) {
		n = search_def_table(def_table, tmp_tab->name);
		
		tmp_list = list;
		for(i = 0; i < n; i++) {
			tmp_list = tmp_list->next;
		}
		tmp_list->value += n;
		tmp_tab = tmp_tab->next;
	}
	
	//escreve cabeçalho
	fprintf(fp_out, "%d", (size + size_prev));
	fputc('\n', fp_out);
	for(i = 0; i < argc-2; i++) {
		fprintf(fp_out, "%s", map[i]);
	}
	fputc('\n', fp_out);
	tmp_list = list;
	while(tmp_list != NULL) {
		fprintf(fp_out, "%d", tmp_list->value);
		fputc(' ', fp_out);
		tmp_list = tmp_list->next;
	}
	
	return 0;
}


