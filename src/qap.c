#include <stdio.h>
#include <stdlib.h>

struct SMatriz{
	int tam;
	int *elementos;
};

typedef struct SMatriz Matriz;

Matriz *novaMatriz( int t ){
	Matriz *m = malloc(sizeof(Matriz));
	m->tam = t;
	m->elementos = (int *)malloc(t*t*sizeof(int*));
	return m;
}

int getElem(Matriz *mat, int lin, int col){
	return mat->elementos[lin*mat->tam + col];
}

void setElem(Matriz *mat, int lin, int col, int val){
	mat->elementos[lin*mat->tam + col] = val;
}

int main(int argc, char const *argv[]){
	int i, j;
	Matriz *matrizFluxo;
	printf("%s\n", 	argv[1]);

	FILE *file;
	file = fopen("fluxo.txt", "r");
	int tam;
	fscanf(file, "%d", &tam);

	matrizFluxo = novaMatriz(tam);

	int valor;

	for (i = 0; i < tam; i++){
		for (j = 0; j < tam; j++){
			fscanf(file, "%d", &valor);
			setElem(matrizFluxo, i, j, valor);
			printf("%d\n", getElem(matrizFluxo, i, j));
		}
	}
	fclose(file);
}