#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FLUXO 1
#define DISTANCIA 2
#define ALPHA 3
#define BETA 4
#define RHO 5
#define Q 6
#define FORMIGAS 7
#define CICLOS 8

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
    ElemSolucao *solucao;
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

void resetFormiga(Formiga *fmg, int tam){
    free(fmg->linhaTabu);
    free(fmg->colunaTabu);
    free(fmg->solucao);
    free(fmg);
    fmg = novaFormiga(tam);
}

void resetMelhorFormiga(Formiga *fmg, int tam){
    free(fmg->linhaTabu);
    free(fmg->colunaTabu);
    free(fmg);
    fmg = novaFormiga(tam);
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

void marcaVisitado(Formiga *fmg, int lin, int col){
    fmg->linhaTabu[lin] = 0;
    fmg->qtdLinhasAtivas--;
    fmg->colunaTabu[col] = 0;
    fmg->qtdColunasAtivas--;
}

void desmarcaVisitado(Formiga *fmg, int lin, int col){
    fmg->linhaTabu[lin] = 1;
    fmg->qtdLinhasAtivas++;
    fmg->colunaTabu[lin] = 1;
    fmg->qtdColunasAtivas++;
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

Matriz *leMatriz(char *path){
    int i, j;
    int tam, valor;
    Matriz *mat;

    FILE *file;
    file = fopen(path, "r");
    fscanf(file, "%d", &tam);

    mat = novaMatriz(tam);

    for (i = 0; i < mat->tam; i++){
        for (j = 0; j < mat->tam; j++){
            fscanf(file, "%d", &valor);
            setElem(mat, i, j, valor);
        }
    }
    fclose(file);

    return mat;
}

void imprimeMatriz(Matriz *mat){
    int i, j;
    for (i = 0; i < mat->tam; i++){
        for (j = 0; j < mat->tam; j++){
            printf("%d\n", getElem(mat, i, j));
        }
    }
}

int buscaMaior(Matriz *mat){
    int i, max;
    max = mat->potencial[0];
    for (i = 0; i < mat->tam; i++){
        if (mat->potencial[i] > max){
            max = mat->potencial[i];
        }
    }
    return max;
}

float *calculaHeuristica(Matriz *matD){
    int i, j;
    float *n = malloc(sizeof(int)*matD->tam);
    for (j = 0; j < matD->tam; j++){
        n[j] = 1 / (float)matD->potencial[j];
    }
    return n;
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
            for (r = 0; r < mat->tam; r++){
                if (fmg->colunaTabu[r]){
                    valorBaixo += ajusteFino(mat, feromonio, localidade, r, in);
                }
            }
        }
    }
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
            maiorAtividade = fluxo->ref[atividade];
            maiorLocalidade = localidade;
            maiorValor = getElem(matA, localidade, fluxo->ref[atividade]);
        }
    }
    elem = novoElemSolucao(maiorAtividade, maiorLocalidade, maiorValor);
    marcaLocalidadeVisitada(fmg, maiorLocalidade);
    return elem;
}

void caminho(Formiga *fmg, Matriz *fluxo, Matriz *matA, FMatriz *feromonio, Entrada *in){
    int idAtividade;
    int somaTabu = fmg->qtdColunasAtivas;
    ElemSolucao *sol;
    for (idAtividade = fluxo->tam - 1; idAtividade > -1; idAtividade--){
        sol = passo(fmg, fluxo, matA, feromonio, idAtividade, in);
        addPar(fmg, sol);
    }
}

void depositaFeromonio(Formiga *vetorFormiga, FMatriz *feromonio, Entrada *in){
    int i, k;
    int loc, atv;
    float valFeromonio = 0;
    for (k = 0; k < in->qtdFormigas; k++){
        for (i = 0; i < feromonio->tam; i++){
            valFeromonio = in->q / vetorFormiga[k].peso;
            loc = vetorFormiga[k].solucao[i].localidade;
            atv = vetorFormiga[k].solucao[i].atividade;
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

void atualizaFeromonio(Formiga *vetorFormiga, FMatriz *feromonio, Entrada *in){
    evaporaFeromonio(feromonio, in);
    depositaFeromonio(vetorFormiga, feromonio, in);
}

void criaFormigas(Formiga *vetorFormigas, int tam, Entrada *in){
    int i;
    for (i = 0; i < in->qtdFormigas; i++){
        vetorFormigas[i] = *novaFormiga(tam);
    }
}

void quick(Matriz *mat, int esq, int dir){
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
}

int main(int argc, char *argv[]){
    int i, j, k;
    Matriz *matrizFluxo;
    Matriz *matrizDistancia;
    Matriz *matrizPareada;

    FMatriz *matrizFeromonio;

    Formiga *formiga;
    Formiga *vetorFormigas;

    ElemSolucao *melhorSolucao;

    Entrada *parametros;

    int *somaFluxo;
    int *somaDistancia;
    int maior1, maior2;
    int melhorPeso = 2147483647; //Maior valor de inteiro em C
    int nCiclos;
    int melhorIdFormiga;

    float vontade;
    float prob1;
    float alpha, beta;

    matrizFluxo = leMatriz(argv[FLUXO]);
    matrizDistancia = leMatriz(argv[DISTANCIA]);
    parametros = novaEntrada(argv[ALPHA], argv[BETA], argv[RHO], argv[Q], argv[FORMIGAS], argv[CICLOS]);

    matrizFeromonio = novaFMatriz(matrizFluxo->tam);
    for (i = 0; i < matrizFeromonio->tam; i++){
        for (j = 0; j < matrizFeromonio->tam; j++){
            setFloat(matrizFeromonio, i, j, 0.0015);
        }
    }

    somaPotencial(matrizFluxo);
    somaPotencial(matrizDistancia);
    matrizPareada = coupling(matrizDistancia->potencial, matrizFluxo->potencial, matrizFluxo->tam);

    quick(matrizFluxo, 0, (matrizFluxo->tam - 1));
    quick(matrizDistancia, 0, (matrizFluxo->tam - 1));

    /*formiga = novaFormiga(matrizFluxo->tam);*/
    
    vetorFormigas = malloc(parametros->qtdFormigas *  sizeof(Formiga));

    /*vetorFormigas[0] = *formiga;*/

    criaFormigas(vetorFormigas, matrizFluxo->tam, parametros);

    nCiclos = parametros->qtdCiclos;

    while (nCiclos){
        for (k = 0; k < parametros->qtdFormigas; k++){
            caminho(&vetorFormigas[k], matrizFluxo, matrizPareada, matrizFeromonio, parametros);
        }
        for (k = 0; k < parametros->qtdFormigas; k++){
            if (vetorFormigas[k].peso < melhorPeso){
                melhorPeso = vetorFormigas[k].peso;
                melhorSolucao = vetorFormigas[k].solucao;
                melhorIdFormiga = k;
            }
        }
        atualizaFeromonio(vetorFormigas, matrizFeromonio, parametros);
        for (k = 0; k < parametros->qtdFormigas; k++){
            if (k == melhorIdFormiga){
                resetMelhorFormiga(&vetorFormigas[k], matrizFluxo->tam);
            } else {
                resetFormiga(&vetorFormigas[k], matrizFluxo->tam);
            }
        }
        nCiclos--;
    }
    for (i = 0; i < matrizFluxo->tam; i++){
        printf("%d : %d = %d\n", melhorSolucao[i].atividade, melhorSolucao[i].localidade, melhorSolucao[i].valor);
    }
    printf("%d\n", melhorPeso);
    
}