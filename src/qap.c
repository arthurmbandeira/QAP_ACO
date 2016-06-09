#include <stdio.h>
#include <stdlib.h>

#define FLUXO 1
#define DISTANCIA 2
#define CICLOS 3
#define FORMIGAS 4

struct SMatriz{
    int tam;
    int *elementos;
    int *potencial;
};
typedef struct SMatriz Matriz;

Matriz *novaMatriz( int t ){
    Matriz *mat = malloc(sizeof(Matriz));
    mat->tam = t;
    mat->elementos = (int *)malloc(t*t*sizeof(int*));
    mat->potencial = malloc(sizeof(int)*t);
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
            /*printf("%d\n", getElem(mat, i, j));*/
        }
    }
    fclose(file);

    return mat;
}

void printaMatriz(Matriz *mat){
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

int main(int argc, char *argv[]){
    int i;
    Matriz *matrizFluxo;
    Matriz *matrizDistancia;
    int *somaFluxo;
    int *somaDistancia;
    int maior1, maior2;


    matrizFluxo = leMatriz(argv[FLUXO]);
    matrizDistancia = leMatriz(argv[DISTANCIA]);
    printaMatriz(matrizFluxo);
    printaMatriz(matrizDistancia);

    somaFluxo = somaPotencial(matrizFluxo);
    somaDistancia = somaPotencial(matrizDistancia);
    for (i = 0; i < matrizFluxo->tam; i++){
        printf("%d\n", somaFluxo[i]);
    }
    for (i = 0; i < matrizDistancia->tam; i++){
        printf("%d\n", somaDistancia[i]);
    }
    maior1 = buscaMaior(matrizFluxo);
    maior2 = buscaMaior(matrizDistancia);
    printf("Maior1: %d Maior2: %d\n", maior1, maior2);

}