#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	
	char usage[] = "Usage: ./assembler -mode myprogram.asm myprogram.extension\nif mode = \"-p\" preprocessing only. extension = \".pre\"\nif mode = \"-m\" expands macros. extension = \".mcr\"\nif mode = \"-o\" performs complete assembly. extension = \".o\"\n"
	
	FILE *fin, *fout;
	//Declarar tabela de erros e de linhas
	
	if((argc != 4) || ((strcmp(argv[1], "-p")) && (strcmp(argv[1], "-m")) && (strcmp(argv[1], "-o")))) {
		//checar extensão válida de argv[3]? escrever extensão correta mesmo assim?
		printf("%s\n",usage);
		return 0;
	}

    if(!(fin = OpenFile(argv[2]))) return 0;
    fout = 
    
    if(!(preprocess(fp, error_tab))) {
		//retorna erros
	}
	else {
		fecha arquivo de saída anterior
		fecha arquivo fonte
		if((!strcmp(argv[1], "-o")) || (!strcmp(argv[1], "-o"))) {
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

    return 0;
}

