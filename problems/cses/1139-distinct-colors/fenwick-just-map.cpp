#include <stdio.h>
#include <unordered_map>

const int MAX_NODES = 200'000;
const int NIL = -1;

struct list {
  int val, next;
};

struct node {
  int color, distinct;
  int start;
  int ptr; // pointer into the adjacency list
};

// A Fenwick tree extended with colors. When we colorize a bit with a given
// color, the tree automatically clears the previous occurrence of the color.
struct fenwick_tree {
  int v[MAX_NODES + 1];
  std::unordered_map<int, int> last_pos;
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
    auto last = last_pos.find(color);
    if (last != last_pos.end()) {
      add(last->second, -1);
      last->second = pos;
    } else {
      last_pos[color] = pos;
    }
    add(pos, +1);
  }

  // count[pos..n] = total - count[1..pos-1]
  int suffix_count(int pos) {
    int s = total;
    pos--;
    while (pos) {
      s -= v[pos];
      pos &= pos - 1;
    }
    return s;
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

  // Note: we could reuse the .color field to save memory.
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
  fen.init(num_nodes);
  dfs(1);
  write_output_data();

  return 0;
}
