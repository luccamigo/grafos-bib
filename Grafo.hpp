
// Interface do grafo e assinaturas das operações básicas.
// Notas de design:
// - Internamente os vértices são indexados de 0..n-1.
// - Na interface pública que recebe vértice de entrada/saída, assume-se 1..n.
// - O grafo é não-direcionado (arestas adicionadas em u<->v).
// - O armazenamento pode ser por matriz de adjacência ou listas de adjacência.
// - Se 'ponderado' for true, os pesos são lidos do arquivo (3ª coluna); caso
//   contrário, cada aresta tem peso 1.0.
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <limits>

// Representação escolhida para o grafo.
// MATRIZ: matriz de adjacência (n x n), boa para grafos densos e consultas O(1).
// LISTA:  listas de adjacência, boa para grafos esparsos.
enum TipoRepresentacao {
    MATRIZ,
    LISTA
};

class Grafo {
private:
    int n;                  // número de vértices
    int m;                  // número de arestas (conta cada aresta uma vez)
    TipoRepresentacao tipo; // representação escolhida (MATRIZ ou LISTA)
    bool ponderado;         // indica se o grafo possui pesos nas arestas

    // representação em matriz
    int **matrizAdj;
    double **matrizPeso;

    // representação em lista: para cada vértice, array de vizinhos, pesos e grau atual
    int **listaAdj;
    double **listaPeso;
    int *grauAdj;

    // aloca estruturas internas de acordo com a representação
    void initMatriz();
    void initLista();

public:
    // Constrói um grafo com 'numVertices' vértices.
    // - 'rep' define a representação (MATRIZ/ LISTA).
    // - se ponderado=true, a terceira coluna do arquivo de entrada é usada como peso.
    Grafo(int numVertices, TipoRepresentacao rep, bool ponderado = false);
    ~Grafo();

    // Lê a estrutura do grafo de arquivo texto.
    // Formato esperado:
    //   primeira linha: n (número de vértices, deve bater com o do construtor)
    //   demais linhas: u v [peso]
    // Onde u e v estão em 1..n (ajustados internamente para 0..n-1).
    // Se o grafo não for ponderado, o peso é ignorado e assume-se 1.0.
    bool lerDeArquivo(const char *nomeArquivo);

    // Escreve em arquivo informações gerais do grafo:
    // |V|, |E|, grau médio e a distribuição empírica de graus (grau:frequência).
    bool escreverInfo(const char *nomeSaida) const;

    // Busca em Largura (BFS) a partir de 'raiz' (1..n), gerando pai e nível de cada vértice.
    // O arquivo de saída possui linhas: vertice:pai:nivel (pai=0 para raiz ou não alcançado).
    bool buscaLargura(int raiz, const char *nomeSaida) const;
    // Busca em Profundidade (DFS) a partir de 'raiz' (1..n), gerando pai e nível de descoberta.
    bool buscaProfundidade(int raiz, const char *nomeSaida) const;

    // Computa as componentes conexas (em grafo não-direcionado) e grava tamanho e vértices de cada.
    bool componentesConexas(const char *nomeSaida) const;

    // Caminho mínimo (Dijkstra se ponderado; BFS se pesos=1) para um destino específico.
    bool caminhoMinimo(int origem, int destino, const char *nomeSaida) const;
    // Caminhos mínimos de 'origem' para todos os vértices (1..n). Saída: id:dist:caminho
    bool caminhosMinimos(int origem, const char *nomeSaida) const;

    // Exemplo/placeholder: imprime "vasco" na tela (não implementado neste arquivo fonte).
    void imprimirVasco() const;

private:
    // Insere aresta não-direcionada u<->v, com 'peso'. Evita duplicatas.
    void adicionaAresta(int u, int v, double peso);
    // Retorna o grau (número de vizinhos) do vértice u (0..n-1).
    int grauVertice(int u) const;
    // Rotina recursiva de DFS que preenche vetores 'pai' e 'nivel'.
    void dfsRec(int u, bool *vis, int *pai, int *nivel) const;
};

#endif // GRAPH_HPP
