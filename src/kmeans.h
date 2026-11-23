#ifndef KMEANS_H_
#define KMEANS_H_

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// As três versões do k-means têm a mesma assinatura:
//   dados        : matriz qtd_pontos x qtd_dimensoes, guardada linha a linha
//   retorno      : vetor com o cluster (0..qtd_clusters-1) de cada ponto
// As funções auxiliares (distância, normalização, etc.) ficam dentro de cada arquivo .c.
// A semente do rand() deve ser definida antes de chamar (srand), para sortear os centroides iniciais.

int* kmeans_sequencial(const double* const dados, const int qtd_pontos, const int qtd_dimensoes, const int qtd_clusters);
int* kmeans_openmp(const double* const dados, const int qtd_pontos, const int qtd_dimensoes, const int qtd_clusters);
int* kmeans_cuda(const double* const dados, const int qtd_pontos, const int qtd_dimensoes, const int qtd_clusters);

// Versões sem normalização dos dados
int* kmeans_sequencial_sem_normalizacao(const double* const dados, const int qtd_pontos, const int qtd_dimensoes, const int qtd_clusters);
int* kmeans_openmp_sem_normalizacao(const double* const dados, const int qtd_pontos, const int qtd_dimensoes, const int qtd_clusters);
int* kmeans_cuda_sem_normalizacao(const double* const dados, const int qtd_pontos, const int qtd_dimensoes, const int qtd_clusters);

#endif
