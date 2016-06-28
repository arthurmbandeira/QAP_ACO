#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
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

struct SElemSolucao{
    int atividade;
    int localidade;
    int valor;
};
typedef struct SElemSolucao ElemSolucao;

struct SFormiga{
    int tam;
    int *linhaTabu;
    int *colunaTabu;
    int id;
    int qtdLinhasAtivas;
    int qtdColunasAtivas;
    int peso;
    ElemSolucao *solucao;
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

Formiga *novaFormiga(int t){
    int i;
    Formiga *fmg = malloc(sizeof(Formiga));
    fmg->id = 0;
    fmg->tam = t;
    fmg->linhaTabu = malloc(t*sizeof(int));
    fmg->colunaTabu = malloc(t*sizeof(int));
    fmg->solucao = malloc(t*sizeof(ElemSolucao));
    fmg->peso = 0;
    fmg->qtdLinhasAtivas = t;
    fmg->qtdColunasAtivas = t;
    for (i = 0; i < t; i++){
        fmg->linhaTabu[i] = 1;
        fmg->colunaTabu[i] = 1;
    }
    return fmg;
}

ElemSolucao *novoElemSolucao(int atividade, int localidade, int valor){
    ElemSolucao *elem = malloc(sizeof(ElemSolucao));
    elem->atividade = atividade;
    elem->localidade = localidade;
    elem->valor = valor;
    return elem;
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

Matriz *coupling(int *vet1, int *vet2, int tam){
    int i, j, val;
    Matriz *A;
    A = novaMatriz(tam);
    for (i = 0; i < tam; i++){
        for (j = 0; j < tam; j++){
            val = vet1[i] * vet2[j];
            setElem(A, i, j, val);
        }
    }
    return A;
}

float getDesejo(Matriz *mat, int i, int j){
    return 1 / (float)getElem(mat, i, j);
}

void addPar(Formiga *fmg, ElemSolucao *elem){
    fmg->solucao[fmg->id++] = *elem;
    fmg->peso += elem->valor;
}

float ajusteFino(Matriz *mat, FMatriz *fmat, int lin, int col, Entrada *in){
    float var1, var2;
    var1 = pow(getFloat(fmat, lin, col), in->alpha);
    var2 = pow(getDesejo(mat, lin, col), in->beta);
    return var1 * var2;
}

float calculaTotal(Formiga *fmg, Matriz *mat, FMatriz *feromonio, Entrada *in){
    int localidade, r;
    float valorBaixo = 0;
    for (localidade = 0; localidade < mat->tam; localidade++){
        if (fmg->linhaTabu[localidade]){
            /*printf("localidade %d\n", localidade);*/
            for (r = 0; r < mat->tam; r++){
                if (fmg->colunaTabu[r]){
                    valorBaixo += ajusteFino(mat, feromonio, localidade, r, in);
                }
            }
        }
    }
    /*printf("-------------------------------------------------\n");*/
    return valorBaixo;
}

float probabilidade(Matriz *mat, FMatriz *feromonio, Formiga *fmg, int localidade, int atividade, float total, Entrada *in){
    int r;
    float valorCima;
    float prob;
    float maiorProb = 0;
    if (fmg->linhaTabu[localidade]){
        valorCima = ajusteFino(mat, feromonio, localidade, atividade, in);
        prob = valorCima / total;
    } else {
        prob = 0;
    }
    return prob;
}

ElemSolucao *passo(Formiga *fmg, Matriz *fluxo, Matriz *matA, FMatriz *feromonio, int atividade, Entrada *in){
    int localidade;
    int maiorAtividade, maiorLocalidade, maiorValor;
    ElemSolucao *elem;
    float total, prob, maiorProb;

    marcaAtividadeVisitada(fmg, atividade);
    for (localidade = 0; localidade < fluxo->tam; localidade++){
        total = calculaTotal(fmg, matA, feromonio, in);
        prob = probabilidade(matA, feromonio, fmg, localidade, fluxo->ref[atividade], total, in);
        if (maiorProb < prob){
            maiorProb = prob;
            maiorAtividade = atividade;
            maiorLocalidade = localidade;
            maiorValor = getElem(matA, localidade, fluxo->ref[atividade]);
        }
    }
    elem = novoElemSolucao(maiorAtividade, maiorLocalidade, maiorValor);
    marcaLocalidadeVisitada(fmg, maiorLocalidade);
    return elem;
}

int pegaMaior(Matriz *fluxo){
    int i, max, index;
    max = 0;
    for (i = 0; i < fluxo->tam; i++){
        if (fluxo->potencial[i] > max){
            max = fluxo->potencial[i];
            index = i;
        }
    }
    fluxo->potencial[index]++;
    fluxo->potencial[index] = fluxo->potencial[index] * -1;
    return index;
}

void resetMaior(Matriz *fluxo, int index){
    fluxo->potencial[index] = fluxo->potencial[index] * -1;
    fluxo->potencial[index]--;
}

void caminho(Formiga *fmg, Matriz *fluxo, Matriz *matA, FMatriz *feromonio, Entrada *in){
    int i;
    /*int somaTabu = fmg->qtdColunasAtivas;*/
    ElemSolucao *sol;
    for (i = 0; i < fluxo->tam; i++){
        /*printf("tamanho %d\n", fluxo->tam);*/
        /*atividade = pegaMaior(fluxo);*/
        shuffle(fluxo, fluxo->tam);
        /*printf("maior %d\n", atividade);*/
        /*printf("%d %d\n", idAtividade, fluxo->elementos[idAtividade]);*/
        //fluxo->potencial[fluxo->ref[i]]
        sol = passo(fmg, fluxo, matA, feromonio, i, in);
        /*printf("%d\n", sol);*/
        addPar(fmg, sol);
        /*printf("----------------------------------------------------------\n");*/
    }
    for (i = 0; i < fluxo->tam; i++){
        resetMaior(fluxo, i);
    }
}

void iniciliazaFeromonio(FMatriz *feromonio){
    int i, j;
    for (i = 0; i < feromonio->tam; i++){
        for (j = 0; j < feromonio->tam; j++){
            setFloat(feromonio, i, j, 10);
        }
    }
}

void depositaFeromonio(Formiga *vetorFormigas, FMatriz *feromonio, Entrada *in){
    int i, k;
    int loc, atv;
    float valFeromonio = 0;
    for (k = 0; k < in->qtdFormigas; k++){
        for (i = 0; i < feromonio->tam; i++){
            valFeromonio = in->q / vetorFormigas[k].peso;
            loc = vetorFormigas[k].solucao[i].localidade;
            atv = vetorFormigas[k].solucao[i].atividade;
            setFloat(feromonio, loc, atv, getFloat(feromonio, loc, atv) + valFeromonio);
        }
    }
}

void evaporaFeromonio(FMatriz *feromonio, Entrada *in){
    int i, j;
    float fatorEvaporacao;
    fatorEvaporacao = (1 - in->rho);
    for (i = 0; i < feromonio->tam; i++){
        for (j = 0; j < feromonio->tam; j++){
            setFloat(feromonio, i, j, getFloat(feromonio, i, j) * fatorEvaporacao);
        }
    }
}

void atualizaFeromonio(Formiga *vetorFormigas, FMatriz *feromonio, Entrada *in){
    evaporaFeromonio(feromonio, in);
    depositaFeromonio(vetorFormigas, feromonio, in);
}

void criaFormigas(Formiga *vetorFormigas, int tam, Entrada *in){
    int i;
    for (i = 0; i < in->qtdFormigas; i++){
        vetorFormigas[i] = *novaFormiga(tam);
    }
}

void clonaVetor(ElemSolucao *vet1, ElemSolucao *vet2, int tam){
    int i;
    for (i = 0; i < tam; i++){
        vet1[i] = vet2[i];
    }
}

/*void quick(Matriz *mat, int esq, int dir){
    int pivo = esq,i,ch,j;
    for(i = esq + 1;i <= dir; i++){
        j = i;
        if(mat->potencial[mat->ref[j]] < mat->potencial[mat->ref[pivo]]){
            ch = mat->ref[j];
            while(j > pivo){    
                mat->ref[j] = mat->ref[j-1];
                j--;
            }
            mat->ref[j] = ch;
            pivo++;        
        }  
    }
    if(pivo - 1 >= esq){
        quick(mat, esq, pivo - 1);
    }
    if(pivo + 1 <= dir){
        quick(mat, pivo + 1, dir);
    }
}*/

int main(int argc, char *argv[]){
    int i, j, k;

    int tamanho;
    int nCiclos;
    int melhorIdFormiga;
    int melhorPeso = 2147483647; //Maior valor de inteiro em C

    int *somaFluxo;
    int *somaDistancia;

    Matriz *matrizFluxo;
    Matriz *matrizDistancia;
    Matriz *matrizPareada;

    FMatriz *matrizFeromonio;

    Formiga *vetorFormigas;

    ElemSolucao *melhorSolucao;

    Entrada *parametros;

    srand(time(NULL));

    tamanho = tamanhoMatriz(argv[ARQUIVO]);

    parametros = novaEntrada(argv[ALPHA], argv[BETA], argv[RHO], argv[Q], argv[FORMIGAS], argv[CICLOS]);

    matrizFluxo = novaMatriz(tamanho);
    matrizDistancia = novaMatriz(tamanho);

    leMatriz(matrizFluxo, matrizDistancia, argv[ARQUIVO]);
    /*imprimeMatriz(matrizFluxo);
    printf("\n");*/
    //imprimeMatriz(matrizDistancia);
    //printf("\n");

    matrizFeromonio = novaFMatriz(matrizFluxo->tam);
    iniciliazaFeromonio(matrizFeromonio);

    somaPotencial(matrizFluxo);
    somaPotencial(matrizDistancia);
    printf("antes\n");
    for (i = 0; i < tamanho; i++){
        printf("%d  ", matrizFluxo->potencial[i]);
    }
    printf("\n");
    /*shuffle(matrizFluxo, tamanho);*/
    printf("depois\n");
    for (i = 0; i < tamanho; i++){
        printf("%d  ", matrizFluxo->potencial[i]);
    }
    printf("\n");
    for (i = 0; i < tamanho; i++){
        printf("%d  ", matrizFluxo->potencial[matrizFluxo->ref[i]]);
    }
    printf("\n\n\n");
    matrizPareada = coupling(matrizDistancia->potencial, matrizFluxo->potencial, matrizFluxo->tam);
    imprimeMatriz(matrizPareada);
    printf("\n\n\n");

    /*for (i = 0; i < tamanho; i++){
        printf("fluxo %d %d\n", i, matrizFluxo->potencial[i]);
    }
    printf("\n");*/

    /*quick(matrizFluxo, 0, (matrizFluxo->tam - 1));*/
    /*quick(matrizDistancia, 0, (matrizFluxo->tam - 1));*/
    
    vetorFormigas = malloc(parametros->qtdFormigas *  sizeof(Formiga));

    criaFormigas(vetorFormigas, matrizFluxo->tam, parametros);

    nCiclos = parametros->qtdCiclos;

    melhorSolucao = malloc(tamanho*sizeof(ElemSolucao));

    while (nCiclos){
        for (k = 0; k < parametros->qtdFormigas; k++){
            caminho(&vetorFormigas[k], matrizFluxo, matrizPareada, matrizFeromonio, parametros);
        }
        for (k = 0; k < parametros->qtdFormigas; k++){
            /*printf("formiga %d peso %d\n", k, vetorFormigas[k].peso);*/
            if (vetorFormigas[k].peso < melhorPeso){
                melhorPeso = vetorFormigas[k].peso;
                clonaVetor(melhorSolucao, vetorFormigas[k].solucao, tamanho);
                melhorIdFormiga = k;
            }
        }
        atualizaFeromonio(vetorFormigas, matrizFeromonio, parametros);
        /*imprimeFeromonio(matrizFeromonio);*/
        for (k = 0; k < parametros->qtdFormigas; k++){
            vetorFormigas[k] = *novaFormiga(tamanho);
        }
        nCiclos--;
        /*printf("%d\n", melhorPeso);*/
    }
    printf("\n\n");
    for (i = 0; i < tamanho; i++){
        printf("%d  ", matrizFluxo->potencial[matrizFluxo->ref[i]]);
    }
    printf("\n");
    for (i = 0; i < matrizFluxo->tam; i++){
        printf("%d : %d = %d\n", melhorSolucao[i].atividade, melhorSolucao[i].localidade, melhorSolucao[i].valor);
    }
    printf("%d\n", melhorPeso);
    
    
}