// Negate edge costs and run the Bellman-Ford algorithm to find the lowest
// negative cost. Make note of the nodes that were relaxed in the last (n-th)
// iteration. These are the nodes that have arbitrarily large scores. If the
// destination (node n) is reachable from any of these nodes, it too will have
// an arbitrarily large score. Otherwise print its score.
#include <stdio.h>
#include <vector>

const int MAX_NODES = 50'000;
const long long INFINITY = 5'000'000'000'000LL;

struct edge {
  int v;
  int cost;
};

struct node {
  std::vector<edge> adj;
  std::vector<short> rev_adj;
  long long dist;
  bool infinite;
  bool visited;
};

node nd[MAX_NODES];
int n;

void read_data() {
  int m, u, v, cost;
  scanf("%d %d", &n, &m);
  while (m--) {
    scanf("%d %d %dd", &u, &v, &cost);
    nd[u].adj.push_back({v, -cost});
    nd[v].rev_adj.push_back(u);
  }
}

bool relax_edge(int u, int v, int cost) {
  if ((nd[u].dist != INFINITY) && (nd[u].dist + cost < nd[v].dist)) {
    nd[v].dist = nd[u].dist + cost;
    return true;
  } else {
    return false;
  }
}

void bellman_ford() {
  for (int u = 1; u <= n; u++) {
    nd[u].dist = INFINITY;
  }
  nd[1].dist = 0;

  for (int stage = 1; stage <= n; stage++) {
    for (int u = 1; u <= n; u++) {
      for (edge e: nd[u].adj) {
        if (relax_edge(u, e.v, e.cost) && (stage == n)) {
          nd[e.v].infinite = true;
        }
      }
    }
  }
}

bool rev_dfs(int u) {
  bool result = nd[u].infinite;
  nd[u].visited = true;
  for (int v: nd[u].rev_adj) {
    if (!nd[v].visited) {
      result |= rev_dfs(v);
    }
  }
  return result;
}

int main() {
  read_data();
  bellman_ford();
  bool reachable_infinite = rev_dfs(n);
  printf("%lld\n", reachable_infinite ? -1 : -nd[n].dist);

  return 0;
}
