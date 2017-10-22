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
#include <utils.h>
//#include "Preprocess.h"
//#include "Pass_Zero.h"
//#include "One_Pass.h"

int main(int argc, char *argv[]) {
	
	char usage[] = "Usage: ./assembler -mode myprogram.asm myprogram.extension\nif mode = \"-p\" preprocessing only. extension = \".pre\"\nif mode = \"-m\" expands macros. extension = \".mcr\"\nif mode = \"-o\" performs complete assembly. extension = \".o\"\n";
	
	char input_file[] = argv[2];
	char output_file[] = argv[3];
	char pass_pre_out[] = argv[3];
	char pass_zero_out[] = argv[3];
	FILE *fp_in = NULL;
	FILE *fp_out = NULL;
	
	if(argc != 4) {
		//checar extensão válida de argv[3]? escrever extensão correta mesmo assim?
		printf("%s\n",usage);
		return 0;
	}

    if ((fp_in = fopen(input_file, "r")) == NULL) {
		printf("Input file not found.\n");
		return 0;
	} 

    if(!strcmp(argv[1], "-p")) {
    
    	if ((fp_out = fopen(output_file, "w")) == NULL) {
			printf("Could not open output file.\n");
			return 0;
		}
		
    	Preprocess(fp_in, fp_out);
    	
    	fclose(fp_in);
    	fclose(fp_out);
    	
    	return 1;
    }
    else {
		if(!strcmp(argv[1], "-m")) {
		
			strcat(pass_pre_out, ".pre");
			
			if ((fp_out = fopen(pass_pre_out, "w")) == NULL) {
				printf("Could not open intermediate file 1.\n");
				return 0;
			}
			
			Preprocess(fp_in, pass_pre_out);
			
			fclose(fp_in);
    		fclose(fp_out);
    		
    		if ((fp_in = fopen(pass_pre_out, "r")) == NULL) {
				printf("Could not open intermediate file 2.\n");
				return 0;
			}
			
			if ((fp_out = fopen(output_file, "w")) == NULL) {
				printf("Could not open output file.\n");
				return 0;
			}
    		
			Pass_zero(fp_in, fp_out);
			
			fclose(fp_in);
			fclose(fp_out);
			//remove(pass_pre_out);
			
			return 1;
		}
		else {
			if (!strcmp(argv[1], "-o")) {
			
				strcat(pass_pre_out, ".pre");
			
				if ((fp_out = fopen(pass_pre_out, "w")) == NULL) {
					printf("Could not open intermediate file 1.\n");
					return 0;
				}
			
				Preprocess(fp_in, pass_pre_out);
			
				fclose(fp_in);
				fclose(fp_out);
				
				if ((fp_in = fopen(pass_pre_out, "r")) == NULL) {
					printf("Could not open intermediate file 2.\n");
					return 0;
				}
			
				strcat(pass_zero_out, ".mcr");
			
				if ((fp_out = fopen(pass_zero_out, "w")) == NULL) {
					printf("Could not open intermediate file 3.\n");
					return 0;
				}
				
				Pass_zero(fp_in, fp_out);
			
				fclose(fp_in);
				fclose(fp_out);
				
				if ((fp_in = fopen(pass_zero_out, "r")) == NULL) {
					printf("Could not open intermediate file 4.\n");
					return 0;
				}
				
				if ((fp_out = fopen(output_file, "w")) == NULL) {
					printf("Could not open output file.\n");
					return 0;
				}
				
				One_Pass(fp_in, fp_out);
			
				fclose(fp_in);
				fclose(fp_out);
				//remove(pass_pre_out);
				//remove(pass_zero_out);
			
				return 1;
			}
			else {
				printf("%s\n",usage);
				return 0;
			}
		}
	}

    return 0;
}

