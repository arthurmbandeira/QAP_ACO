#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <pthread.h>

#define ARQUIVO 1
#define ALPHA 2
#define BETA 3
#define RHO 4
#define Q 5
#define FORMIGAS 6
#define CICLOS 7

struct SMatriz{
    int tam;
    int *elementos;
    int *potencial;
    int *ref;
};
typedef struct SMatriz Matriz;

struct SFMatriz{
    int tam;
    float *elementos;
};
typedef struct SFMatriz FMatriz;

struct SPar{
    int *localidade;
    int *atividade;
};
typedef struct SPar Par;

struct SFormiga{
    int tam;
    int *linhaTabu;
    int *colunaTabu;
    int id;
    int qtdLinhasAtivas;
    int qtdColunasAtivas;
    int peso;
};
typedef struct SFormiga Formiga;

struct SEntrada{
    float alpha;
    float beta;
    float rho;
    float q;
    int qtdFormigas;
    int qtdCiclos;
};
typedef struct SEntrada Entrada;

Matriz *novaMatriz(int t){
    int i;
    Matriz *mat = malloc(sizeof(Matriz));
    mat->tam = t;
    mat->elementos = (int *)malloc(t*t*sizeof(int*));
    mat->potencial = malloc(sizeof(int)*t);
    mat->ref = malloc(sizeof(int)*t);
    for (i = 0; i < t; i++){
        mat->ref[i] = i;
    }
    return mat;
}

FMatriz *novaFMatriz(int t){
    FMatriz *fmat = malloc(sizeof(FMatriz));
    fmat->tam = t;
    fmat->elementos = (float *)malloc(t*t*sizeof(float*));
    return fmat;
}

Par *novoPar(int t){
    Par *par = malloc(sizeof(Par));
    par->localidade = malloc(sizeof(int)*t);
    par->atividade = malloc(sizeof(int)*t);
    return par;
}

Formiga *novaFormiga(int t){
    int i;
    Formiga *fmg = malloc(sizeof(Formiga));
    fmg->id = 0;
    fmg->tam = t;
    fmg->linhaTabu = malloc(t*sizeof(int));
    fmg->colunaTabu = malloc(t*sizeof(int));
    fmg->peso = 0;
    fmg->qtdLinhasAtivas = t;
    fmg->qtdColunasAtivas = t;
    for (i = 0; i < t; i++){
        fmg->linhaTabu[i] = 1;
        fmg->colunaTabu[i] = 1;
    }
    return fmg;
}

Entrada *novaEntrada(char *alpha, char *beta, char *rho, char *q, char *qtdFormigas, char *qtdCiclos){
    Entrada *in = malloc(sizeof(Entrada));
    in->alpha = atof(alpha);
    in->beta = atof(beta);
    in->rho = atof(rho);
    in->q = atof(q);
    in->qtdFormigas = atoi(qtdFormigas);
    in->qtdCiclos = atoi(qtdCiclos);
    return in;
}

int getElem(Matriz *mat, int lin, int col){
    return mat->elementos[lin*mat->tam + col];
}

void setElem(Matriz *mat, int lin, int col, int val){
    mat->elementos[lin*mat->tam + col] = val;
}

float getFloat(FMatriz *fmat, int lin, int col){
    return fmat->elementos[lin*fmat->tam + col];
}

void setFloat(FMatriz *fmat, int lin, int col, float val){
    fmat->elementos[lin*fmat->tam + col] = val;
}

void marcaAtividadeVisitada(Formiga *fmg, int col){
    fmg->colunaTabu[col] = 0;
    fmg->qtdColunasAtivas--;
}

void marcaLocalidadeVisitada(Formiga *fmg, int lin){
    fmg->linhaTabu[lin] = 0;
    fmg->qtdLinhasAtivas--;
}

int *somaPotencial(Matriz *mat){
    int i, j;
    int tamanho;
    int val;

    tamanho = mat->tam;
    val = 0;

    for (j = 0; j < tamanho; j++){
        for (i = 0; i < tamanho; i++){
            val +=  getElem(mat, i, j);
        }
        mat->potencial[j] = val;
        val = 0;
    }

    return mat->potencial;
}

int tamanhoMatriz(char *path){
    int tam;

    FILE *file;
    file = fopen(path, "r");
    fscanf(file, "%d", &tam);

    return tam;
}

void leMatriz(Matriz *matrizFluxo, Matriz *matrizDistancia, char *path){
    int i, j;
    int tam, valor;

    FILE *file;
    file = fopen(path, "r");
    fscanf(file, "%d", &tam);

    for (i = 0; i < tam; i++){
        for (j = 0; j < tam; j++){
            fscanf(file, "%d", &valor);
            setElem(matrizFluxo, i, j, valor);
        }
    }
    for (i = 0; i < tam; i++){
        for (j = 0; j < tam; j++){
            fscanf(file, "%d", &valor);
            setElem(matrizDistancia, i, j, valor);
        }
    }
    fclose(file);
}

void imprimeMatriz(Matriz *mat){
    int i, j;
    
    printf("   ");
    for (i = 0; i < mat->tam; i++){
        if (i > 10){
            printf(" %d ", i);
        }
        else{
            printf("  %d", i);
        }
    }
    printf("\n\n");
    for (i = 0; i < mat->tam; i++){
        if (i < 10){
            printf(" ");
        }
        printf("%d  ", i);
        for (j = 0; j < mat->tam; j++){
            if (getElem(mat, i, j) < 10){
                printf(" %d ", getElem(mat, i, j));
            }
            else{
                printf("%d ", getElem(mat, i, j));
            }
        }
        printf("\n");
    }
}

