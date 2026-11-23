
#include "help.h"

// Lê os pontos do data.txt para a memória (qtd_valores = n * m)
void scanner_data(const char *file_name, double *dados, const int qtd_valores){
  FILE *fl = fopen(file_name, "r");
  if(!fl){
    printf("Erro na abertura do arquivo %s...\n", file_name);
    exit(1);
  }
  int i = 0;
  while(i< qtd_valores){
    if(fscanf(fl, "%lf", dados +i) != 1){ //lê 1 double e guarda em dados[i]
      printf("Erro na leitura de %s: esperava %d números, mas só leu %d\n", file_name, qtd_valores, i);
      exit(1);
    }
    ++i;
  }
  fclose(fl);
}

// Gravam o resultado em arquivo (qtd_pontos = n)
void printf_result(const char *file_name, const int* const rotulos, const int qtd_pontos){
  FILE *fl = fopen(file_name, "a");
  if(!fl){
    printf("Erro na abertura do arquivo %s de resultados...\n", file_name);
    exit(1);
  }
  fprintf(fl, "Resultado do k-means clustering...\n");
  int i;
  for(i=0; i<qtd_pontos; ++i){
    fprintf(fl, "Objeto [%d] = %d;\n", i, rotulos[i]);
  }
  fputc('\n', fl);
  fclose(fl);
}


void printf_full_result(const char *file_name, const int* const rotulos, const int qtd_pontos, const double precision){
  FILE *fl = fopen(file_name, "a");
  if(!fl){
    printf("Erro na abertura do arquivo %s de resultados...\n", file_name);
    exit(1);
  }
  fprintf(fl, "Resultado do k-means clustering...\nPrecisão do k-means clustering = %.5lf;\n", precision);
  int i;
  for(i=0; i<qtd_pontos; ++i){
    fprintf(fl, "Objeto [%d] = %d;\n", i, rotulos[i]);
  }
  fputc('\n', fl);
  fclose(fl);
}

// Lê os rótulos ideais do res.txt
void scanner_ideal_rotulos(const char *file_name, int *rotulos_ideais, const int qtd_pontos){
  FILE *fl = fopen(file_name, "r");
  if(!fl){
    printf("Não foi possível acessar o arquivo %s com a partição ideal para leitura...\n", file_name);
    exit(1);
  }
  int i = 0;
  while(i< qtd_pontos){
    if(fscanf(fl, "%d", rotulos_ideais +i) != 1){
      printf("Erro na leitura da partição perfeita do arquivo %s\n", file_name);
      exit(1);
    }
    ++i;
  }
  fclose(fl);
}

// Compara o resultado do k-means com o ideal (retorna a precisão)
double get_precision(int *rotulos_ideais, int *rotulos_kmeans, const int qtd_pontos){
  unsigned long yy = 0ul, ny = 0ul;
	int i, j;
	for (i = 0; i < qtd_pontos; ++i) {
		for (j = i + 1; j < qtd_pontos; ++j) {
			if (rotulos_ideais[i] == rotulos_ideais[j] && rotulos_kmeans[i] == rotulos_kmeans[j]) ++yy;
			if (rotulos_ideais[i] != rotulos_ideais[j] && rotulos_kmeans[i] == rotulos_kmeans[j]) ++ny;
		}
	}
	return yy == 0ul && ny == 0ul ? 0.0 : (double)yy / (double)(yy + ny);
}

// Conta em quantos pontos os dois vetores de rótulos diferem (usado para comparar sequencial x OpenMP x CUDA)
int contar_rotulos_diferentes(const int *rotulos_a, const int *rotulos_b, const int qtd_pontos){
  int diferentes = 0, i;
  for(i=0; i<qtd_pontos; ++i){
    if(rotulos_a[i] != rotulos_b[i]) ++diferentes;
  }
  return diferentes;
}
