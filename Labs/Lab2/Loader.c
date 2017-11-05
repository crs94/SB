#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[]) {

	FILE *fp_in;
	FILE *fp_out;
	fpos_t pos;
	char c;
	char *map;
	char str_aux[12]; //max size = 99GB?
	char *name = argv[1];
	int acc;	// Accumulator
	int pc;		// Program Counter
	int size = 0;
	int headers;
	int *chunks;
	int *addresses;
	int *memory;
	int j;
	int n;

	
	n = atoi(argv[2]);
	chunks = (int*)malloc(n*sizeof(int));
	addresses = (int*)malloc(n*sizeof(int));
	for(j = 0; j<n; j++) {
		chunks[j] = atoi(argv[j+3])
		addresses[j] = atoi(argv[j+3+n])
	}
	n = 0;
	j = 0;
	
	fopen(fp_out);
	
	while((c = fgetc(fp_in)) != EOF) {
		if(c == 'H') headers++;
		if(c == 'T') headers++;
		if(headers == 2) {
			str_aux[n] = c;
			n++;
		}
		else if(headers == 3) {
			str_aux[n] = '\0';
			size = atoi(str_aux);
			headers++;
			n = 0;
			fgetpos(fp_in, &pos);
		}
		else if(headers == 4) {
			n++;
		}
		else if(headers == 5) {
			map = (char*)malloc((n+1)*(sizeof(char)));
			fsetpos(fp_in, &pos);
			headers++;
		}
		else if(headers == 6) {
			if(j < n) {
				map[j] = c;
				j++;
			}
		}
		else if(headers == 7) {
			map[j] = '\0';
			headers++;
			n = 0;
			fgetpos(fp_in, &pos); //Keeps position for after simulation?
			break;
		}
	}
	
	memory = (int*)malloc(size*sizeof(int));
	
	while(fscanf(fp_in, "%s", str_aux) != EOF) {
		n = atoi(str_aux);
		memory[j] = n;
		j++;
	}
	// Gerar arquivo de saída e depois simular? E remover arquivo em caso de erro?
	
	//if(j < size) erro
	
	j = 0;
	printf("Simulating code\n");
	for(j = 0; j<n; j++) {
		switch(memory[j]) {
			case 1:
				j++;
				acc += memory[memory[j]]; // Does it work?
				break;
			case 2:
				j++;
				acc -= memory[memory[j]];
				break;
			case 3:
				j++;
				acc *= memory[memory[j]];
				break;
			case 4:
				j++;
				acc /= memory[memory[j]];
				break;
			case 5:
				j++;
				pc = memory[j];
				break;
			case 6:
				j++;
				if(acc < 0) pc = memory[j];
				break;
			case 7:
				j++;
				if(acc > 0) pc = memory[j];
				break;
			case 8:
				j++;
				if(acc == 0) pc = memory[j];
				break;
			case 9:
				memory[memory[j+2]] = memory[memory[j+1]];
				j += 2;
				break;
			case 10:
				j++;
				acc = memory[memory[j]];
				break;
			case 11: // what if acc > 255?
				j++;
				memory[memory[j]] = acc;
				break;
			case 12:
				j++;
				printf("Input: ");
				scanf("%s", str_aux);
				memory[j] = atoi(str_aux);
				break;
			case 13:
				j++;
				printf("Output: %d\n", memory[j]);
				break;
			case 14: // Assembler still needs to detect if there is a STOP instruction
				j = n;
				break;
			default:
		}
	}
	printf("Finished simulation\n");
	
	fsetpos(fp_in, &pos);
	//confere se programa cabe na memória
	//novo while(fscanf), agora ajustando endereços em chunks
	
	
}
	
