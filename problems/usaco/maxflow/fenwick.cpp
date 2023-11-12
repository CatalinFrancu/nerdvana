#include <algorithm>
#include <stdio.h>

const int MAX_NODES = 50000;
const int MAX_PATHS = 100000;
const int NIL = -1;

struct list {
  int val, next;
};

struct node {
  int start, finish;
  int ptr; // pointer into the adjacency list
};

struct path {
  int u, v;
};

struct fenwick_tree {
  unsigned short v[MAX_NODES + 1];
  int n;

  void init(int n) {
    this->n = n;
  }

  void add(int pos, int val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  void inc(int pos) {
    add(pos, +1);
  }

  void dec(int pos) {
    add(pos, -1);
  }

  int count(int pos) {
    int s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }

  int range_count(int x, int y) {
    return count(y) - count(x - 1);
  }
};

list adj[2 * MAX_NODES];
list path_list[2 * MAX_PATHS];
node n[MAX_NODES + 1];
path by_start[MAX_PATHS], by_finish[MAX_PATHS];
fenwick_tree active_start, active_finish;
int num_nodes, num_paths;

void add_neighbor(int u, int v) {
  static int ptr = 0;
  adj[ptr] = { v, n[u].ptr };
  n[u].ptr = ptr++;
}

void read_input_data() {
  FILE* f = fopen("maxflow.in", "r");
  fscanf(f, "%d %d", &num_nodes, &num_paths);
  for (int u = 1; u <= num_nodes; u++) {
    n[u].ptr = NIL;
  }

  for (int i = 0; i < num_nodes - 1; i++) {
    int u, v;
    fscanf(f, "%d %d", &u, &v);
    add_neighbor(u, v);
    add_neighbor(v, u);
  }

  for (int i = 0; i < num_paths; i++) {
    fscanf(f, "%d %d", &by_start[i].u, &by_start[i].v);
  }
  fclose(f);
}

void compute_ranges(int u) {
  static int time = 0;

  n[u].start = ++time;

  for (int ptr = n[u].ptr; ptr != NIL; ptr = adj[ptr].next) {
    int v = adj[ptr].val;
    if (!n[v].start) {
      compute_ranges(v);
    }
  }

  n[u].finish = time;
}

void sort_paths() {
  for (int i = 0; i < num_paths; i++) {
    path& p = by_start[i];
    if (n[p.u].start > n[p.v].start) {
      int tmp = p.u; p.u = p.v; p.v = tmp;
    }
    by_finish[i] = p;
  }

  std::sort(by_start, by_start + num_paths, [](path a, path b) {
    return n[a.u].start < n[b.u].start;
  });
  std::sort(by_finish, by_finish + num_paths, [](path a, path b) {
    return
      (n[a.v].finish < n[b.v].finish) ||
      ((n[a.v].finish == n[b.v].finish) &&
       (n[a.v].start > n[b.v].start));
  });
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

int process_paths_starting_at(int u) {
  static int pos = 0;

  int orig = pos;
  while (by_start[pos].u == u) {
    int v = by_start[pos].v;
    active_start.inc(n[u].start);
    active_finish.inc(n[v].start);
    pos++;
  }
  return pos - orig;
}

void process_paths_ending_at(int v) {
  static int pos = 0;

  while (by_finish[pos].v == v) {
    int u = by_finish[pos].u;
    active_start.dec(n[u].start);
    active_finish.dec(n[v].start);
    pos++;
  }
}

int max_load = 0;

void compute_load(int u) {
  // Paths that terminate in u's subtree.
  int load = active_finish.range_count(n[u].start, n[u].finish);

  // Paths that originate at u.
  load += process_paths_starting_at(u);

  for (int ptr = n[u].ptr; ptr != NIL; ptr = adj[ptr].next) {
    int v = adj[ptr].val;
    if (n[v].start > n[u].start) {
      compute_load(v);
      // Path that originated in a descendant of u and will terminate either
      // in another one of u's children or outside of u's subtree.
      load += active_start.range_count(n[v].start, n[v].finish);
    }
  }

  process_paths_ending_at(u);
  max_load = max(max_load, load);
}

void write_answer() {
  FILE* f = fopen("maxflow.out", "w");
  fprintf(f, "%d\n", max_load);
  fclose(f);
}

int main() {
  read_input_data();
  compute_ranges(1);
  sort_paths();

  active_start.init(num_nodes);
  active_finish.init(num_nodes);

  compute_load(1);
  write_answer();

  return 0;
}
