#include <stdio.h>
#include <stdlib.h>

#define FLUXO 1
#define DISTANCIA 2

struct SMatriz{
    int tam;
    int *elementos;
};
typedef struct SMatriz Matriz;

Matriz *novaMatriz( int t ){
    Matriz *mat = malloc(sizeof(Matriz));
    mat->tam = t;
    mat->elementos = (int *)malloc(t*t*sizeof(int*));
    return mat;
}

int getElem(Matriz *mat, int lin, int col){
    return mat->elementos[lin*mat->tam + col];
}

void setElem(Matriz *mat, int lin, int col, int val){
    mat->elementos[lin*mat->tam + col] = val;
}

void somaPotencial(Matriz *mat){
    int i, j;
    int tamanho;
    int val;
    tamanho = mat->tam;
    int vet [tamanho];
    val = 0;
    for (j = 0; j < tamanho; j++){
        for (i = 0; i < tamanho; i++){
            val +=  getElem(mat, i, j);
            /*printf("%d, %d, %d\n", val, i, j);*/
        }
        vet[j] = val;
        printf("%d\n", vet[j]);
        val = 0;
    }
}

Matriz *leMatriz(Matriz *mat, char *path){
    int i, j;
    int tam, valor;

    FILE *file;
    file = fopen(path, "r");
    fscanf(file, "%d", &tam);
    mat = novaMatriz(tam);

    for (i = 0; i < tam; i++){
        for (j = 0; j < tam; j++){
            fscanf(file, "%d", &valor);
            setElem(mat, i, j, valor);
            /*printf("%d\n", getElem(mat, i, j));*/
        }
    }

    fclose(file);
    return mat;
}

int main(int argc, char *argv[]){
    Matriz *matrizFluxo;
    Matriz *matrizDistancia;
    Matriz *somaFluxo;

    matrizFluxo = leMatriz(matrizFluxo, argv[FLUXO]);
    somaPotencial(matrizFluxo);
    matrizDistancia = leMatriz(matrizDistancia, argv[DISTANCIA]);
    somaPotencial(matrizDistancia);
}