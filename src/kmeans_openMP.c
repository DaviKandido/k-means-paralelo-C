#include "kmeans.h"


// Calcula a distância euclidiana entre dois pontos (x1 e x2) de dimensão m
double get_distancia_euclidiana_quadrada(const double *x1, const double *x2, int m){
  double d, r = 0.0;
  for (; m > 0; --m) {
    d = *x1 - *x2;
    r += d * d;
    ++x1;
    ++x2;
  }
  return r;
}

// Normaliza os dados (x) de dimensão n x m
void cols_normalization(double* const x, const int n, const int m){
  // Aponta para a posição logo após a matriz armazenada em sequência.
  const double* const end = x + n * m;
  int j;
  // Processa cada coluna independentemente.
  for (j = 0; j < m; ++j) {
    double sd, Ex = 0.0, Exx = 0.0, *ptr;
    // Acumula os valores da coluna e seus quadrados.
    for (ptr = x + j; ptr < end; ptr += m) {
      sd = *ptr;
      Ex += sd;
      Exx += sd * sd;
    }
    // Calcula a média e a variância da coluna.
    Exx /= n;
    Ex /= n;
    // Calcula o inverso do desvio padrão; evita divisão por zero em coluna constante.
    sd = fabs(Exx - Ex * Ex);
    if (sd == 0.0) sd = 1.0;
    sd = 1.0 / sqrt(sd);
    // Centraliza os valores pela média e os escala pelo desvio padrão.
    for (ptr = x + j; ptr < end; ptr += m) {
      *ptr = (*ptr - Ex) * sd;
    }
  }
}

// Verifica se val aparece entre os s primeiros elementos de y (contains).
char constr(const int *nums, const int val, int s){
  for (; s > 0; --s) {
    if (*nums == val) return 1;
    ++nums;
  }
  return 0;
}

// Escolhe os k centros iniciais sorteando k pontos distintos dos dados.
void det_centroides(const double* const x, double* const c, const int n, const int m, const int k){
  int *nums = (int*)malloc(k * sizeof(int));
  srand((unsigned int)time(NULL)); // Semente do gerador = hora atual (em segundos)
  
  // Escolhe um centro por iteração (garantindo que seja distinto dos já escolhidos)
  int i;
  for (i = 0; i < k; ++i) {
    int val = rand() % n;
    while (constr(nums, val, i)) { // Verifica se já foi escolhido como centro
      val = rand() % n;
    }
    nums[i] = val;
    memcpy(c + i * m, x + val * m, m * sizeof(double));
  }
  free(nums);
}

// Determina a qual cluster o ponto x pertence, dado os k centros c.
int get_cluster(const double* const x, const double* const c, const int m, int k){
  int res = --k;
	double min_d = get_distancia_euclidiana_quadrada(x, c + k * m, m);
	while (k > 0) {
		--k;
		const double cur_d = get_distancia_euclidiana_quadrada(x, c + k * m, m);
		if (cur_d < min_d) {
			min_d = cur_d;
			res = k;
		}
	}
	return res;
}

// Determina a partição inicial (y) e o número de elementos em cada cluster (nums) a partir dos centros c.
int* det_start_partition(const double* const x, const double* const c, int* const nums, int n, const int m, const int k){
  int *y = (int*)malloc(n * sizeof(int));
  if (!y) return NULL;
  memset(nums, 0, k * sizeof(int));
  int i;
  for (i = 0; i < n; ++i) {
    const int f = get_cluster(x + i * m, c, m, k);
    y[i] = f;
    ++nums[f];
  }
  return y;
}

// Executa uma iteração do k-means: recalcula os centros e reatribui os pontos.
char check_partition(const double* const x, double* const c, int* const y, int* const nums, const int n, const int m, const int k){
  memset(c, 0, k * m * sizeof(double));
	int i, j;
	for (i = 0; i < n; ++i) {
		double* const c_yi = c + y[i] * m;
		const double* const x_i = x + i * m;
		for (j = 0; j < m; ++j) {
			c_yi[j] += x_i[j];
		}
	}
	for (i = 0; i < k; ++i) {
		const double f = nums[i];
		double* const c_i = c + i * m;
		for (j = 0; j < m; ++j) {
			c_i[j] /= f;
		}
	}
	memset(nums, 0, k * sizeof(int));
	char flag = 0; 
	for (i = 0; i < n; ++i) {
		const int f = get_cluster(x + i * m, c, m, k);
		if (y[i] != f) flag = 1;
		y[i] = f;
		++nums[f];
	}
	return flag;
}


int* kmeans(const double* const X, const int n, const int m, const int k){
  double *x = (double*)malloc(n * m * sizeof(double));
	memcpy(x, X, n * m * sizeof(double));
	cols_normalization(x, n, m);
	double *c = (double*)malloc(k * m * sizeof(double));
	det_centroides(x, c, n, m, k);
	int *nums = (int*)malloc(k * sizeof(int));
	int *y = det_start_partition(x, c, nums, n, m, k);
	if (!y) return NULL;
	while (check_partition(x, c, y, nums, n, m, k));
	free(x);
	free(c);
	free(nums);
	return y;
}

// Executa o k-means sem normalizar os dados.
int* kmeans_no_normalization(const double* const x, const int n, const int m, const int k){
  	double *c = (double*)malloc(k * m * sizeof(double));
	det_centroides(x, c, n, m, k);
	int *nums = (int*)malloc(k * sizeof(int));
	int *y = det_start_partition(x, c, nums, n, m, k);
	if (!y) return NULL;
	while (check_partition(x, c, y, nums, n, m, k));
	free(c);
	free(nums);
	return y;
}