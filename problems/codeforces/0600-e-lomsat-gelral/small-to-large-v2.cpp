// Small-to-large merging, v2: return freq_info objects instead of storing
// them on every node.
#include <stdio.h>
#include <unordered_map>

const int MAX_NODES = 100'000;

struct freq_info {
  std::unordered_map<int, int> map; // colors => frequencies
  int max_f;                        // highest frequency in map
  long long sum;                    // sum of colors having max_f

  freq_info(int color) {
    map[color] = 1;
    max_f = 1;
    sum = color;
  }

  void swap(freq_info& other) {
    map.swap(other.map);
    max_f = other.max_f;
    sum = other.sum;
    // Technically, we should also copy our values to other, but it's not
    // necessary.
  }

  void absorb(freq_info& src) {
    if (src.map.size() > map.size()) {
      swap(src);
    }

    for (auto [color, f]: src.map) {
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

struct cell {
  int val, next;
};

struct node {
  int color;
  int adj;
  long long sum;
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
int n;

void add_neighbor(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_input_data() {
  scanf("%d", &n);

  for (int u = 1; u <= n; u++) {
    scanf("%d", &nd[u].color);
  }

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_neighbor(u, v);
    add_neighbor(v, u);
  }
}

freq_info dfs(int u) {
  freq_info result(nd[u].color);
  nd[u].color = 0; // prevent infinite recursion

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].val;
    if (nd[v].color) {
      freq_info f = dfs(v);
      result.absorb(f);
    }
  }

  nd[u].sum = result.sum;
  return result;
}

void write_output_data() {
  for (int u = 1; u <= n; u++) {
    printf("%lld ", nd[u].sum);
  }
  printf("\n");
}

int main() {
  read_input_data();
  dfs(1);
  write_output_data();

  return 0;
}
