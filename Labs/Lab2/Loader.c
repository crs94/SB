#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include<string.h>

int main(int argc, char *argv[]) {

	FILE *fp_in;
	FILE *fp_out;
	fpos_t fpos;
	char *map;
	char str_aux[12];
	int acc;	// Accumulator
	int pc;		// Program Counter
	int size = 0;
	int size_available = 0;
	int *chunk_sizes;
	int *addresses;
	int *memory;
	int j;
	int n;
	int aux;
	int i;

	
	n = atoi(argv[2]);
	chunk_sizes = (int*)malloc(n*sizeof(int));
	addresses = (int*)malloc(n*sizeof(int));
	for(j = 0; j<n; j++) {
		chunk_sizes[j] = atoi(argv[j+3]);
		size_available += chunk_sizes[j];
		addresses[j] = atoi(argv[j+3+n]);
	}
	n = 0;
	j = 0;
	
	if((fp_in = fopen(argv[1], "r")) == NULL) {
		printf("Coudn't open input file!\n");
        return 0;
	}
	if((fp_out = fopen(strcat(argv[1], ".im"), "w")) == NULL) {
		printf("Coudn't open output file!\n");
        return 0;
	}
	
	fscanf(fp_in, "%d", &size);
	printf("got size\n");
	
	if(size > size_available) {
		printf("OUT OF MEMORY - YOUR PROGRAM WILL NOT BE LOADED\n");
		return 0;		
	}
	
	map = (char*)malloc(size*(sizeof(char)));
	printf("got map\n");
	fscanf(fp_in, "%s", map);
	printf("got map\n");
	memory = (int*)malloc(size*sizeof(int));
	fgetpos(fp_in, &fpos);
	
	printf("allocated\n");
	
	while(fscanf(fp_in, "%d", &n) != EOF) {
		memory[j] = n;
		j++;
	}
	printf("got memory\n");
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
				break;
		}
	}
	printf("Finished simulation\n");
	
	fsetpos(fp_in, &fpos);	
	j = 0;
	while(fscanf(fp_in, "%d", &n) != EOF) {
		memory[j] = n;
		j++;
	}

	for(n = 0; n < size; n++) {
		if(map[n] == '1') {
			aux = chunk_sizes[0];
			for(i = 0; memory[n] > aux; i++) { 
				aux += chunk_sizes[i+1];
			}
			memory[n] += addresses[i];
		}
		fprintf(fp_out, "%d ", memory[n]);
	}
		
	return 0;
}
	
