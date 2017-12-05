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
		free(chunk_sizes);
		free(addresses);
        return 0;
	}
	if((fp_out = fopen(strcat(argv[1], ".im"), "w")) == NULL) {
		printf("Coudn't open output file!\n");
        free(chunk_sizes);
		free(addresses);
        return 0;
	}

	fscanf(fp_in, "%d", &size);

	if(size > size_available) {
		printf("OUT OF MEMORY - YOUR PROGRAM WILL NOT BE LOADED\n");
		free(chunk_sizes);
		free(addresses);
        return 0;
	}

	map = (char*)malloc(size*(sizeof(char)));
	fscanf(fp_in, "%s", map);
	memory = (int*)malloc(size*sizeof(int));
	fgetpos(fp_in, &fpos);

	while(fscanf(fp_in, "%d", &n) != EOF) {
		memory[j] = n;
		j++;
	}
	// Gerar arquivo de saída e depois simular? E remover arquivo em caso de erro?

	//if(j < size) erro

	j = 0;
	acc = 0;
	for(j = 0; j<size; j++) {
		switch(memory[j]) {
			case 1:
				j++;
                n = memory[j];
				acc += memory[n];
				break;
			case 2:
				j++;
				n = memory[j];
				acc -= memory[n];
				break;
			case 3:
				j++;
				n = memory[j];
				acc *= memory[n];
				break;
			case 4:
				j++;
				n = memory[j];
				acc /= memory[n];
				break;
			case 5:
				j++;
				n = memory[j];
				j = (n-1);
				break;
			case 6:
				j++;
				n = memory[j];
				if(acc < 0) j = (n-1);
				break;
			case 7:
				j++;
				n = memory[j];
				if(acc > 0) j = (n-1);
				break;
			case 8:
				j++;
				n = memory[j];
				if(acc == 0) j = (n-1);
				break;
			case 9:
			    n = memory[j+1];
			    i = memory[j+2];
				memory[n] = memory[i];
				j += 2;
				break;
			case 10:
				j++;
				n = memory[j];
				acc = memory[n];
				break;
			case 11: // what if acc > 255?
				j++;
				n = memory[j];
				memory[n] = acc;
				break;
			case 12:
				j++;
				n = memory[j];
				printf("Input: ");
				scanf("%s", str_aux);
				memory[n] = atoi(str_aux);
				break;
			case 13:
				j++;
				n = memory[j];
				printf("Output: %d\n", memory[n]);
				break;
			case 14: // Assembler still needs to detect if there is a STOP instruction
				j = size+1;
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

    /* Checks whether there is a chunk of memory that
     * can load the whole file
     */
    i = 0;
    for(n = 0; n < atoi(argv[2]); n++) {
        if(chunk_sizes[n] >= size) {
            for(i = 0; i < size; i++) {
                if(map[i] == '1') {
                    memory[i] += addresses[n];
                }
                fprintf(fp_out, "%d ", memory[i]);
            }
            break;
        }
    }

    if(i == 0) {  //No chunk was large enough for the whole file
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
    }

	free(chunk_sizes);
	free(addresses);
	free(map);
	free(memory);

	return 0;
}

