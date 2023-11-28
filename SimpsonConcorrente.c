#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int N_THREADS, fatorMultiplicador;
double resultado, limiteSuperior, limiteInferior;
pthread_mutex_t mutex;


typedef struct{
   double limiteInferior;
   double limiteSuperior;
   int quantidadeIteracoes;
   double tamanhoIntervalo;
} infosIntegracao;


double funcao(double x) {
   return x;
}


void * regraDeSimpsonConcorrente(void *arg) {
    infosIntegracao* infos = (infosIntegracao *) arg;
    double soma1 = 0, soma2 = 0, resultadoLocal;

    infos->tamanhoIntervalo = (infos->limiteSuperior-infos->limiteInferior) / infos->quantidadeIteracoes;
    double* valores = (double *) malloc(sizeof(double) * (infos->quantidadeIteracoes+1));

    for(int i = 0; i < (infos->quantidadeIteracoes+1); i++) {
        valores[i] = funcao(infos->limiteInferior + i*infos->tamanhoIntervalo);
    }

    for(int i = 1; i < ceil(infos->quantidadeIteracoes / 2); i++) {
        soma1 += valores[2 * i];
    }
    soma1 *= 2;

    for(int i = 1; i < ceil((infos->quantidadeIteracoes / 2) + 1); i++) {
        soma2 += valores[(2 * i) - 1];
    }
    soma2 *= 4;

    resultadoLocal = ((infos->tamanhoIntervalo/3) * (funcao(infos->limiteInferior) + funcao(infos->limiteSuperior) + soma1 + soma2));

    pthread_mutex_lock(&mutex);
    resultado += resultadoLocal;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    pthread_t *tid;
    infosIntegracao* infos;
    double diferenca;



    if(argc<5) {
        printf("Digite: %s <limiteInferior> <limiteSuperior> <Numero de threads> <Fator multiplicador>\n", argv[0]);
        return 1;
    }

    limiteInferior = atof(argv[1]);
    limiteSuperior = atof(argv[2]);
    N_THREADS = atoi(argv[3]);
    fatorMultiplicador = atoi(argv[4]);



    tid = (pthread_t*) malloc(sizeof(pthread_t)*N_THREADS);
    if(tid==NULL) {puts("ERRO--malloc"); return 2;}

    infos = (infosIntegracao*) malloc(sizeof(infosIntegracao)*N_THREADS);
    if(infos==NULL) {puts("ERRO--malloc"); return 2;}

    pthread_mutex_init(&mutex, NULL);



    resultado = 0;
    diferenca = (limiteSuperior - limiteInferior) / N_THREADS;
    for(int i = 0; i < N_THREADS; i++) {
        (infos + i)->limiteInferior = i == 0 ? limiteInferior : ceil(limiteInferior + (i * diferenca));
        (infos + i)->limiteSuperior = i == (N_THREADS - 1) ? limiteSuperior : ceil(limiteInferior + ((i+1) * diferenca));
        (infos + i)->quantidadeIteracoes = fatorMultiplicador * (ceil((infos + i)->limiteSuperior - (infos + i)->limiteInferior));

        if(pthread_create(tid+i, NULL, regraDeSimpsonConcorrente, (void*) (infos + i))){
            puts("ERRO--pthread_create"); return 3;
         }
    }

    for(int i = 0; i < N_THREADS; i++) {
        pthread_join(*(tid+i), NULL);
    }


    pthread_mutex_destroy(&mutex);

    free(tid);

    printf("Valor aproximado da integral definida: %lf\n", resultado);

   return 0;
}