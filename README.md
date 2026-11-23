# k-means-paralelo-C

Implementação do k-means em C (C99) com três versões comparáveis no mesmo programa:

| Método       | Arquivo                   | Situação                          |
| ------------ | ------------------------- | --------------------------------- |
| `sequencial` | `src/kmeans_sequencial.c` | pronto                            |
| `openmp`     | `src/kmeans_openmp.c`     | cópia do sequencial (a paralelizar) |
| `cuda`       | `src/kmeans_cuda.c`       | cópia do sequencial (a paralelizar) |

## Algoritmo

Agrupa `n` pontos de `m` atributos em `k` clusters. Saída: um rótulo (0 a k-1) por ponto.

1. Normalizar cada coluna: `(x − média) / desvio padrão`.
2. Sortear `k` pontos distintos como centroides iniciais.
3. Atribuir cada ponto ao centroide mais próximo.
4. Recalcular cada centroide como a média dos pontos do seu cluster.
5. Reatribuir os pontos ao centroide mais próximo.
6. Repetir 4 e 5 até nenhum ponto mudar de cluster.

A distância é a euclidiana ao quadrado (sem raiz, pois não muda qual centroide é o mais próximo).

## Estrutura

| Arquivo                 | Função                                                        |
| ----------------------- | ------------------------------------------------------------- |
| `src/main.c`            | argumentos, execução dos métodos, tempo e tabela              |
| `src/kmeans.h`          | declara as três versões                                       |
| `src/kmeans_*.c`        | uma versão do k-means por arquivo                             |
| `src/help.c` / `help.h` | ler dados, gravar resultado, calcular precisão                |
| `datasets/`             | `data.txt` (pontos) e `res.txt` (rótulos corretos)            |


## Funções (`kmeans_sequencial.c`)

| Passo | Função                              | O que faz                                               |
| ----- | ----------------------------------- | ------------------------------------------------------- |
| -     | `get_distancia_euclidiana_quadrada` | distância² entre dois pontos                            |
| 1     | `normalizar_colunas`                | normaliza cada coluna                                   |
| 2     | `definir_centroides_iniciais`       | sorteia `k` pontos distintos (`contem_valor`)           |
| 3     | `definir_particao_inicial`          | primeira atribuição e tamanho dos clusters              |
| -     | `get_cluster_mais_proximo`          | índice do centroide mais próximo de um ponto            |
| 4 e 5 | `atualizar_particao`                | uma iteração; devolve `1` se algum ponto mudou          |
| 6     | `kmeans_sequencial`                 | encadeia tudo e repete até `atualizar_particao` dar `0` |


```c
int* kmeans_xxx(const double* const dados, const int qtd_pontos,
                const int qtd_dimensoes, const int qtd_clusters);
```

## Arquivos

- **`data.txt`**: `n` linhas com `m` números separados por espaço.
- **`res.txt`** (opcional): `n` inteiros, um por linha. Serve para calcular a precisão.



## Compilação

```
make            # Linux
make windows    # Windows
```

Executável: `bin/k-means-paralelo-C`.

## Uso

```
./bin/k-means-paralelo-C <dados> <n> <m> <k> <saida> [ideal|-] [modo] [semente]
```

| Argumento | Significado                                                            |
| --------- | ---------------------------------------------------------------------- |
| `dados`   | arquivo com os pontos                                                  |
| `n`, `m`  | quantidade de pontos e de atributos                                    |
| `k`       | quantidade de clusters                                                 |
| `saida`   | arquivo do resultado (no modo `todos`: `<saida>.<método>`)             |
| `ideal`   | (opcional) arquivo `res.txt`; use `-` para não informar                |
| `modo`    | (opcional) `sequencial`, `openmp`, `cuda` ou `todos` (padrão: sequencial) |
| `semente` | (opcional) semente do sorteio dos centroides (padrão: hora atual)      |

```
./bin/k-means-paralelo-C datasets/iris/data.txt 150 4 3 saida.txt datasets/iris/res.txt todos 42
```

## Comparação dos métodos

O modo `todos` roda os três e mostra:

```
Metodo       Tempo (s)    Speedup    Precisao   Rotulos diferentes do sequencial
sequencial   0.000019     1.00       0.76001    0
openmp       0.000022     0.84       0.76001    0
cuda         0.000017     1.11       0.76001    0
```

- **Tempo**: tempo real da chamada (inclui a normalização), medido com `omp_get_wtime()`.
- **Speedup**: tempo do sequencial ÷ tempo do método.
- **Rótulos diferentes**: deve ser `0`, pois os três partem dos mesmos centroides (`srand(semente)` antes de cada um). Serve para validar as versões paralelas.
- **Threads OpenMP**: `OMP_NUM_THREADS=4 ./bin/k-means-paralelo-C ...`

## Observações

- OpenMP e CUDA ainda são cópias do sequencial (speedup ≈ 1). A versão CUDA, ao usar kernels, deve virar `.cu` e ser compilada com `nvcc`.
- Cluster vazio gera centroide `NaN` (não tratado).
- Os `res.txt` de `page_blocks` e `vehicle_silhouettes` estão incompletos; o programa encerra com erro ao lê-los.
