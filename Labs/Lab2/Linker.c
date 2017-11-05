#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct list_node {
	int value;
	struct list_node *next;
}

int main(int argc, char *argv[]) {

	FILE *fp_in;
	FILE *fp_out;
	struct list_node *list = NULL;
	struct list_node *new_node = NULL;
	struct list_node *tmp = NULL;
	int size_prev = 0;
	int size = 0;
	int i = 0;
	int headers;
	int replace;
	char name[101];
	char c;
	char *map;
	char str_aux[12]; //max size = 99GB?
	
	fopen(fp_out);
	
	// Assume-se que o usuário não fez a burrice de abrir e editar o código objeto XD
	// Considera-se que o arquivo tem três linhas de cabeçalho iniciadas pela letra H, sem espaço ou ":" depois
	// e uma única linha de código objeto com os opcodes e operandos
	while(i < argc) {
		headers = 0;
		size += size_prev;
		fopen();
		// apenas a partir do segundo arquivo será preciso corrigir os endereços
		// usar while(fgetc() para não limitar tamanho da linha? fazer duas passagens para determinar o tamanho de map
		while(GetLine(fp_in, line)) { //modificar GetLine para retornar o tamanho da linha lida?
			if(c == 'H') {
				headers++;
				if((headers == 1) && (i == 0)) {
					//pega nome do programa? ou usa argv[1]?
					n = 0;
					while((c = fgetc(fp_in) != 'H')) {
						name[n] = c;
					}
					name[n] = '\0';
				}
				else if(headers == 2) {
					//copia tamanho para uma string. GetToken? for?
					n = 0;
					while((c = fgetc(fp_in) != 'H')) {
						str_aux[n] = c;
					}
					str_aux[n] = '\0';
					size_prev = atoi(str_aux);
				}
				else if(headers == 3) {
					fgetpos(fp_in);
					n = 0;
					while((c = fgetc(fp_in) != 'T')) {
						n++;
					}
					map = (char*)malloc((n+1)*(sizeof(char)));
					fsetpos() // verificar manual
					for(j = 0; j < n; j++) {
						map[j] = fgetc(fp_in);
					}
					map[j] = '\0';
				}
				else {
					//erro
				}
			}
			else if(c == 'T') {
				n = 0;
				fscanf(fp_in, "%d", &replace);
				while(replace != EOF) { // Not sure if this works
					if(map[n] == '1') {
						replace += (size + 1);
					}
					n++
					new_node = (struct list_node*)malloc(sizeof(struct list_node));
					new_node->value = replace;
					new_node->next = NULL;
					tmp = list
					while(tmp->next != NULL) {
						tmp = tmp->next;
					}
					tmp->next = new_node;
					
					fscanf(fp_in, "%d", &replace);	
				}
			}
		}
	}
	//escreve cabeçalho
	//escreve lista no arquivo de saída
}