static int rand_int(int n) {
    int limit = RAND_MAX - RAND_MAX % n;
    int rnd;

    do {
        rnd = rand();
    } while (rnd >= limit);
    return rnd % n;
}

void shuffle(Matriz *array, int n) {
    int i, j, tmp;
    for (i = n - 1; i > 0; i--) {
        j = rand_int(i + 1);
        tmp = array->ref[j];
        array->ref[j] = array->ref[i];
        array->ref[i] = tmp;
    }
}

void imprimeFeromonio(FMatriz *fmat){
    int i, j;
    for (i = 0; i < fmat->tam; i++){
        printf("     %d   ", i);
    }
    printf("\n");
    for (i = 0; i < fmat->tam; i++){
        if (i < 10){
            printf(" ");
        }
        printf("%d ", i);
        for (j = 0; j < fmat->tam; j++){
            printf("%f " ,getFloat(fmat, i, j));
        }
        printf("\n");
    }
}

void criaFormigas(Formiga *vetorFormigas, int tam, Entrada *in){
    int i;
    for (i = 0; i < in->qtdFormigas; i++){
        vetorFormigas[i] = *novaFormiga(tam);
    }
}

void caminho(Formiga *fmg, Matriz *fluxo, Matriz *distancia, Entrada *in){
    int i, j;
    int valAtv = 0, valLoc = 0, total = 0;
    Par *tupla;
    tupla = novoPar(fluxo->tam);

    shuffle(fluxo, fluxo->tam);
    for (i = 0; i < fluxo->tam; i++){
        tupla->localidade[i] = i;
        tupla->atividade[i] = fluxo->ref[i];
    }
    for (i = 0; i < fluxo->tam; i++){
        printf("%d : %d\n", tupla->atividade[i], tupla->localidade[i]);
    }
    printf("\n");
    for (i = 0; i < fluxo->tam; i++){
        for (j = 0; j < fluxo->tam; j++){
            valAtv = getElem(fluxo, tupla->atividade[i], tupla->atividade[j]);
            valLoc = getElem(distancia, tupla->localidade[i], tupla->localidade[j]);
            printf("%d : %d  ", valLoc, valAtv);
            total += valAtv * valLoc;
        }
        printf("\n");
    }
    printf("%d\n", total);
}

int main(int argc, char *argv[]){
    int i, j, k;

    int tamanho;
    int nCiclos;
    int melhorIdFormiga;
    int melhorPeso = INT_MAX;

    int *somaFluxo;
    int *somaDistancia;

    Matriz *matrizFluxo;
    Matriz *matrizDistancia;
    Matriz *matrizPareada;

    FMatriz *matrizFeromonio;

    Formiga *vetorFormigas;

    Entrada *parametros;

    srand(time(NULL));

    tamanho = tamanhoMatriz(argv[ARQUIVO]);

    parametros = novaEntrada(argv[ALPHA], argv[BETA], argv[RHO], argv[Q], argv[FORMIGAS], argv[CICLOS]);

    matrizFluxo = novaMatriz(tamanho);
    matrizDistancia = novaMatriz(tamanho);

    leMatriz(matrizFluxo, matrizDistancia, argv[ARQUIVO]);


    vetorFormigas = malloc(parametros->qtdFormigas * sizeof(Formiga));
    criaFormigas(vetorFormigas, matrizFluxo->tam, parametros);

    for (k = 0; k < parametros->qtdFormigas; k++){
        caminho(&vetorFormigas[k], matrizFluxo, matrizDistancia, parametros);
    }


    /*imprimeMatriz(matrizFluxo);
    printf("\n");*/
    //imprimeMatriz(matrizDistancia);
    //printf("\n");
    

/*    matrizFeromonio = novaFMatriz(matrizFluxo->tam);
    inicializaFeromonio(matrizFeromonio);

    somaPotencial(matrizFluxo);
    somaPotencial(matrizDistancia);
    printf("\n");
    for (i = 0; i < tamanho; i++){
        printf("%d  ", matrizFluxo->potencial[matrizFluxo->ref[i]]);
    }
    printf("\n\n\n");
    matrizPareada = coupling(matrizDistancia->potencial, matrizFluxo->potencial, matrizFluxo->tam);
    imprimeMatriz(matrizPareada);
    printf("\n\n\n");
    
    vetorFormigas = malloc(parametros->qtdFormigas * sizeof(Formiga));

    criaFormigas(vetorFormigas, matrizFluxo->tam, parametros);

    nCiclos = parametros->qtdCiclos;

    melhorSolucao = malloc(tamanho*sizeof(ElemSolucao));

    inicializaFormigas(vetorFormigas, matrizFluxo, matrizDistancia, matrizPareada, matrizFeromonio, parametros);

    while (nCiclos){
        for (k = 0; k < parametros->qtdFormigas; k++){
            caminho(&vetorFormigas[k], matrizFluxo, matrizPareada, matrizFeromonio, parametros);
        }
        for (k = 0; k < parametros->qtdFormigas; k++){
            if (vetorFormigas[k].peso < melhorPeso){
                melhorPeso = vetorFormigas[k].peso;
                clonaVetor(melhorSolucao, vetorFormigas[k].solucao, tamanho);
                melhorIdFormiga = k;
            }
        }
        atualizaFeromonio(vetorFormigas, matrizFeromonio, parametros);
        for (k = 0; k < parametros->qtdFormigas; k++){
            vetorFormigas[k] = *novaFormiga(tamanho);
        }
            nCiclos--;
    }  */  
}