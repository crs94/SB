/***********************************************************************
*
* FILE NAME: Assembler.c
*
*
* PURPOSE:
*
*
* FILE REFERENCES:
*
*
* EXTERNAL VARIABLES:
*
* Name				Type		IO		Description
* -------------		-------		--		-----------------------------
*
*
* EXTERNAL REFERENCES:
*
* Name					Description
* -------------			-----------------------------
*
*
* ABNORMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES:
*	none
*
*
* ASSUMPTIONS, CONSTRAINTS, RESTRICTIONS: none
*
*
* NOTES:
*
*
***********************************************************************/

#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	
	char usage[] = "Usage: ./assembler -mode myprogram.asm myprogram.extension\nif mode = \"-p\" preprocessing only. extension = \".pre\"\nif mode = \"-m\" expands macros. extension = \".mcr\"\nif mode = \"-o\" performs complete assembly. extension = \".o\"\n"
	
	char input_file[] = "TestFiles/SB_test_getline.asm"; // To be replaced by argv[2]
	char output_file[] = "TestFiles/output.txt";
	FILE *fin = NULL;
	FILE *fout = NULL;
	//Declarar tabela de erros e de linhas
	
	if((argc != 4) || ((strcmp(argv[1], "-p")) && (strcmp(argv[1], "-m")) && (strcmp(argv[1], "-o")))) {
		//checar extensão válida de argv[3]? escrever extensão correta mesmo assim?
		printf("%s\n",usage);
		return 0;
	}

    if ((fp_in = fopen(input_file, "r")) == NULL) {
		printf("File not found.\n");
		return 0;
	} 

	if ((fp_out = fopen(output_file, "w")) == NULL) {
        printf("File not found!\n");
        return 0;
    }
    
    if(preprocess(fp_in, error_tab)) {
		if((!strcmp(argv[1], "-m")) || (!strcmp(argv[1], "-o"))) {
			abre arquivo anterior para leitura
			abre novo arquivo de saída
			if(!(Pass_zero(arquivo para leitura, error_tab, arquivo para escrita)))
				retorna erro
			fecha arquivos
			remove arquivo da passagem de pré-processamento?
			if (!strcmp(argv[1], "-o")) {
				Declara tabelas de saída outtable_inst e outtable_data
				if(!(one_pass(arquivo_passagem0, outtable_inst, outtable_data, error_tab, )))
					retorna erro
				remove arquivo_passagem0
				Processa erros
				escreve arquivo objeto
				retorna
			}
			escreve mensagens de erro
		}
	}
	else {
	
	}

    return 0;
}

