// Method: Edmonds-Karp (Ford-Fulkerson + BFS) with a speed optimization. Once
// we build the BFS tree, do not stop after pumping flow along the sink's
// predecessor. Instead, iterate through all of the sink's predecessors and
// try to pump flow along each of them. This may find several disjoint
// augmenting paths.
//
// Afterwards, run a unit DFS from s to t repeatedly and print the path.
#include <queue>
#include <stdio.h>

const int MAX_NODES = 500;
const int MAX_EDGES = 1'000;
const int NONE = -1;
const int INFINITY = 1'001;

struct edge {
  short v;
  short cap;
  short next;
};

struct node {
  short adj;
  short parent, edge;
};

edge e[2 * MAX_EDGES];
node n[MAX_NODES + 1];
std::queue<short> q;
int num_nodes, num_edges;

void add_edge(short u, short v, short cap) {
  static int pos = 0;

  e[pos] = { v, cap, n[u].adj };
  n[u].adj = pos++;
}

void read_data() {
  scanf("%d %d", &num_nodes, &num_edges);
  for (int u = 1; u <= num_nodes; u++) {
    n[u].adj = NONE;
  }
  for (int i = 0; i < num_edges; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v, 1);
    add_edge(v, u, 0);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

bool bfs_reachable() {
  for (int u = 1; u <= num_nodes; u++) {
    n[u].parent = NONE;
  }
  q = {};
  q.push(1);

  while (!q.empty() && (n[num_nodes].parent == NONE)) {
    int u = q.front();
    q.pop();
    for (int pos = n[u].adj; pos != NONE; pos = e[pos].next) {
      int v = e[pos].v;
      // Only consider positive capacities towards unexplored nodes.
      if ((n[v].parent == NONE) && e[pos].cap) {
        n[v].parent = u;
        n[v].edge = pos;
        q.push(v);
      }
    }
  }

  return (n[num_nodes].parent != NONE);
}

int path_minimum() {
  int min_cap = INFINITY;
  int u = num_nodes;
  while (u != 1) {
    min_cap = min(min_cap, e[n[u].edge].cap);
    u = n[u].parent;
  }
  return min_cap;
}

void pump_on_path(int flow) {
  int u = num_nodes;
  while (u != 1) {
    e[n[u].edge].cap -= flow;
    e[n[u].edge ^ 1].cap += flow;
    u = n[u].parent;
  }
}

int ford_fulkerson() {
  int flow = 0;

  while (bfs_reachable()) {
    // the sink's successors are also its predecessors
    for (int pos = n[num_nodes].adj; pos != NONE; pos = e[pos].next) {
      int v = e[pos].v;
      if ((v == 1) || (n[v].parent != NONE)) {
        // Connect the sink to v.
        n[num_nodes].parent = v;
        n[num_nodes].edge = pos ^ 1;
        int augment = path_minimum();
        if (augment) {
          pump_on_path(augment);
          flow += augment;
        }
      }
    }
  }

  return flow;
}

void clear_parent() {
  for (int u = 1; u <= num_nodes; u++) {
    n[u].parent = NONE;
  }
}

void dfs(int u, int parent, int edge) {
  n[u].parent = parent;
  n[u].edge = edge;

  for (int pos = n[u].adj; pos != NONE; pos = e[pos].next) {
    int v = e[pos].v;
    int rev = pos ^ 1;
    // Follow a regular edge if there is residual flow on the reverse edge.
    if ((n[v].parent == NONE) &&
        !(pos & 1) &&
        e[rev].cap) {
      dfs(v, u, pos);
    }
  }
}

void print_path(int u, int len) {
  if (u == 1) {
    printf("%d\n1 ", len);
  } else {
    int edge = n[u].edge;
    e[edge].cap++;
    e[edge ^ 1].cap--;
    print_path(n[u].parent, len + 1);
    printf("%d ", u);
  }
}

void recover_paths() {
  do {
    clear_parent();
    dfs(1, NONE, NONE);
    if (n[num_nodes].parent != NONE) {
      print_path(num_nodes, 1);
      printf("\n");
    }
  } while (n[num_nodes].parent != NONE);
}

int main() {
  read_data();
  int max_flow = ford_fulkerson();
  printf("%d\n", max_flow);
  recover_paths();

  return 0;
}
