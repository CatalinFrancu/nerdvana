#include <stdio.h>
#include <unordered_set>

const int MAX_NODES = 200'000;

typedef std::unordered_set<int> set;

struct cell {
  int val, next;
};

struct node {
  int color, distinct;
  int adj; // pointer into the adjacency list
};

cell list[2 * MAX_NODES];
node n[MAX_NODES + 1];
int num_nodes;

void add_neighbor(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, n[u].adj };
  n[u].adj = ptr++;
}

void read_data() {
  scanf("%d", &num_nodes);

  for (int u = 1; u <= num_nodes; u++) {
    scanf("%d", &n[u].color);
  }

  for (int i = 0; i < num_nodes - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_neighbor(u, v);
    add_neighbor(v, u);
  }
}

void merge_into(set& src, set& dest) {
  dest.merge(src); // convenience method since C++17
  src.clear();
}

set dfs(int u) {
  n[u].distinct = 1; // mark it as visited to prevent infinite recursion
  set result = { n[u].color };
  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].val;
    if (!n[v].distinct) {
      set s = dfs(v);
      if (s.size() > result.size()) {
        s.swap(result);
      }
      merge_into(s, result);
    }
  }

  n[u].distinct = result.size();
  return result;
}

void write_answer() {
  for (int u = 1; u <= num_nodes; u++) {
    printf("%d ", n[u].distinct);
  }
  printf("\n");
}

int main() {
  read_data();
  dfs(1);
  write_answer();

  return 0;
}
