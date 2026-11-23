// OpenMP: por enquanto esta é uma cópia da versão sequencial. Paralelize aqui (ex.: o laço de reatribuição
// e o de soma dos pontos em atualizar_particao), conferindo na tabela que 'Rotulos diferentes' continua 0.

#include "kmeans.h"

// Calcula a distância euclidiana ao quadrado entre dois pontos (ponto_a e ponto_b) de dimensão qtd_dimensoes
static double get_distancia_euclidiana_quadrada(const double *ponto_a, const double *ponto_b, int qtd_dimensoes){
  double diferenca, soma = 0.0;
  for (; qtd_dimensoes > 0; --qtd_dimensoes) {
    diferenca = *ponto_a - *ponto_b;
    soma += diferenca * diferenca;
    ++ponto_a;
    ++ponto_b;
  }
  return soma;
}

// Normaliza os dados (matriz qtd_pontos x qtd_dimensoes): (valor - média) / desvio padrão, coluna por coluna
static void normalizar_colunas(double* const dados, const int qtd_pontos, const int qtd_dimensoes){
  // Aponta para a posição logo após a matriz armazenada em sequência.
  const double* const fim = dados + qtd_pontos * qtd_dimensoes;
  int j;
  // Processa cada coluna independentemente.
  for (j = 0; j < qtd_dimensoes; ++j) {
    double valor, desvio_padrao, media = 0.0, media_dos_quadrados = 0.0, *ponteiro;
    // Acumula os valores da coluna e seus quadrados.
    for (ponteiro = dados + j; ponteiro < fim; ponteiro += qtd_dimensoes) {
      valor = *ponteiro;
      media += valor;
      media_dos_quadrados += valor * valor;
    }
    // Calcula a média e a média dos quadrados da coluna (até aqui eram somas).
    media_dos_quadrados /= qtd_pontos;
    media /= qtd_pontos;
    // Calcula o inverso do desvio padrão; evita divisão por zero em coluna constante.
    desvio_padrao = fabs(media_dos_quadrados - media * media);
    if (desvio_padrao == 0.0) desvio_padrao = 1.0;
    desvio_padrao = 1.0 / sqrt(desvio_padrao);
    // Centraliza os valores pela média e os escala pelo desvio padrão.
    for (ponteiro = dados + j; ponteiro < fim; ponteiro += qtd_dimensoes) {
      *ponteiro = (*ponteiro - media) * desvio_padrao;
    }
  }
}

// Verifica se 'valor' aparece entre os 'tamanho' primeiros elementos de 'lista'.
static char contem_valor(const int *lista, const int valor, int tamanho){
  for (; tamanho > 0; --tamanho) {
    if (*lista == valor) return 1;
    ++lista;
  }
  return 0;
}

// Escolhe os k centros iniciais sorteando k pontos distintos dos dados.
// A semente do rand() é definida fora (no main), para que todos os métodos comecem dos mesmos centros.
static void definir_centroides_iniciais(const double* const dados, double* const centroides, const int qtd_pontos, const int qtd_dimensoes, const int qtd_clusters){
  int *indices_sorteados = (int*)malloc(qtd_clusters * sizeof(int));

  // Escolhe um centro por iteração (garantindo que seja distinto dos já escolhidos)
  int i;
  for (i = 0; i < qtd_clusters; ++i) {
    int sorteado = rand() % qtd_pontos;
    while (contem_valor(indices_sorteados, sorteado, i)) { // Verifica se já foi escolhido como centro
      sorteado = rand() % qtd_pontos;
    }
    indices_sorteados[i] = sorteado;
    memcpy(centroides + i * qtd_dimensoes, dados + sorteado * qtd_dimensoes, qtd_dimensoes * sizeof(double));
  }
  free(indices_sorteados);
}

// Determina a qual cluster o ponto pertence (o do centroide mais próximo), dados os k centroides.
static int get_cluster_mais_proximo(const double* const ponto, const double* const centroides, const int qtd_dimensoes, int qtd_clusters){
  int cluster_mais_proximo = --qtd_clusters;
  double menor_distancia = get_distancia_euclidiana_quadrada(ponto, centroides + qtd_clusters * qtd_dimensoes, qtd_dimensoes);
  while (qtd_clusters > 0) {
    --qtd_clusters;
    const double distancia_atual = get_distancia_euclidiana_quadrada(ponto, centroides + qtd_clusters * qtd_dimensoes, qtd_dimensoes);
    if (distancia_atual < menor_distancia) {
      menor_distancia = distancia_atual;
      cluster_mais_proximo = qtd_clusters;
    }
  }
  return cluster_mais_proximo;
}

