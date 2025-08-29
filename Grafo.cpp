#include "Grafo.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>
#include <queue>
#include <vector>
#include <utility>

using std::vector;
using std::priority_queue;
using std::pair;

Grafo::Grafo(int numVertices, TipoRepresentacao rep, bool ponderado)
: n(numVertices), m(0), tipo(rep), ponderado(ponderado),
  matrizAdj(nullptr), matrizPeso(nullptr), listaAdj(nullptr), listaPeso(nullptr), grauAdj(nullptr)
{
    if (tipo == MATRIZ) initMatriz();
    else               initLista();
}

Grafo::~Grafo() {
    if (tipo == MATRIZ) {
        for (int i = 0; i < n; i++) {
            delete[] matrizAdj[i];
            delete[] matrizPeso[i];
        }
        delete[] matrizAdj;
        delete[] matrizPeso;
    } else {
        for (int i = 0; i < n; i++) {
            delete[] listaAdj[i];
            delete[] listaPeso[i];
        }
        delete[] listaAdj;
        delete[] listaPeso;
        delete[] grauAdj;
    }
}

void Grafo::initMatriz() {
    matrizAdj  = new int*[n];
    matrizPeso = new double*[n];
    for (int i = 0; i < n; i++) {
        matrizAdj[i]  = new int[n]();
        matrizPeso[i] = new double[n];
        for (int j = 0; j < n; j++)
            matrizPeso[i][j] = std::numeric_limits<double>::infinity();
    }
}

void Grafo::initLista() {
    listaAdj   = new int*[n];
    listaPeso  = new double*[n];
    grauAdj    = new int[n]();
    for (int i = 0; i < n; i++) {
        listaAdj[i]  = new int[n];
        listaPeso[i] = new double[n];
    }
}

bool Grafo::lerDeArquivo(const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) return false;

    int nv;
    if (fscanf(f, "%d", &nv) != 1 || nv != n) {
        fclose(f);
        return false;
    }
    char linha[256];
    fgets(linha, sizeof(linha), f);

    while (fgets(linha, sizeof(linha), f)) {
        int u, v;
        double w = 1.0;
        int cnt = sscanf(linha, "%d %d %lf", &u, &v, &w);
        if (cnt >= 2) {
            u--; v--;
            if (u >= 0 && u < n && v >= 0 && v < n)
                adicionaAresta(u, v, ponderado ? w : 1.0);
        }
    }
    fclose(f);
    return true;
}

void Grafo::adicionaAresta(int u, int v, double peso) {
    if (tipo == MATRIZ) {
        if (matrizAdj[u][v] == 0) {
            matrizAdj[u][v] = matrizAdj[v][u] = 1;
            matrizPeso[u][v] = matrizPeso[v][u] = peso;
            m++;
        }
    } else {
        bool existe = false;
        for (int i = 0; i < grauAdj[u]; i++)
            if (listaAdj[u][i] == v) { existe = true; break; }
        if (!existe) {
            listaAdj[u][ grauAdj[u] ] = v;
            listaPeso[u][ grauAdj[u] ] = peso;
            grauAdj[u]++;
            listaAdj[v][ grauAdj[v] ] = u;
            listaPeso[v][ grauAdj[v] ] = peso;
            grauAdj[v]++;
            m++;
        }
    }
}

int Grafo::grauVertice(int u) const {
    if (tipo == MATRIZ) {
        int g = 0;
        for (int v = 0; v < n; v++) g += matrizAdj[u][v];
        return g;
    }
    return grauAdj[u];
}

bool Grafo::escreverInfo(const char *nomeSaida) const {
    FILE *f = fopen(nomeSaida, "w");
    if (!f) return false;
    fprintf(f, "Número de vértices: %d\n", n);
    fprintf(f, "Número de arestas:   %d\n", m);
    double soma = 0;
    for (int i = 0; i < n; i++) soma += grauVertice(i);
    fprintf(f, "Grau médio:          %.2f\n", soma / n);
    int *cont = new int[n]();
    for (int i = 0; i < n; i++) cont[ grauVertice(i) ]++;
    fprintf(f, "Distribuição (grau:freq):\n");
    for (int d = 0; d < n; d++)
        if (cont[d] > 0) fprintf(f, "  %d:%d\n", d, cont[d]);
    delete[] cont;
    fclose(f);
    return true;
}

