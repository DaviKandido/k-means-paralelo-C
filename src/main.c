#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#include "help.h"
#include "kmeans.h"

// Todas as versões do k-means têm esta mesma assinatura
typedef int* (*FuncaoKMeans)(const double* const, const int, const int, const int);

#define QTD_METODOS 3
static const char *nomes_metodos[QTD_METODOS] = {"sequencial", "openmp", "cuda"};
static const FuncaoKMeans funcoes_metodos[QTD_METODOS] = {kmeans_sequencial, kmeans_openmp, kmeans_cuda};

int main(int argc, char **argv) {
	
	if (argc < 6) {
		puts("Uso: programa <dados> <n> <m> <k> <saida> [ideal|-] [sequencial|openmp|cuda|todos] [semente]");
		exit(1);
	}
	const int n = atoi(argv[2]), m = atoi(argv[3]), k = atoi(argv[4]);
	if (n < 1 || m < 1 || k < 1 || k > n) {
		puts("Values of input parameters are incorrect...");
		exit(1);
	}

	// Argumentos opcionais: "-" no lugar do ideal significa "sem arquivo ideal"
	const char *arquivo_ideal = (argc > 6 && strcmp(argv[6], "-") != 0) ? argv[6] : NULL;
	const char *modo = (argc > 7) ? argv[7] : "sequencial";
	const unsigned int semente = (argc > 8) ? (unsigned int)atoi(argv[8]) : (unsigned int)time(NULL);
	const int todos = strcmp(modo, "todos") == 0;
	int modo_valido = todos;

	for (int i = 0; i < QTD_METODOS; ++i) {
		if (strcmp(modo, nomes_metodos[i]) == 0) modo_valido = 1;
	}
	if (!modo_valido) {
		puts("Modo invalido: use sequencial, openmp, cuda ou todos.");
		exit(1);
	}

	double *x = (double*)malloc(n * m * sizeof(double));
	if (!x) {
		puts("Erro de alocação de memória (talvez não haja memória suficiente para o método escolhido)...");
		exit(1);
	}
	scanner_data(argv[1], x, n * m);

	int *ideal = NULL;
	if (arquivo_ideal) {
		ideal = (int*)malloc(n * sizeof(int));
		if (ideal) scanner_ideal_rotulos(arquivo_ideal, ideal, n);
	}

	// Executa cada método escolhido. Antes de cada um, srand(semente) garante os MESMOS centroides iniciais.
	int *rotulos[QTD_METODOS] = {NULL, NULL, NULL};
	double tempos[QTD_METODOS] = {0.0, 0.0, 0.0};
	for (int i = 0; i < QTD_METODOS; ++i) {
		if (!todos && strcmp(modo, nomes_metodos[i]) != 0) continue;
		srand(semente);
		const double inicio = omp_get_wtime(); // tempo real (clock() somaria o tempo de todas as threads)
		rotulos[i] = funcoes_metodos[i](x, n, m, k);
		tempos[i] = omp_get_wtime() - inicio;
		if (!rotulos[i]) {
			puts("Erro de alocação de memória (talvez não haja memória suficiente para o método escolhido)...");
			exit(1);
		}
	}

	// Tabela de comparação
	printf("\nsemente = %u | threads OpenMP = %d | n = %d, m = %d, k = %d\n", semente, omp_get_max_threads(), n, m, k);
	printf("%-12s %-12s %-10s %-10s %s\n", "Metodo", "Tempo (s)", "Speedup", "Precisao", "Rotulos diferentes do sequencial");
	for (int i = 0; i < QTD_METODOS; ++i) {
		if (!rotulos[i]) continue;
		printf("%-12s %-12.6f", nomes_metodos[i], tempos[i]);
		if (rotulos[0]) printf(" %-10.2f", tempos[0] / tempos[i]);
		else printf(" %-10s", "-");
		if (ideal) printf(" %-10.5f", get_precision(ideal, rotulos[i], n));
		else printf(" %-10s", "-");
		if (rotulos[0]) printf(" %d\n", contar_rotulos_diferentes(rotulos[0], rotulos[i], n));
		else printf(" -\n");
	}

	// Grava o resultado de cada método (no modo "todos" o nome do método é acrescentado ao arquivo)
	for (int i = 0; i < QTD_METODOS; ++i) {
		if (!rotulos[i]) continue;
		char nome_saida[1024];
		if (todos) snprintf(nome_saida, sizeof(nome_saida), "%s.%s", argv[5], nomes_metodos[i]);
		else snprintf(nome_saida, sizeof(nome_saida), "%s", argv[5]);
		if (ideal) printf_full_result(nome_saida, rotulos[i], n, get_precision(ideal, rotulos[i], n));
		else printf_result(nome_saida, rotulos[i], n);
	}

	for (int i = 0; i < QTD_METODOS; ++i) free(rotulos[i]);
	free(ideal);
	free(x);
	return 0;
}
