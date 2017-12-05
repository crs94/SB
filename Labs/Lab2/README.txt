Alunos:
Antonio Aureliano de Anicésio Cardoso - 12/0049503
Camila Rosa da Silva - 12/0008921

Compilador utilizado: gcc versão 4.8.5
Sistema operacional: SUSE

Compilar o
	Montador com:	gcc ASSEMBLER.c -o assembler
	Ligador com:	gcc Linker.c -o linker
	Carregador com:	gcc Loader.c -o loader

Modo de chamada do
	Montador:	./assembler mod1.asm mod2.asm mod3.asm
	Ligador:	./linker output mod1.o mod2.o mod3.o
	Carregador:	./loader output numero_de_chunks memoria_disponivel_em_cada_chunk endereco_inicial_de_cada_chunk

O montador consegue compilar ate tres arquivos de uma so vez. Mensagens 
de erro serao exibidas caso o número de argumentos de chamada seja maior
do que quatro e menor do que dois. Caso o montador encontre algum erro 
durante qualquer passagem, mensagens de erro são exibidas no terminal e
os arquivos de saida nao serao gerados.

O ligador tambem consegue ligar ate tres modulos em um unico arquivo. O
primeiro argumento que precisa ser passado apos a chamada ao programa e
o nome do arquivo de saida e, em seguida, sao passados os nomes dos
arquivos objetos que serao ligados.

O carregador recebe como primeiro argumento o nome do arquivo de saida
do ligador. Em seguida, deve ser passado a quantidade de chunks de memoria
disponiveis para o carregamento do programa. Este valor deve ser seguido
por uma sequencia de valores que indicam a quantidade de memoria disponivel
em cada um dos chunks disponiveis. Por ultimo, uma sequencia de valores que 
indicam os enderecos onde cada um dos chunks de memoria sao iniciados deve 
ser informada ao programa.