bool Grafo::buscaLargura(int raiz1, const char *nomeSaida) const {
    int raiz = raiz1 - 1;
    bool *vis   = new bool[n]();
    int  *pai   = new int[n];
    int  *nivel = new int[n];
    int  *fila  = new int[n];
    int ini = 0, fim = 0;

    for (int i = 0; i < n; i++) {
        pai[i]   = -1;
        nivel[i] = -1;
        // vis[i] já é false por new bool[n]()
    }

    vis[raiz]   = true;
    pai[raiz]   = -1;
    nivel[raiz] = 0;
    fila[fim++] = raiz;

    while (ini < fim) {
        int u = fila[ini++];
        if (tipo == MATRIZ) {
            for (int v = 0; v < n; v++) {
                if (matrizAdj[u][v] && !vis[v]) {
                    vis[v]       = true;
                    pai[v]       = u;
                    nivel[v]     = nivel[u] + 1;
                    fila[fim++] = v;
                }
            }
        } else {
            for (int k = 0; k < grauAdj[u]; k++) {
                int v = listaAdj[u][k];
                if (!vis[v]) {
                    vis[v]       = true;
                    pai[v]       = u;
                    nivel[v]     = nivel[u] + 1;
                    fila[fim++] = v;
                }
            }
        }
    }

    FILE *f = fopen(nomeSaida, "w");
    if (!f) return false;
    fprintf(f, "vertice:pai:nivel\n");
    for (int i = 0; i < n; i++) {
        int paiExibe   = (pai[i] < 0 ? 0 : pai[i] + 1);
        int nivelExibe = nivel[i];
        fprintf(f, "%d:%d:%d\n", i + 1, paiExibe, nivelExibe);
    }
    fclose(f);

    delete[] vis;
    delete[] pai;
    delete[] nivel;
    delete[] fila;
    return true;
}

void Grafo::dfsRec(int u, bool *vis, int *pai, int *nivel) const {
    vis[u] = true;
    if (tipo == MATRIZ) {
        for (int v = 0; v < n; v++)
            if (matrizAdj[u][v] && !vis[v]) {
                pai[v] = u; nivel[v] = nivel[u] + 1;
                dfsRec(v, vis, pai, nivel);
            }
    } else {
        for (int i = 0; i < grauAdj[u]; i++) {
            int v = listaAdj[u][i];
            if (!vis[v]) {
                pai[v] = u; nivel[v] = nivel[u] + 1;
                dfsRec(v, vis, pai, nivel);
            }
        }
    }
}

bool Grafo::buscaProfundidade(int raiz1, const char *nomeSaida) const {
    int raiz = raiz1 - 1;
    bool *vis = new bool[n]();
    int *pai = new int[n];
    int *nivel = new int[n];
    for (int i = 0; i < n; i++) 
    { 
        pai[i] = -1; nivel[i] = -1; 
    }
    pai[raiz] = -1; nivel[raiz] = 0;
    dfsRec(raiz, vis, pai, nivel);
    FILE *f = fopen(nomeSaida, "w");
    
    if (!f) return false;
    fprintf(f, "vertice:pai:nivel\n");
    
    for (int i = 0; i < n; i++)
        fprintf(f, "%d:%d:%d\n", i+1, (pai[i]<0?0:pai[i]+1), nivel[i]);
    fclose(f);
    delete[] vis; 
    delete[] pai; 
    delete[] nivel;
    
    return true;
}