// Determina a partição inicial (rotulos) e o número de elementos em cada cluster (qtd_por_cluster) a partir dos centroides.
static int* definir_particao_inicial(const double* const dados, const double* const centroides, int* const qtd_por_cluster, int qtd_pontos, const int qtd_dimensoes, const int qtd_clusters){
  int *rotulos = (int*)malloc(qtd_pontos * sizeof(int));
  if (!rotulos) return NULL;
  memset(qtd_por_cluster, 0, qtd_clusters * sizeof(int));
  int i;
  for (i = 0; i < qtd_pontos; ++i) {
    const int cluster = get_cluster_mais_proximo(dados + i * qtd_dimensoes, centroides, qtd_dimensoes, qtd_clusters);
    rotulos[i] = cluster;
    ++qtd_por_cluster[cluster];
  }
  return rotulos;
}

// Executa uma iteração do k-means: recalcula os centroides e reatribui os pontos.
// Retorna 1 se algum ponto mudou de cluster (continua iterando) e 0 se nada mudou (terminou).
static char atualizar_particao(const double* const dados, double* const centroides, int* const rotulos, int* const qtd_por_cluster, const int qtd_pontos, const int qtd_dimensoes, const int qtd_clusters){
  memset(centroides, 0, qtd_clusters * qtd_dimensoes * sizeof(double));
  int i, j;
  // Soma os pontos de cada cluster dentro do centroide correspondente
  for (i = 0; i < qtd_pontos; ++i) {
    double* const centroide_do_cluster = centroides + rotulos[i] * qtd_dimensoes;
    const double* const ponto = dados + i * qtd_dimensoes;
    for (j = 0; j < qtd_dimensoes; ++j) {
      centroide_do_cluster[j] += ponto[j];
    }
  }
  // Divide pela quantidade de pontos do cluster: soma / quantidade = média
  for (i = 0; i < qtd_clusters; ++i) {
    const double tamanho_do_cluster = qtd_por_cluster[i];
    double* const centroide = centroides + i * qtd_dimensoes;
    for (j = 0; j < qtd_dimensoes; ++j) {
      centroide[j] /= tamanho_do_cluster;
    }
  }
  // Reatribui cada ponto ao centroide mais próximo e recontabiliza os tamanhos
  memset(qtd_por_cluster, 0, qtd_clusters * sizeof(int));
  char mudou = 0;
  for (i = 0; i < qtd_pontos; ++i) {
    const int novo_cluster = get_cluster_mais_proximo(dados + i * qtd_dimensoes, centroides, qtd_dimensoes, qtd_clusters);
    if (rotulos[i] != novo_cluster) mudou = 1;
    rotulos[i] = novo_cluster;
    ++qtd_por_cluster[novo_cluster];
  }
  return mudou;
}

// k-means openmp (com normalização). Retorna os rótulos (quem chama dá free) ou NULL se faltar memória.
int* kmeans_openmp(const double* const dados_originais, const int qtd_pontos, const int qtd_dimensoes, const int qtd_clusters){
  double *dados = (double*)malloc(qtd_pontos * qtd_dimensoes * sizeof(double));
  memcpy(dados, dados_originais, qtd_pontos * qtd_dimensoes * sizeof(double));
  normalizar_colunas(dados, qtd_pontos, qtd_dimensoes);
  double *centroides = (double*)malloc(qtd_clusters * qtd_dimensoes * sizeof(double));
  definir_centroides_iniciais(dados, centroides, qtd_pontos, qtd_dimensoes, qtd_clusters);
  int *qtd_por_cluster = (int*)malloc(qtd_clusters * sizeof(int));
  int *rotulos = definir_particao_inicial(dados, centroides, qtd_por_cluster, qtd_pontos, qtd_dimensoes, qtd_clusters);
  if (!rotulos) return NULL;
  while (atualizar_particao(dados, centroides, rotulos, qtd_por_cluster, qtd_pontos, qtd_dimensoes, qtd_clusters));
  free(dados);
  free(centroides);
  free(qtd_por_cluster);
  return rotulos;
}

// Executa o k-means openmp sem normalizar os dados.
int* kmeans_openmp_sem_normalizacao(const double* const dados, const int qtd_pontos, const int qtd_dimensoes, const int qtd_clusters){
  double *centroides = (double*)malloc(qtd_clusters * qtd_dimensoes * sizeof(double));
  definir_centroides_iniciais(dados, centroides, qtd_pontos, qtd_dimensoes, qtd_clusters);
  int *qtd_por_cluster = (int*)malloc(qtd_clusters * sizeof(int));
  int *rotulos = definir_particao_inicial(dados, centroides, qtd_por_cluster, qtd_pontos, qtd_dimensoes, qtd_clusters);
  if (!rotulos) return NULL;
  while (atualizar_particao(dados, centroides, rotulos, qtd_por_cluster, qtd_pontos, qtd_dimensoes, qtd_clusters));
  free(centroides);
  free(qtd_por_cluster);
  return rotulos;
}
