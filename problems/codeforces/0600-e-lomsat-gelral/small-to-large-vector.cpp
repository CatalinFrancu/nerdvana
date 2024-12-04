#include <stdio.h>
#include <unordered_map>
#include <vector>

const int MAX_NODES = 100'000;

struct node {
  std::unordered_map<int, int> map; // Maps colors to frequencies
  std::vector<int> adj;
  int color;
  int max_f;                        // Highest frequency in map                 
  long long sum;                    // Sum of colors having highest frequency

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

node n[MAX_NODES + 1];
int num_nodes;

void read_input_data() {
  scanf("%d", &num_nodes);

  for (int u = 1; u <= num_nodes; u++) {
    scanf("%d", &n[u].color);
  }

  for (int i = 0; i < num_nodes - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    n[u].adj.push_back(v);
    n[v].adj.push_back(u);
  }
}

void dfs(int u) {

  n[u].create_map();

  for (auto v: n[u].adj) {
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
