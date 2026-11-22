# k-means-in-C
Implementação do algoritmo de agrupamento k-means em C (C89/C90 padrão)

## Sobre o k-means
O k-means é um algoritmo iterativo de agrupamento de dados, desenvolvido por Stuart Lloyd, dos Bell Labs, na década de 1950, como uma técnica para modulação por código de pulso.  
A ideia principal do algoritmo é que, a cada iteração, com base no particionamento existente, os centros dos clusters são recalculados e, em seguida, as instâncias são divididas em clusters de acordo com qual dos novos centros fico


## Exemplo de uso

Compilando a partir do código-fonte (Linux):
```
make
```
Compilando a partir do código-fonte (Windows):
```
make windows
```
Se a compilação deu certo, você encontra o executável no subdiretório `bin`.  
Execute o programa:
```
./bin/k-means-in-C ./datasets/iris/data.txt 150 4 3 ./datasets/iris/new_result.txt ./datasets/iris/res.txt
```
```
./bin/k-means-in-C ./datasets/wine/data.txt 178 13 3 ./datasets/wine/new_result.txt ./datasets/wine/res.txt
```