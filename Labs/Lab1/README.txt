Alunos:
Antonio Aureliano de Anicésio Cardoso - 12/0049503
Camila Rosa da Silva - 12/0008921

Compilador utilizado: gcc versão 4.9.2
Sistema operacional: Debian 8

Compilar com: gcc ASSEMBLER.c -o assembler -lm

Modo de chamada: ./assembler -mode input_file output_file

Se -mode é "-p" o programa realiza apenas a passagem de pré-processamento
sobre input_file

Se -mode é "-m" o programa realiza a passagem de pré-processamento 
seguida pela passagem de expansão de macros e gera um arquivo intermediário
com a extensão ".pre" e em seguida o arquivo de saída output_file com as 
macros expandidas

Se -mode é "-o" o programa realiza as duas passagens anteriores seguidas 
do procedimento de montagem por passagem única. Três arquivos são gerados.
Dois intermediários com as extensões ".pre", ".mcr", e outpu_file. 
Este último contendo o código objeto resultante da montagem.

O programa irá exibir uma mensagem de erro caso o número de argumentos de 
chamada seja diferente de 4 ou se mode não corresponde a nenhum dos casos
anteriores.

Caso o programa encontre algum erro durante qualquer passagem, mensagens
de erro são exibidas no terminal.



