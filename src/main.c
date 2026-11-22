#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "help.h"
#include "kmeans.h"

// TRaduza tudo para portugues
int main(int argc, char **argv) {
	if (argc < 6) {
		puts("Não há parâmetros suficientes...");
		exit(1);
	}
	const int n = atoi(argv[2]), m = atoi(argv[3]), k = atoi(argv[4]);
	if (n < 1 || m < 1 || k < 1 || k > n) {
		puts("O valor dos parâmetros de entrada está incorreto...");
		exit(1);
	}
	double *x = (double*)malloc(n * m * sizeof(double));
	if (!x) {
		puts("Erro de alocação de memória...");
		exit(1);
	}
	scanner_data(argv[1], x, n * m);
	clock_t cl = clock();
	int *y = kmeans(x, n, m, k);
	cl = clock() - cl;
	if (!y) {
		puts("Erro de alocação de memória...");
		free(x);
		exit(1);
	}
	if (argc > 6) {
		int *ideal = (int*)malloc(n * sizeof(int));
		if (!ideal) printf_result(argv[5], y, n);
		else {
			scanner_ideal_rotulos(argv[6], ideal, n);
			const double p = get_precision(ideal, y, n);
			printf("Precisão do agrupamento k-means = %.5lf;\n", p);
			printf_full_result(argv[5], y, n, p);
			free(ideal);
		}
	} else printf_result(argv[5], y, n);
	printf("Tempo para agrupamento k-means = %lf s.;\nO trabalho do programa foi concluído...\n", (double)cl / CLOCKS_PER_SEC);
	free(x);
	free(y);
	return 0;
}
