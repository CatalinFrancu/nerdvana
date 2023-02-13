#include <stdio.h>

#define MAX_N 500'000
#define NIL -1

struct edge {
  int v, next;
} e[2 * MAX_N];
int adj[MAX_N];                // adjacency lists
int v[MAX_N];                  // node values
int p[MAX_N];                  // parents
int first[MAX_N], last[MAX_N]; // first and last appearances in Euler tour
int euler_ptr;                 // used during the Euler tour
int fen[2 * MAX_N + 1];        // Fenwick tree over the Euler tour
int fen_size;                  // 2*n, but defined explicitly for clarity

void fenwick_build() {
  for (int i = 1; i <= fen_size; i++) {
    int j = i + (i & -i);
    if (j <= fen_size) {
      fen[j] += fen[i];
    }
  }
}

void fenwick_add(int pos, int val) {
  do {
    fen[pos] += val;
    pos += pos & -pos;
  } while (pos <= fen_size);
}

int fenwick_sum(int pos) {
  int s = 0;
  while (pos) {
    s += fen[pos];
    pos &= pos - 1;
  }
  return s;
}

void add_edge(int u, int v, int pos) {
  e[pos] = { v, adj[u] };
  adj[u] = pos;
}

// Traverse the tree; compute parents, the Euler tour and first/last
// appearances.
void dfs(int u) {
  first[u] = ++euler_ptr; // One-based, to please the Fenwick tree.
  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (v && p[v] == NIL) {
      p[v] = u;
      dfs(v);
    }
  }
  last[u] = ++euler_ptr;
}

int main() {
  int n;

  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    adj[i] = p[i] = NIL;
  }
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v, 2 * i);
    add_edge(v, u, 2 * i + 1);
  }

  p[0] = NIL;
  dfs(0);

  fen_size = 2 * n;
  for (int i = 0; i < n; i++) {
    int val;
    scanf("%d", &val);
    fen[first[i]] = val;
    fen[last[i]] = -val;
  }
  fenwick_build(); // O(n), as opposed to O(n log n) for 2n insertions.
   
  int num_queries;
  scanf("%d", &num_queries);
  while (num_queries--) {
    int query_type, u, val;
    scanf("%d %d", &query_type, &u);
    if (query_type == 1) {
      // Update node value.
      scanf("%d", &val);
      fenwick_add(first[u], +val);
      fenwick_add(last[u], -val);
    } else {
      // Print sum of nodes from u to root.
      printf("%d\n", fenwick_sum(first[u]));
    }
  }
}
