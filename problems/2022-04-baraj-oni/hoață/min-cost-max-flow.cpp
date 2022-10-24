#include <stdio.h>

#define MAX_N 300
#define MAX_W 300
#define MAX_NODES (MAX_N * (MAX_W + 1) + 1)
#define MAX_EDGES (MAX_NODES * 4)

#define INFINITY 1'000'000'000
#define NIL -1

typedef struct {
  int v, cap, cost, next;
} edge;

typedef struct {
  int u, d;
} heap_node;

// input data
int val[MAX_N], weight[MAX_N], alarm[MAX_N];
int rooms, thieves, knapsack;

// Note: because we always call add_edge() in pairs, the reverse edge of i
// will always be i ^ 1.
edge e[MAX_EDGES];   // statically allocated edge array
int adj[MAX_NODES];  // heads of adjacency lists
int d[MAX_NODES];    // Dijkstra distances / potentials
int p[MAX_NODES];    // Dijkstra parents
int pe[MAX_NODES];   // edge from parent to child in Dijkstra
int sink_potential;  // cumulative
int nodes, edges;

// heap for Dijkstra
heap_node h[MAX_NODES]; // 1-based
int hpos[MAX_NODES];    // lookup table for decrease_key
int hsize;

inline int min(int x, int y) {
  return (x < y) ? x : y;
}

// heap functions
void heap_init() {
  hsize = 0;
  for (int u = 0; u < nodes; u++) {
    hpos[u] = NIL;
  }
}

inline void heap_set(int pos, heap_node x) {
  h[pos] = x;
  hpos[x.u] = pos;
}

// Updates u's distance to d[u], which is known to have decreased.
// If u is not in the heap, then inserts u.
void heap_decrease_key(int u) {
  int pos = (hpos[u] == NIL) ? ++hsize : hpos[u];
  h[0].d = d[u]; // sentinel

  while (d[u] < h[pos / 2].d) {
    heap_set(pos, h[pos / 2]);
    pos /= 2;
  }

  heap_set(pos, { u, d[u] });
}

heap_node heap_pop() { // not to be confused with hip-hop
  heap_node root = h[1], f = h[hsize--];
  hpos[root.u] = NIL;

  int p = 1;
  bool run = true;
  while (run && 2 * p <= hsize) {
    // c = best child
    int c = ((2 * p + 1 <= hsize) && (h[2 * p + 1].d < h[2 * p].d))
      ? (2 * p + 1)
      : (2 * p);

    if (h[c].d < f.d)  {
      heap_set(p, h[c]);
      p = c;
    } else {
      run = false;
    }
  }

  heap_set(p, f);
  return root;
}

void add_edge(int u, int v, int cap, int cost) {
  e[edges] = { v, cap, cost, adj[u] };
  adj[u] = edges++;
}

void init_graph() {
  nodes = rooms * (knapsack + 1) + 1;
  edges = 0;
  for (int i = 0; i < nodes; i++) {
    adj[i] = NIL;
  }

  for (int i = 0; i < rooms; i++) {
    for (int j = 0; j <= knapsack; j++) {
      int u = i * (knapsack + 1) + j;

      if (j >= weight[i]) {
        add_edge(u - weight[i], u, thieves, -val[i]);
        add_edge(u, u - weight[i], 0, +val[i]);
      }

      if (i) {
        add_edge(u - (knapsack + 1), u, alarm[i - 1], 0);
        add_edge(u, u - (knapsack + 1), 0, 0);
      }
    }
  }

  // connect every weight of the last room to the sink
  for (int u = (rooms - 1) * (knapsack + 1); u < nodes - 1; u++) {
    add_edge(u, nodes - 1, alarm[rooms - 1], 0);
    add_edge(nodes - 1, u, 0, 0);
  }

  sink_potential = 0;
}

// Dijkstra works to establish the initial potentials, as no negative cycles
// exist. However, there exist negative edges, so Dijkstra can be very slow
// (hundreds of millions of heap inserts).
void init_potentials() {
  d[0] = 0;
  for (int u = 1; u < nodes; u++) {
    d[u] = INFINITY;
  }

  for (int u = 0; u < nodes; u++) {
    for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
      if (e[pos].cap) {
        d[e[pos].v] = min(d[e[pos].v], d[u] + e[pos].cost);
      }
    }
  }
}

void reduce_costs() {
  for (int u = 0; u < nodes; u++) {
    if (d[u] != INFINITY) {
      for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
        e[pos].cost += d[u] - d[e[pos].v];
      }
    }
  }

  sink_potential += d[nodes - 1];
}

// Returns true iff the sink is reachable
int dijkstra() {
  d[0] = 0;
  for (int u = 1; u < nodes; u++) {
    d[u] = INFINITY;
  }
  heap_init();
  heap_decrease_key(0);

  while (hsize) {
    heap_node pair = heap_pop();
    if (pair.d == d[pair.u]) {
      int u = pair.u;
      for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
        // Relax the edge.
        //
        // Relax, have a cigar, make yourself at home. Hell is full of high
        // court judges, failed saints. We've got cardinals, archbishops,
        // barristers, certified accountants, music critics, they're all
        // here. You're not alone. You're never alone, not here you're not. OK
        // break's over.
        int v = e[pos].v;
        if (e[pos].cap && d[u] + e[pos].cost < d[v]) {
          d[v] = d[u] + e[pos].cost;
          p[v] = u;
          pe[v] = pos; // Make a note of e[pos] so we can later push flow on it
          heap_decrease_key(v);
        }
      }
    }
  }

  return d[nodes - 1] < INFINITY;
}

// Determines the bottleneck capacity along the shortest path (no more than
// flow). Pushes that flow and returns it.
int push_flow(int u, int flow) {
  int v = u;
  while (v) {
    flow = min(flow, e[pe[v]].cap);
    v = p[v];
  }

  // Now push it.
  while (u) {
    int pos = pe[u];
    e[pos].cap -= flow;
    e[pos ^ 1].cap += flow;
    u = p[u];
  }

  return flow;
}

int main() {
  int num_tests;

  scanf("%d", &num_tests);
  while (num_tests--) {

    scanf("%d %d %d", &rooms, &thieves, &knapsack);
    for (int i = 0; i < rooms; i++) {
      scanf("%d %d %d", &val[i], &weight[i], &alarm[i]);
    }

    init_graph();
    init_potentials();
    reduce_costs();

    int profit = 0;
    int remaining = thieves;
    while (remaining && dijkstra()) {
      reduce_costs();
      int pushed = push_flow(nodes - 1, remaining);
      remaining -= pushed;
      profit -= pushed * sink_potential; // sink_potential is negative
    }

    printf("%d\n", remaining ? -1 : profit);
  }

  return 0;
}
