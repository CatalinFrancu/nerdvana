// Method: Dinic's algorithm.
#include <queue>
#include <stdio.h>

const int MAX_NODES = 500;
const int MAX_EDGES = 1'000;
const int NONE = -1;
const int INFINITY = 1'000'000'001;

struct edge {
  int cap;
  short v, next;
};

struct node {
  short adj;
  short ptr;
  short dist;

  bool is_reachable() {
    return dist != NONE;
  }
};

edge e[2 * MAX_EDGES];
node n[MAX_NODES + 1];
int num_nodes, num_edges;

void add_edge(short u, short v, int cap) {
  static int pos = 0;

  e[pos] = { cap, v, n[u].adj };
  n[u].adj = pos++;
}

void read_data() {
  scanf("%d %d", &num_nodes, &num_edges);
  for (int u = 1; u <= num_nodes; u++) {
    n[u].adj = NONE;
  }
  for (int i = 0; i < num_edges; i++) {
    int u, v, c;
    scanf("%d %d %d", &u, &v, &c);
    add_edge(u, v, c);
    add_edge(v, u, 0);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

bool bfs_reachable() {
  for (int u = 1; u <= num_nodes; u++) {
    n[u].dist = NONE;
  }
  n[1].dist = 0;
  std::queue<short> q;
  q.push(1);

  while (!q.empty() && !n[num_nodes].is_reachable()) {
    int u = q.front();
    q.pop();
    for (int pos = n[u].adj; pos != NONE; pos = e[pos].next) {
      int v = e[pos].v;
      if (!n[v].is_reachable() && e[pos].cap) {
        n[v].dist = n[u].dist + 1;
        q.push(v);
      }
    }
  }

  return n[num_nodes].is_reachable();
}

void reset_dfs_pointers() {
  for (int u = 1; u <= num_nodes; u++) {
    n[u].ptr = n[u].adj;
  }
}

int dfs(int u, int incoming_flow) {
  if (u == num_nodes) {
    return incoming_flow;
  }

  int result = 0;
  while (!result && (n[u].ptr != NONE)) {
    int p = n[u].ptr;
    int v = e[p].v, cap = e[p].cap;
    if (cap && (n[v].dist == n[u].dist + 1)) {
      result = dfs(v, min(cap, incoming_flow));
      e[p].cap -= result;
      e[p ^ 1].cap += result;
    }
    // Stay on the same edge if still usable. This cuts the running time in 6.
    if (!result) {
      n[u].ptr = e[p].next;
    }
  }

  return result;
}

long long dinic() {
  long long flow = 0;

  while (bfs_reachable()) {
    reset_dfs_pointers();
    int augment;
    while ((augment = dfs(1, INFINITY))) {
      flow += augment;
    }
  }

  return flow;
}

int main() {
  read_data();
  long long max_flow = dinic();
  printf("%lld\n", max_flow);

  return 0;
}