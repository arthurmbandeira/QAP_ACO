#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FLUXO 1
#define DISTANCIA 2
#define CICLOS 3
#define FORMIGAS 4

// 0 <= ALPHA <= 1

#define ALPHA 5
#define BETA 6
#define RHO 7
#define Q 1

struct SMatriz{
    int tam;
    int *elementos;
    int *potencial;
    int *ref;
};
typedef struct SMatriz Matriz;

struct SFormiga{
    int tam;
    int *listaTabu;
    float *elementos;
};
typedef struct SFormiga Formiga;

Matriz *novaMatriz( int t ){
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

int getElem(Matriz *mat, int lin, int col){
    return mat->elementos[lin*mat->tam + col];
}

void setElem(Matriz *mat, int lin, int col, int val){
    mat->elementos[lin*mat->tam + col] = val;
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

/*Matriz *desejabilidade(Matriz *mat){
    int i, j;
    float val;
    Matriz *desejo;
    desejo = novaMatriz(mat->tam);
    for (i = 0; i < mat->tam; i++){
        for (j = 0; j < mat->tam; j++){
            val = 1 / (float)(getElem(mat, i, j));
            setElem(desejo, i, j, val);
        }
    }
    return desejo;
}*/

Matriz *calculaFeromonio(Matriz *mat){
    int i, j, k;
    float valor;
    Matriz *fer;
    fer = novaMatriz(mat->tam);
    for (i = 0; i < fer->tam; i++){
        for (j = 0; j < fer->tam; j++){
            if (1){
                valor = 1;
                setElem(fer, i, j, valor);
            } else {
                valor = 0;
                setElem(fer, i, j, valor);
            }
        }
    }
    return fer;
}

void quick(Matriz *mat, int esq, int dir){
    int pivo = esq,i,ch,j;
    for(i=esq+1;i<=dir;i++){
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
    if(pivo-1 >= esq){
        quick(mat,esq,pivo-1);
    }
    if(pivo+1 <= dir){
        quick(mat,pivo+1,dir);
    }
}

/*void passo1(){
int i;
for (i = 0; i < count; i++){

}
}

float passo2(Matriz *matDist, int i, int j, int k, int nroFormigas, float alpha, float beta){
float out;
float parteDeCima;
float parteDeBaixo;
parteDeCima = pow(calculaFeromonio(), alpha) * pow(1 / (float)matDist->potencial[j], beta);

for (l = 0; l < matDist->tam; l++){
if (1){
parteDeBaixo += pow(calculaFeromonio(), alpha) * pow(1 / (float)matDist->potencial[l], beta)
}

}
}*/

int main(int argc, char *argv[]){
    int i, j;
    Matriz *matrizFluxo;
    Matriz *matrizDistancia;
    Matriz *matrizFeromonio;
    Matriz *matrizPareada;
    Matriz *matrizVontade;

    int *somaFluxo;
    int *somaDistancia;
    int maior1, maior2;
    float vontade;


    matrizFluxo = leMatriz(argv[FLUXO]);
    matrizDistancia = leMatriz(argv[DISTANCIA]);
    imprimeMatriz(matrizFluxo);
    printf("\n");
    imprimeMatriz(matrizDistancia);
    printf("\n");
    somaPotencial(matrizFluxo);
    somaPotencial(matrizDistancia);
    for (i = 0; i < matrizFluxo->tam; i++){
        printf("%d\n", matrizFluxo->potencial[i]);
    }
    printf("\n");
    for (i = 0; i < matrizDistancia->tam; i++){
        printf("%d\n", matrizDistancia->potencial[i]);
    }
    printf("\n");

    matrizPareada = coupling(matrizDistancia->potencial, matrizFluxo->potencial, matrizFluxo->tam);
    imprimeMatriz(matrizPareada);
    printf("\n");
    for (i = 0; i < matrizPareada->tam; i++){
        for (j = 0; j < matrizPareada->tam; j++){
            vontade = getDesejo(matrizPareada, i, j);
            printf("%f\n", vontade);
        }
    }
}

/*
maior1 = buscaMaior(matrizFluxo);
maior2 = buscaMaior(matrizDistancia);
printf("Maior1: %d Maior2: %d\n", maior1, maior2);
printf("\n");
hDistancia = calculaHeuristica(matrizDistancia);
for (i = 0; i < matrizDistancia->tam; i++){
printf("%f\n", hDistancia[i]);
}
printf("\n");
matrizFeromonio = calculaFeromonio(matrizDistancia);
imprimeMatriz(matrizFeromonio);
printf("\n");
quick(matrizFluxo, 0, (matrizFluxo->tam - 1));
quick(matrizDistancia, 0, (matrizFluxo->tam - 1));
for (i = 0; i < matrizFluxo->tam; i++){
printf("%d\n", matrizFluxo->potencial[matrizFluxo->ref[i]]);
}
*/