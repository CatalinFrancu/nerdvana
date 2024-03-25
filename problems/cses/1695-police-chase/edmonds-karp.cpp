// Method: Edmonds-Karp (Ford-Fulkerson + BFS) with a speed optimization. Once
// we build the BFS tree, do not stop after pumping flow along the sink's
// predecessor. Instead, iterate through all of the sink's predecessors and
// try to pump flow along each of them. This may find several disjoint
// augmenting paths.
//
// The last BFS leaves positive parents for reachable nodes, NONE for
// unreachable nodes. Use the information to print all edges going from
// reachable to unreachable nodes.
#include <stdio.h>

const int MAX_NODES = 500;
const int MAX_EDGES = 1'000;
const int NONE = -1;
const int INFINITY = 1'000'000;

struct edge {
  short v;
  short cap;
  short next;
};

struct node {
  short adj;
  short parent, edge;
};

struct queue {
  int v[MAX_NODES];
  int head, tail;

  void clear() {
    head = tail = 0;
  }

  void enqueue(int x) {
    v[tail++] = x;
  }

  int dequeue() {
    return v[head++];
  }

  bool is_empty() {
    return head == tail;
  }
};

edge e[4 * MAX_EDGES];
node n[MAX_NODES + 1];
queue q;
int num_nodes, num_edges;

void add_edge(short u, short v) {
  static int pos = 0;

  e[pos] = { v, 1, n[u].adj };
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
    add_edge(u, v);
    add_edge(v, u);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

bool bfs_reachable() {
  for (int u = 1; u <= num_nodes; u++) {
    n[u].parent = NONE;
  }
  q.clear();
  q.enqueue(1);

  while (!q.is_empty() && (n[num_nodes].parent == NONE)) {
    int u = q.dequeue();
    for (int pos = n[u].adj; pos != NONE; pos = e[pos].next) {
      int v = e[pos].v;
      // Only consider positive capacities towards unexplored nodes.
      if ((n[v].parent == NONE) && e[pos].cap) {
        n[v].parent = u;
        n[v].edge = pos;
        q.enqueue(v);
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

void write_min_cut(int flow) {
  printf("%d\n", flow);

  for (int u = 1; u <= num_nodes; u++) {
    if ((u == 1) || (n[u].parent != NONE)) {
      for (int pos = n[u].adj; pos != NONE; pos = e[pos].next) {
        int v = e[pos].v;
        // Only process original edges, not reverse edges.
        if ((n[v].parent == NONE) && !(pos & 1)) {
          printf("%d %d\n", u, v);
        }
      }
    }
  }
}

int main() {
  read_data();
  int max_flow = ford_fulkerson();
  write_min_cut(max_flow);

  return 0;
}
