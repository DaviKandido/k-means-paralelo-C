#ifndef KMEANS_H_
#define KMEANS_H_

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


// Calcula a distância euclidiana entre dois pontos (x1 e x2) de dimensão m
double get_distancia_euclidiana_quadrada(const double *x1, const double *x2, int m);

// Normaliza os dados (x) de dimensão n x m
void cols_normalization(double* const x, const int n, const int m);

// Verifica se val aparece entre os s primeiros elementos de nums (contains).
char constr(const int *nums, const int val, int s);

// Escolhe os k centros iniciais sorteando k pontos distintos dos dados.
void det_centroides(const double* const x, double* const c, const int n, const int m, const int k);

// Determina a qual cluster o ponto x pertence, dado os k centros c.
int get_cluster(const double* const x, const double* const c, const int m, int k);

// Determina a partição inicial (y) e o número de elementos em cada cluster (nums) a partir dos centros c.
int* det_start_partition(const double* const x, const double* const c, int* const nums, int n, const int m, const int k);

// Executa uma iteração do k-means: recalcula os centros e reatribui os pontos.
char check_partition(const double* const x, double* const c, int* const y, int* const nums, const int n, const int m, const int k);


int* kmeans(const double* const X, const int n, const int m, const int k);


int* kmeans_no_normalization(const double* const x, const int n, const int m, const int k);

#endif