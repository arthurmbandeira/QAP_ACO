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

void leMatriz(Matriz *mat, char *path){
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
			printf("%d\n", getElem(mat, i, j));
		}
	}
	fclose(file);
}

int main(int argc, char *argv[]){
	Matriz *matrizFluxo;
	Matriz *matrizDistancia;

	leMatriz(matrizFluxo, argv[FLUXO]);
	leMatriz(matrizDistancia, argv[DISTANCIA]);
	
}