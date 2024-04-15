// Complexitate: O(M + 2^D * D), unde D este numărul de noduri dense.
//
// Metodă: iterăm prin toate submulțimile de noduri dense. Pentru fiecare
// submulțime calculăm profitul maxim considerînd și restul nodurilor.
//
// Dacă eliminăm nodurile dense, rămînem doar cu niște lanțuri. Pe un lanț
// putem calcula profitul maxim. Problema este clasică: să se calculeze suma
// maximă a unui subșir care nu conține elemente consecutive. Calculăm acest
// profit în patru moduri distincte, după cum includem sau nu fiecare capăt al
// lanțului.
//
// Pot apărea cazuri particulare:
//
// * Graful este un lanț.
// * Graful este un ciclu.
// * Graful conține exact o frunză.
//
// Pentru a le trata unitar, introducem un nod suplimentar de profit zero
// (conectorul) și legăm toate frunzele de el.
#include <stdio.h>

const int MAX_NODES = 100'001; // +1 pentru conectorul de lanțuri
const int MAX_EDGES = 150'020; // +20 pentru legarea frunzelor
const int MAX_DENSE = 16; // +1 pentru nodul conector
const int NONE = -1;

struct cell {
  int v, next;
};

struct vertex {
  int adj;
  int degree;
  short profit;
  char dense_ind;
};

struct dense_vertex {
  int profit;
  bool active;

  struct {
    int profit[2][2];
    bool coexist;
  } e[MAX_DENSE];
};

cell list[2 * MAX_EDGES];
vertex node[MAX_NODES + 1];
dense_vertex d[MAX_DENSE];
int n, num_dense;
int max_profit;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, node[u].adj };
  node[u].adj = pos++;
  node[u].degree++;
}

void add_edge_pair(int u, int v) {
  add_edge(u, v);
  add_edge(v, u);
}

void read_data() {
  int m;
  FILE* f = fopen("metrou.in", "r");
  fscanf(f, "%d %d", &n, &m);
  for (int i = 1; i <= n; i++) {
    fscanf(f, "%hd", &node[i].profit);
  }
  while (m--) {
    int u, v;
    fscanf(f, "%d %d", &u, &v);
    add_edge_pair(u, v);
  }
  fclose(f);
}

void advance_chain(int& u, int& v) {
  int pos = node[v].adj;
  int w = (list[pos].v == u)
    ? list[list[pos].next].v
    : list[pos].v;

  u = v; v = w;
}

void collect_leaves() {
  int connector = 0;

  for (int u = 1; u <= n; u++) {
    if (node[u].degree == 1) {
      connector = n + 1;
      add_edge_pair(u, connector);
    }
  }
  if (connector) {
    n++;
    if (node[connector].degree == 1) { // dacă graful avea o singură frunză
      int u = connector, v = list[node[connector].adj].v;
      advance_chain(u, v);
      add_edge_pair(connector, v);
    }
  }
}

void collect_dense_nodes() {
  for (int u = 1; u <= n; u++) {
    if (node[u].degree >= 3) {
      d[num_dense].profit = node[u].profit;
      node[u].dense_ind = num_dense++;
    } else {
      node[u].dense_ind = NONE;
    }
  }

  if (!num_dense) {
    d[num_dense].profit = node[n].profit;
    node[n].dense_ind = num_dense++;
  }
}

void init_coexist() {
  for (int i = 0; i < num_dense; i++) {
    for (int j = 0; j < num_dense; j++) {
      d[i].e[j].coexist = true;
    }
  }
}

void adjacent_dense_nodes(int u, int v) {
  int du = node[u].dense_ind;
  int dv = node[v].dense_ind;
  d[du].e[dv].coexist = false;
  d[dv].e[du].coexist = false;
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

// Calculează profitul pe lanțul (u,v) exclusiv. Returnează ultimul nod
// înaintea celui dens.
int traverse_chain(int u, int v, bool with_u) {
  int du = node[u].dense_ind;
  int prev = 0, cur = with_u ? 0 : node[v].profit;

  advance_chain(u, v);
  while (node[v].dense_ind == NONE) {
    int tmp = max(node[v].profit + prev, cur);
    prev = cur;
    cur = tmp;
    advance_chain(u, v);
  }

  int dv = node[v].dense_ind;
  int* profit = d[du].e[dv].profit[with_u];
  profit[false] += cur;
  profit[true] += prev;

  return u;
}

void traverse_chain_twice(int u, int v) {
  traverse_chain(u, v, false);
  int last = traverse_chain(u, v, true);
  node[last].adj = 0; // Previno parcurgerea în sens invers a lanțului
}

void traverse_chains() {
  init_coexist();

  for (int u = 1; u <= n; u++) {
    if (node[u].dense_ind != NONE) {
      for (int ptr = node[u].adj; ptr; ptr = list[ptr].next) {
        int v = list[ptr].v;
        if (node[v].dense_ind != NONE) {
          adjacent_dense_nodes(u, v);
        } else if (node[v].adj) {
          traverse_chain_twice(u, v);
        }
      }
    }
  }
}

void gen_subsets(int k, int profit) {
  if (k == num_dense) {
    max_profit = max(max_profit, profit);
    return;
  }

  int (*self)[2] = d[k].e[k].profit;
  int profit_yes = d[k].profit + self[true][true];
  int profit_no = self[false][false];
  bool coexist = true;
  for (int i = 0; i < k; i++) {
    coexist &= d[i].e[k].coexist;
    int* from_i = d[i].e[k].profit[d[i].active];
    profit_no += from_i[false];
    profit_yes += from_i[true];
  }

  gen_subsets(k + 1, profit + profit_no);

  if (coexist) {
    d[k].active = true;
    gen_subsets(k + 1, profit + profit_yes);
    d[k].active = false;
  }
}

void write_result() {
  FILE *f = fopen("metrou.out", "w");
  fprintf(f, "%d\n", max_profit);
  fclose(f);
}

int main() {
  read_data();
  collect_leaves();
  collect_dense_nodes();
  traverse_chains();
  gen_subsets(0, 0);
  write_result();

  return 0;
}
