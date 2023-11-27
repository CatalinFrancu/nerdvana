#include <stdio.h>

const int MAX_NODES = 200'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int size;
};

cell list[2 * MAX_NODES];
node n[MAX_NODES + 1];
int num_nodes;

void add_child(int u, int v) {
  static int pos = 1;
  list[pos] = { v, n[u].adj };
  n[u].adj = pos++;
}

void read_input_data() {
  scanf("%d", &num_nodes);

  for (int i = 1; i < num_nodes; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_child(u, v);
    add_child(v, u);
  }
}

void size_dfs(int u) {
  n[u].size = 1;

  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!n[v].size) {
      size_dfs(v);
      n[u].size += n[v].size;
    }
  }
}

int get_heavy_child(int u, int parent) {
  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if ((v != parent) && (n[v].size > num_nodes / 2)) {
      return v;
    }
  }
  return 0;
}

void find_centroid() {
  int u = 1, parent = 0, child;

  while ((child = get_heavy_child(u, parent)) != 0) {
    parent = u;
    u = child;
  }

  printf("%d\n", u);
}

int main() {
  read_input_data();
  size_dfs(1);
  find_centroid();

  return 0;
}
