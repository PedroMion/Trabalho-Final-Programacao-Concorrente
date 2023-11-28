#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "timer.h"

// Definindo variáveis globais que serão utilizadas no código
double limiteInferior, limiteSuperior;
int quantidadeIteracoes;


// Struct que guarda as informações de integração a, b, h e quantidade de iterações.
// Será passada como argumento para a função que executa a regra, para ficar mais legível
typedef struct{
   double limiteInferior;
   double limiteSuperior;
   double tamanhoIntervalo;
   int quantidadeIteracoes;
} infosIntegracao;


// Função matemática que será integrada
double funcao(double x) {
   return x;
}


// Função que realiza a aproximação da integral a partir da regra de Simpson
double regraDeSimpsonSequencial(infosIntegracao infos) {
    // Criando variáveis locais que serão utilizadas por essa thread
    double soma1 = 0, soma2 = 0, resultado;

    // Calculando o tamanho do intervalo como sendo (b - a) / n
    infos.tamanhoIntervalo = (infos.limiteSuperior-infos.limiteInferior) / infos.quantidadeIteracoes;

    // Criando vetor com tamanho para os valores da função de acordo com a quantidade de iterações,
    // que será usado no código
    double* valores = (double *) malloc(sizeof(double) * (infos.quantidadeIteracoes+1));

    // Calculando o valor da função para cada valor de x pertencente ao intervalo, de acordo com o número de iterações
    for(int i = 0; i < (infos.quantidadeIteracoes+1); i++) {
        valores[i] = funcao(infos.limiteInferior + i*infos.tamanhoIntervalo);
    }

    // Somatório "par", uma parcela do código, realizado da forma que o método dita
    for(int i = 1; i < ceil(infos.quantidadeIteracoes / 2); i++) {
        soma1 += valores[2 * i];
    }
    soma1 *= 2;

    // Somatório "ímpar", uma parcela do código, realizado da forma que o método dita
    for(int i = 1; i < ceil((infos.quantidadeIteracoes / 2) + 1); i++) {
        soma2 += valores[(2 * i) - 1];
    }
    soma2 *= 4;

    // Usa a fórmula da regra de Simpson para estimar a integral neste intervalo
    resultado = ((infos.tamanhoIntervalo/3) * (funcao(infos.limiteInferior) + funcao(infos.limiteSuperior) + soma1 + soma2));

    return resultado;
}


int main(int argc, char* argv[]) {
    infosIntegracao infos;
    double resultado;
    double inicio, fim, delta;

    // Recuperando as variáveis da linha de comando
    if(argc<4) {
        printf("Digite: %s <limiteInferior> <limiteSuperior> <quantidadeIteracoes>\n", argv[0]);
        return 1;
    }

    limiteInferior = atof(argv[1]);
    limiteSuperior = atof(argv[2]);
    quantidadeIteracoes = atoi(argv[3]);

    // Setando os valores no struct
    infos.limiteInferior = limiteInferior;
    infos.limiteSuperior = limiteSuperior;
    infos.quantidadeIteracoes = quantidadeIteracoes;

    GET_TIME(inicio);
    // Calcula a integral aproximada
    resultado = regraDeSimpsonSequencial(infos);
    GET_TIME(fim);
    
    delta = fim - inicio;

    printf("Tempo de execução da versão sequencial: %lfs\n", delta);
    printf("Valor aproximado da integral definida: %lf\n", resultado);

   return 0;
}