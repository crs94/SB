#include<stdio.h>
#include<stdlib.h>
//#include<string.h>

int main(int argc, char *argv[]) {

	FILE *fp_in;
	FILE *fp_out;
	fpos_t fpos;
	char *map;
	int acc;	// Accumulator
	int pc;		// Program Counter
	int size = 0;
	int size_text = 0;
	int size_data = 0;
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
		size += chunks[j];
		addresses[j] = atoi(argv[j+3+n])
	}
	n = 0;
	j = 0;
	
	fp_in = fopen(argv[1]);
	fp_out = fopen(strcat(argv[1], ".im"));
	
	fscanf(fp_in, "%d", size_text);
	fscanf(fp_in, "%d", size_data);
	
	if((size_text + size_data) > size) {
		printf("OUT OF MEMORY - YOUR PROGRAM WILL NOT BE LOADED\n");
		return 0;		
	}
	
	map = (char*)malloc((size_text + size_data)*(sizeof(char)));
	fscanf(fp_in, "%s", map);
	memory = (int*)malloc((size_text + size_data)*sizeof(int));
	fgetpos(fp_in, &fpos);
	
	while(fscanf(fp_in, "%d", n) != EOF) {
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
	
	fsetpos(fp_in, &fpos);	
	j = 0;
	while(fscanf(fp_in, "%d", n) != EOF) {
		memory[j] = n;
		j++;
	}
	
	for(n = 0; n < size_text + size_data; n++) {
		if(map[n] == '1') {
			j = 0;
			size = chunks[j];
			while (memory[n] >= size) {
				j++;
				size += chunks[j];
			}
			memory[n] += (addresses[j] + chunks[j] - size);
		}
		fprintf(fp_out, "%d ", memory[n]);
	}
		
	return 0;
}
	
