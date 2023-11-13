#include <stdio.h>
#include <unordered_map>

const int MAX_NODES = 200'000;
const int NIL = -1;

struct list {
  int val, next;
};

struct node {
  int color, distinct;
  int start, finish;
  int ptr; // pointer into the adjacency list
};

// A Fenwick tree extended with colors. When we colorize a bit with a given
// color, the tree automatically clears the previous occurrence of the color.
struct fenwick_tree {
  int v[MAX_NODES + 1];
  int last_pos[MAX_NODES + 1];
  int n, total;

  void init(int n) {
    this->n = n;
    total = 0;
  }

  void add(int pos, int val) {
    total += val;
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  void colorize(int pos, int color) {
    int last = last_pos[color];
    if (last) {
      add(last, -1);
    }
    last_pos[color] = pos;
    add(pos, +1);
  }

  int prefix_count(int pos) {
    int s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }

  int suffix_count(int pos) {
    return total - prefix_count(pos - 1);
  }
};

list adj[2 * MAX_NODES];
node n[MAX_NODES + 1];
fenwick_tree fen;
int num_nodes;

void add_neighbor(int u, int v) {
  static int ptr = 0;
  adj[ptr] = { v, n[u].ptr };
  n[u].ptr = ptr++;
}

void read_input_data() {
  scanf("%d", &num_nodes);

  for (int u = 1; u <= num_nodes; u++) {
    scanf("%d", &n[u].color);
    n[u].ptr = NIL;
  }

  for (int i = 0; i < num_nodes - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_neighbor(u, v);
    add_neighbor(v, u);
  }
}

void normalize_colors() {
  std::unordered_map<int, int> color_map;
  int num_colors = 0;

  for (int u = 1; u <= num_nodes; u++) {
    auto search = color_map.find(n[u].color);
    if (search == color_map.end()) {
      color_map[n[u].color] = ++num_colors;
      n[u].color = num_colors;
    } else {
      n[u].color = search->second;
    }
  }
}

void dfs(int u) {
  static int time = 0;

  n[u].start = ++time;
  fen.colorize(time, n[u].color);

  for (int ptr = n[u].ptr; ptr != NIL; ptr = adj[ptr].next) {
    int v = adj[ptr].val;
    if (!n[v].start) {
      dfs(v);
    }
  }

  n[u].distinct = fen.suffix_count(n[u].start);
}

void write_output_data() {
  for (int u = 1; u <= num_nodes; u++) {
    printf("%d ", n[u].distinct);
  }
  printf("\n");
}

int main() {
  read_input_data();
  normalize_colors();
  fen.init(num_nodes);
  dfs(1);
  write_output_data();

  return 0;
}
