
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <limits>

enum TipoRepresentacao {
    MATRIZ,
    LISTA
};

class Grafo {
private:
    int n;                  // número de vértices
    int m;                  // número de arestas
    TipoRepresentacao tipo; // representação escolhida
    bool ponderado;         // indica se possui pesos

    // representação em matriz
    int **matrizAdj;
    double **matrizPeso;

    // representação em lista: para cada vértice, array de vizinhos, pesos e grau atual
    int **listaAdj;
    double **listaPeso;
    int *grauAdj;

    // aloca estruturas internas
    void initMatriz();
    void initLista();

public:
    // se ponderado=true, interpreta terceira coluna como peso
    Grafo(int numVertices, TipoRepresentacao rep, bool ponderado = false);
    ~Grafo();

    // leitura de arquivo (dois ou três campos: u v [peso])
    bool lerDeArquivo(const char *nomeArquivo);

    // gera arquivo com: |V|, |E|, grau médio e distribuição empírica
    bool escreverInfo(const char *nomeSaida) const;

    // BFS e DFS (pai e nível), raiz em 1..n
    bool buscaLargura(int raiz, const char *nomeSaida) const;
    bool buscaProfundidade(int raiz, const char *nomeSaida) const;

    // componentes conexas
    bool componentesConexas(const char *nomeSaida) const;

    // caminhos mínimos: orig -> dest
    bool caminhoMinimo(int origem, int destino, const char *nomeSaida) const;
    // caminhos mínimos de origem para todos
    bool caminhosMinimos(int origem, const char *nomeSaida) const;

private:
    void adicionaAresta(int u, int v, double peso);
    int grauVertice(int u) const;
    void dfsRec(int u, bool *vis, int *pai, int *nivel) const;
};

#endif // GRAPH_HPP