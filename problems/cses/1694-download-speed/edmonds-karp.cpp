// Method: Edmonds-Karp (Ford-Fulkerson + BFS).
#include <stdio.h>

const int MAX_NODES = 1'000;
const int MAX_EDGES = 5'000;
const int NONE = -1;
const int INFINITY = 1'000'000'001;

struct edge {
  int cap;
  short v, next;
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

edge e[2 * MAX_EDGES];
node n[MAX_NODES + 1];
queue q;
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

long long ford_fulkerson() {
  long long flow = 0;

  while (bfs_reachable()) {
    int augment = path_minimum();
    pump_on_path(augment);
    flow += augment;
  }

  return flow;
}

int main() {
  read_data();
  long long max_flow = ford_fulkerson();
  printf("%lld\n", max_flow);

  return 0;
}
