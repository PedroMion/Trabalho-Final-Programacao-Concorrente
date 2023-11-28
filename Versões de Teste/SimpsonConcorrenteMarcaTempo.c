#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

// Definindo variáveis globais que serão utilizadas no código
int N_THREADS, fatorMultiplicador;
double resultado, limiteSuperior, limiteInferior;
pthread_mutex_t mutex;


// Struct que guarda as informações de integração a, b, h e quantidade de iterações.
// Será passada como argumento para a função que executa a tarefa das threads
typedef struct{
   double limiteInferior;
   double limiteSuperior;
   int quantidadeIteracoes;
   double tamanhoIntervalo;
} infosIntegracao;


// Função matemática que será integrada
double funcao(double x) {
   return x;
}


// Função que realiza a tarefa e integra no intervalo
void * regraDeSimpsonConcorrente(void *arg) {
    // Recuperando as informações de integração recebidas como argumento
    infosIntegracao* infos = (infosIntegracao *) arg;
    // Criando variáveis locais que serão utilizadas por essa thread
    double soma1 = 0, soma2 = 0, resultadoLocal;

    // Calculando o tamanho do intervalo como sendo (b - a) / n
    infos->tamanhoIntervalo = (infos->limiteSuperior-infos->limiteInferior) / infos->quantidadeIteracoes;

    // Criando vetor com tamanho para os valores da função de acordo com a quantidade de iterações,
    // que será usado no código
    double* valores = (double *) malloc(sizeof(double) * (infos->quantidadeIteracoes+1));

    // Calculando o valor da função para cada valor de x pertencente ao intervalo, de acordo com o número de iterações
    for(int i = 0; i < (infos->quantidadeIteracoes+1); i++) {
        valores[i] = funcao(infos->limiteInferior + i*infos->tamanhoIntervalo);
    }

    // Somatório "par", uma parcela do código, realizado da forma que o método dita
    for(int i = 1; i < ceil(infos->quantidadeIteracoes / 2); i++) {
        soma1 += valores[2 * i];
    }
    soma1 *= 2;

    // Somatório "ímpar", uma parcela do código, realizado da forma que o método dita
    for(int i = 1; i < ceil((infos->quantidadeIteracoes / 2) + 1); i++) {
        soma2 += valores[(2 * i) - 1];
    }
    soma2 *= 4;

    // Usa a fórmula da regra de Simpson para estimar a integral neste intervalo
    resultadoLocal = ((infos->tamanhoIntervalo/3) * (funcao(infos->limiteInferior) + funcao(infos->limiteSuperior) + soma1 + soma2));


    // Adiciona resultado local no resultado final, utilizando exclusão mútua
    pthread_mutex_lock(&mutex);
    resultado += resultadoLocal;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    pthread_t *tid;
    infosIntegracao* infos;
    double diferenca;
    double inicio, fim, delta;


    // Recuperando as variáveis da linha de comando
    if(argc<5) {
        printf("Digite: %s <limiteInferior> <limiteSuperior> <Numero de threads> <Fator multiplicador>\n", argv[0]);
        return 1;
    }

    limiteInferior = atof(argv[1]);
    limiteSuperior = atof(argv[2]);
    N_THREADS = atoi(argv[3]);
    fatorMultiplicador = atoi(argv[4]);

    // Marca início do tempo de execução
    GET_TIME(inicio);

    // Inicializando os vetores tid e infos e a variável mutex
    tid = (pthread_t*) malloc(sizeof(pthread_t)*N_THREADS);
    if(tid==NULL) {puts("ERRO--malloc"); return 2;}

    infos = (infosIntegracao*) malloc(sizeof(infosIntegracao)*N_THREADS);
    if(infos==NULL) {puts("ERRO--malloc"); return 2;}

    pthread_mutex_init(&mutex, NULL);


    // Seta resultado para 0 e calcula a "fatia" que cada thread vai utilizar através da conta (b-a)/nThreads
    resultado = 0;
    diferenca = (limiteSuperior - limiteInferior) / N_THREADS;

    // For que cria e dispara as threads
    for(int i = 0; i < N_THREADS; i++) {
        // Se é a primeira thread, recebe o limite inferior original, se não, arredondamos para evitar erros de ponto flutuante
        // Essa mudança acaba desbalanceando levemente a carga de cada thread, porém, é um preço baixo para obter um resultado melhor
        (infos + i)->limiteInferior = i == 0 ? limiteInferior : ceil(limiteInferior + (i * diferenca));
        // Se é a última thread, recebe o limite superior original, se não, arredondamos para ficar condizente com o limite inferior
        (infos + i)->limiteSuperior = i == (N_THREADS - 1) ? limiteSuperior : ceil(limiteInferior + ((i+1) * diferenca));
        // Quantidade de iteracoes recebe o fator multiplicador multiplicado pela diferença a - b arredondada, para evitar erros de ponto flutuante
        (infos + i)->quantidadeIteracoes = fatorMultiplicador * (ceil((infos + i)->limiteSuperior - (infos + i)->limiteInferior));

        if(pthread_create(tid+i, NULL, regraDeSimpsonConcorrente, (void*) (infos + i))){
            puts("ERRO--pthread_create"); return 3;
         }
    }

    // Aguarda as threads terminarem
    for(int i = 0; i < N_THREADS; i++) {
        pthread_join(*(tid+i), NULL);
    }

    // Pega final do tempo de execução
    GET_TIME(fim);
    delta = fim - inicio;

    pthread_mutex_destroy(&mutex);

    free(tid);

    printf("Tempo de execução para %d threads: %lfs\n", N_THREADS, delta);
    printf("Valor aproximado da integral definida: %lf\n", resultado);

   return 0;
}