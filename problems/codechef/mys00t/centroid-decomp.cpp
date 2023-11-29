#include <stdio.h>

const int MAX_NODES = 200'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int size;
  bool dead;
};

cell list[2 * MAX_NODES];
node n[MAX_NODES + 1];
int num_nodes, list_ptr;

void add_child(int u, int v) {
  list[list_ptr] = { v, n[u].adj };
  n[u].adj = list_ptr++;
}

void read_tree() {
  scanf("%d", &num_nodes);

  // wipe adjacency lists
  for (int u = 1; u <= num_nodes; u++) {
    n[u].adj = 0;
    n[u].dead = false;
  }
  list_ptr = 1;

  for (int i = 1; i < num_nodes; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_child(u, v);
    add_child(v, u);
  }
}

void size_dfs(int u, int parent) {
  n[u].size = 1;

  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if ((v != parent) && !n[v].dead) {
      size_dfs(v, u);
      n[u].size += n[v].size;
    }
  }
}

int get_heavy_child(int u, int parent, int limit) {
  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if ((v != parent) && !n[v].dead && (n[v].size > limit)) {
      return v;
    }
  }
  return 0;
}

int find_centroid(int u) {
  int size_limit = n[u].size / 2;
  int parent = 0;
  int child;

  while ((child = get_heavy_child(u, parent, size_limit)) != 0) {
    parent = u;
    u = child;
  }

  return u;
}

void solve(int u) {
  int prev = 0;

  while (u != -1) {
    size_dfs(u, 0);
    u = find_centroid(u);

    printf("1 %d\n", u);
    fflush(stdout);

    n[u].dead = true;
    prev = u;
    scanf("%d", &u);
  }

  printf("2 %d\n", prev);
}

int main() {
  int num_tests, check = 1;
  scanf("%d", &num_tests);

  while (num_tests-- && (check == 1)) {
    read_tree();
    solve(1);
    scanf("%d", &check);
  }

  return 0;
}