bool Grafo::componentesConexas(const char *nomeSaida) const {
    bool *vis = new bool[n]();
    int **compVerts = new int*[n];
    int *compSizes = new int[n];
    int qtd = 0;
    int *fila = new int[n];

    for (int i = 0; i < n; i++) {
        if (!vis[i]) {
            int ini = 0, fim = 0, cnt = 0;
            fila[fim++] = i; vis[i] = true;
            int *thisComp = new int[n];
            while (ini < fim) {
                int u = fila[ini++]; thisComp[cnt++] = u;
                if (tipo == MATRIZ) {
                    for (int v = 0; v < n; v++)
                        if (matrizAdj[u][v] && !vis[v]) {
                            vis[v] = true; fila[fim++] = v;
                        }
                } else {
                    for (int k = 0; k < grauAdj[u]; k++) {
                        int v = listaAdj[u][k];
                        if (!vis[v]) { vis[v] = true; fila[fim++] = v; }
                    }
                }
            }
            compVerts[qtd] = thisComp;
            compSizes[qtd] = cnt;
            qtd++;
        }
    }
    delete[] fila; delete[] vis;
    for (int i = 0; i < qtd - 1; i++) {
        for (int j = 0; j < qtd - 1 - i; j++) {
            if (compSizes[j] < compSizes[j + 1]) {
                std::swap(compSizes[j], compSizes[j + 1]);
                std::swap(compVerts[j], compVerts[j + 1]);
            }
        }
    }
    FILE *f = fopen(nomeSaida, "w");
    if (!f) return false;
    fprintf(f, "Número de componentes: %d\n", qtd);
    fprintf(f, "Componentes (tamanho e vértices em cada):\n");
    for (int i = 0; i < qtd; i++) {
        fprintf(f, "  comp %d (tamanho %d):", i+1, compSizes[i]);
        for (int k = 0; k < compSizes[i]; k++)
            fprintf(f, " %d", compVerts[i][k] + 1);
        fprintf(f, "\n");
        delete[] compVerts[i];
    }
    fclose(f);
    delete[] compVerts; delete[] compSizes;
    return true;
}

bool Grafo::caminhosMinimos(int origem1, const char *nomeSaida) const {
    int origem = origem1 - 1;
    const double INF = std::numeric_limits<double>::infinity();
    vector<double> dist(n, INF);
    vector<int> pai(n, -1);
    struct Node { double d; int u; };
    struct Cmp { bool operator()(const Node &a, const Node &b) const { return a.d > b.d; } };
    priority_queue<Node, vector<Node>, Cmp> pq;

    dist[origem] = 0;
    pq.push({0, origem});
    while (!pq.empty()) {
        Node top = pq.top(); pq.pop();
        int u = top.u; double d = top.d;
        if (d > dist[u]) continue;
        if (tipo == MATRIZ) {
            for (int v = 0; v < n; v++) {
                if (matrizAdj[u][v]) {
                    double w = matrizPeso[u][v];
                    if (dist[v] > dist[u] + w) {
                        dist[v] = dist[u] + w; pai[v] = u;
                        pq.push({dist[v], v});
                    }
                }
            }
        } else {
            for (int i = 0; i < grauAdj[u]; i++) {
                int v = listaAdj[u][i]; double w = listaPeso[u][i];
                if (dist[v] > dist[u] + w) {
                    dist[v] = dist[u] + w; pai[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }
    }
    FILE *f = fopen(nomeSaida, "w"); if (!f) return false;
    for (int i = 0; i < n; i++) {
        fprintf(f, "%d:%.2f:", i+1, dist[i]);
        if (dist[i] < INF) {
            vector<int> path;
            for (int v = i; v != -1; v = pai[v]) path.push_back(v);
            for (auto it = path.rbegin(); it != path.rend(); ++it) {
                fprintf(f, "%d", *it + 1);
                if (it + 1 != path.rend()) fprintf(f, "-");
            }
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return true;
}

bool Grafo::caminhoMinimo(int origem, int destino, const char *nomeSaida) const {
    // calcula todos e filtra apenas destino
    const bool ok = caminhosMinimos(origem, nomeSaida);
    // o arquivo já contém linhas para todos; cabe ao usuário extrair a desejada.
    return ok;
}
