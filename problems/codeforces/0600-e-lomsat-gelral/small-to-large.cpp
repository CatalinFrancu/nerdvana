#include <stdio.h>
#include <unordered_map>

const int MAX_NODES = 100'000;

struct list {
  int val, next;
};

struct node {
  std::unordered_map<int, int> map; // Maps colors to frequencies
  int color;
  int max_f;                        // Highest frequency in map
  long long sum;                    // Sum of colors having highest frequency
  int ptr;                          // Pointer into the adjacency list

  void create_map() {
    map[color] = 1;
    max_f = 1;
    sum = color;
  }

  void swap_info(node& other) {
    map.swap(other.map);
    max_f = other.max_f;
    sum = other.sum;
  }

  void absorb_map(node& src) {
    if (src.map.size() > map.size()) {
      swap_info(src);
    }

    for (auto p: src.map) {
      int color = p.first, f = p.second;
      map[color] += f;
      if (map[color] > max_f) {
        max_f = map[color];
        sum = color;
      } else if (map[color] == max_f) {
        sum += color;
      }
    }

    src.map.clear();
  }
};

list adj[2 * MAX_NODES];
node n[MAX_NODES + 1];
int num_nodes;

void add_neighbor(int u, int v) {
  static int ptr = 1;
  adj[ptr] = { v, n[u].ptr };
  n[u].ptr = ptr++;
}

void read_input_data() {
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

void dfs(int u) {

  n[u].create_map();

  for (int ptr = n[u].ptr; ptr; ptr = adj[ptr].next) {
    int v = adj[ptr].val;
    if (!n[v].map.size()) {
      dfs(v);
      n[u].absorb_map(n[v]);
    }
  }
}

void write_output_data() {
  for (int u = 1; u <= num_nodes; u++) {
    printf("%lld ", n[u].sum);
  }
  printf("\n");
}

int main() {
  read_input_data();
  dfs(1);
  write_output_data();

  return 0;
}
