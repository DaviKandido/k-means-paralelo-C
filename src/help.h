#ifndef HELP_H_
#define HELP_H_

#include <stdlib.h>
#include <stdio.h>


// Lê os pontos do data.txt para a memória (qtd_valores = n * m)
void scanner_data(const char *file_name, double *dados, const int qtd_valores);

// Gravam o resultado em arquivo (qtd_pontos = n)
void printf_result(const char *file_name, const int* const rotulos, const int qtd_pontos);
void printf_full_result(const char *file_name, const int* const rotulos, const int qtd_pontos, const double precision);

// Lê os rótulos ideais do res.txt
void scanner_ideal_rotulos(const char *file_name, int *rotulos_ideais, const int qtd_pontos);

// Compara o resultado do k-means com o ideal (retorna a precisão)
double get_precision(int *rotulos_ideais, int *rotulos_kmeans, const int qtd_pontos);

// Conta em quantos pontos os dois vetores de rótulos diferem
int contar_rotulos_diferentes(const int *rotulos_a, const int *rotulos_b, const int qtd_pontos);

#endif