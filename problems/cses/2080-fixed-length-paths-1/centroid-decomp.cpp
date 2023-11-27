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
int freq[MAX_NODES];
int num_nodes, length;
long long answer;

void add_child(int u, int v) {
  static int pos = 1;
  list[pos] = { v, n[u].adj };
  n[u].adj = pos++;
}

void read_input_data() {
  scanf("%d %d", &num_nodes, &length);

  for (int i = 1; i < num_nodes; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_child(u, v);
    add_child(v, u);
  }
}

/**
 * Following is the problem we are actually trying to solve.
 **/

int max(int x, int y) {
  return (x > y) ? x : y;
}

int max_depth;

// Counts the paths of length k starting from u, going up to the current
// centroid and down into another subtree.
void count_dfs(int u, int parent, int depth) {
  max_depth = max(max_depth, depth);
  answer += freq[length - depth];

  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if ((!n[v].dead) && (v != parent) && (depth < length)) {
      count_dfs(v, u, depth + 1);
    }
  }
}

// Marks the depth of every node.
void mark_dfs(int u, int parent, int depth) {
  freq[depth]++;

  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if ((!n[v].dead) && (v != parent) && (depth < length)) {
      mark_dfs(v, u, depth + 1);
    }
  }
}

// Counts the paths of length k that go trough u.
void count_paths_through(int u) {
  freq[0] = 1; // node u itself
  max_depth = 0;

  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!n[v].dead) {
      count_dfs(v, u, 1);
      mark_dfs(v, u, 1);
    }
  }

  for (int d = 0; d <= max_depth; d++) {
    freq[d] = 0;
  }
}

/**
 * The rest of the code (decompose(), size_dfs(), find_centroid() and
 * get_heavy_child()) is boilerplate. We use it because it allow us to run a
 * DFS from every centroid and still achieve O(n log n) time.
 **/

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

void decompose(int u) {
  size_dfs(u, 0);
  u = find_centroid(u);

  // This is what we are actually trying to solve.
  count_paths_through(u);

  n[u].dead = true;
  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!n[v].dead) {
      decompose(v);
    }
  }
}

void write_answer() {
  printf("%lld\n", answer);
}

int main() {
  read_input_data();
  decompose(1);
  write_answer();

  return 0;
}
