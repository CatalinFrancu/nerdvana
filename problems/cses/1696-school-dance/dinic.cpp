// Method: Particularization of Dinic's algorithm to bipartite graphs.
#include <queue>
#include <stdio.h>

const int MAX_NODES = 2 * 500;
const int MAX_EDGES = 1'000;
const int NONE = -1;

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

edge e[2 * (MAX_EDGES + MAX_NODES)];
node n[MAX_NODES + 3];
int num_l, num_r, num_nodes, num_edges;
int source, sink;

void add_edge(short u, short v, int cap) {
  static int pos = 0;

  e[pos] = { cap, v, n[u].adj };
  n[u].adj = pos++;
}

void add_edge_pair(short u, short v) {
  add_edge(u, v, 1);
  add_edge(v, u, 0);
}

void read_data() {
  int m;
  scanf("%d %d %d", &num_l, &num_r, &m);
  num_nodes = num_l + num_r + 2;
  source = num_nodes - 1;
  sink = num_nodes;

  for (int u = 1; u <= num_nodes; u++) {
    n[u].adj = NONE;
  }
  for (int u = 1; u <= num_l; u++) {
    add_edge_pair(source, u);
  }
  for (int v = 1; v <= num_r; v++) {
    add_edge_pair(v + num_l, sink);
  }
  while (m--) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge_pair(u, v + num_l);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

bool bfs_reachable() {
  for (int u = 1; u <= num_nodes; u++) {
    n[u].dist = NONE;
  }
  n[source].dist = 0;
  std::queue<short> q;
  q.push(source);

  while (!q.empty() && !n[sink].is_reachable()) {
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

  return n[sink].is_reachable();
}

void reset_dfs_pointers() {
  for (int u = 1; u <= num_nodes; u++) {
    n[u].ptr = n[u].adj;
  }
}

bool dfs(int u) {
  if (u == sink) {
    return true;
  }

  bool found = 0;
  while (!found && (n[u].ptr != NONE)) {
    int p = n[u].ptr;
    int v = e[p].v, cap = e[p].cap;
    if (cap && (n[v].dist == n[u].dist + 1)) {
      found = dfs(v);
      e[p].cap -= found;
      e[p ^ 1].cap += found;
    }
    // Either the edge is unusable or it has become saturated. In either case,
    // delete it.
    n[u].ptr = e[p].next;
  }

  return found;
}

int dinic() {
  int flow = 0;
  while (bfs_reachable()) {
    reset_dfs_pointers();
    while (dfs(source)) {
      flow++;
    }
  }

  return flow;
}

void recover_matching() {
  for (int u = 1; u <= num_l; u++) {
    for (int pos = n[u].adj; pos != NONE; pos = e[pos].next) {
      int v = e[pos].v;
      if ((v != source) && !e[pos].cap) {
        printf("%d %d\n", u, v - num_l);
      }
    }
  }
}

int main() {
  read_data();
  int max_flow = dinic();
  printf("%d\n", max_flow);
  recover_matching();

  return 0;
}
