#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_EDGES = 200'000;

struct edge {
  int u, v;
  bool used;

  int other(int z) {
    return (z == u) ? v : u;
  }
};

struct cell {
  int ind;
  int next;
};

struct node {
  int adj;
  int degree;
};

node n[MAX_NODES + 1];
edge e[MAX_EDGES + 1];
cell list[2 * MAX_EDGES + 1];
int st[MAX_EDGES + 1];
int sol[MAX_EDGES + 1];
int num_nodes, num_edges, num_odd_vertices;

void add_edge(int u, int ind) {
  static int ptr = 0;

  list[++ptr] = { ind, n[u].adj };
  n[u].adj = ptr;
  n[u].degree++;
  num_odd_vertices += (n[u].degree % 2) ? +1 : -1;
}

void read_graph() {
  scanf("%d %d", &num_nodes, &num_edges);
  for (int i = 1; i <= num_edges; i++) {
    scanf("%d %d", &e[i].u, &e[i].v);
    add_edge(e[i].u, i);
    add_edge(e[i].v, i);
  }
}

int find_unused_edge(int u) {
  int ptr = n[u].adj;
  while (ptr && e[list[ptr].ind].used) {
    ptr = list[ptr].next;
  }
  n[u].adj = ptr;

  return ptr ? list[ptr].ind : 0;
}

bool euler_tour() {
  if (num_odd_vertices) {
    return false;
  }

  int st_size = 1, sol_size = 0;
  st[0] = 1;

  while (st_size) {
    int u = st[st_size - 1];
    int ind = find_unused_edge(u);
    if (ind) {
      e[ind].used = 1;
      st[st_size++] = e[ind].other(u);
    } else {
      sol[sol_size++] = u;
      st_size--;
    }
  }

  return (sol_size == num_edges + 1);
  // Otherwise there are unreachable edges, meaning the graph is not connected.
}

void write_solution() {
  for (int i = 0; i <= num_edges; i++) {
    printf("%d ", sol[i]);
  }
  printf("\n");
}

int main() {
  read_graph();
  int feasible = euler_tour();
  if (feasible) {
    write_solution();
  } else {
    printf("IMPOSSIBLE\n");
  }

  return 0;
}
