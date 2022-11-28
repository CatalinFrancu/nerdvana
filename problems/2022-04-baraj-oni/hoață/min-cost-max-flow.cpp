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
  int adj;  // head of adjacency list
  int d;    // Dijkstra distance / potential
  int p;    // Dijkstra parent
  int pe;   // edge from parent to child in Dijkstra
  int hpos; // lookup table for decrease_key
} node;

// input data
int val[MAX_N], weight[MAX_N], alarm[MAX_N];
int rooms, thieves, knapsack;

// Note: because we always call add_edge() in pairs, the reverse edge of i
// will always be i ^ 1.
node n[MAX_NODES];
edge e[MAX_EDGES];       // statically allocated edge array
int h[MAX_NODES], hsize; // heap for Dijkstra (1-based)
int sink_potential;  // cumulative
int nodes, edges;

inline int min(int x, int y) {
  return (x < y) ? x : y;
}

// heap functions
void heap_init() {
  hsize = 0;
  for (int u = 0; u < nodes; u++) {
    n[u].hpos = NIL;
  }
}

inline void heap_set(int pos, int u) {
  h[pos] = u;
  n[u].hpos = pos;
}

inline bool heap_cmp(int u, int v) {
  return (n[u].d < n[v].d) || (n[u].d == n[v].d && u < v);
}

// Updates u's distance to d[u], which is known to have decreased.
// If u is not in the heap, then inserts u.
void heap_decrease_key(int u) {
  int pos = (n[u].hpos == NIL) ? ++hsize : n[u].hpos;
  h[0] = u; // sentinel

  while (heap_cmp(u, h[pos / 2])) {
    heap_set(pos, h[pos / 2]);
    pos /= 2;
  }

  heap_set(pos, u);
}

int heap_pop() { // not to be confused with hip-hop
  int root = h[1], f = h[hsize--];
  n[root].hpos = NIL;

  int p = 1;
  bool run = true;
  while (run && 2 * p <= hsize) {
    // c = best child
    int c = ((2 * p + 1 <= hsize) && heap_cmp(h[2 * p + 1], h[2 * p]))
      ? (2 * p + 1)
      : (2 * p);

    if (heap_cmp(h[c], f))  {
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
  e[edges] = { v, cap, cost, n[u].adj };
  n[u].adj = edges++;
}

void init_graph() {
  nodes = rooms * (knapsack + 1) + 1;
  edges = 0;
  for (int u = 0; u < nodes; u++) {
    n[u].adj = NIL;
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
  n[0].d = 0;
  for (int u = 1; u < nodes; u++) {
    n[u].d = INFINITY;
  }

  for (int u = 0; u < nodes; u++) {
    for (int pos = n[u].adj; pos != NIL; pos = e[pos].next) {
      if (e[pos].cap) {
        n[e[pos].v].d = min(n[e[pos].v].d, n[u].d + e[pos].cost);
      }
    }
  }
}

void reduce_costs() {
  for (int u = 0; u < nodes; u++) {
    if (n[u].d != INFINITY) {
      for (int pos = n[u].adj; pos != NIL; pos = e[pos].next) {
        e[pos].cost += n[u].d - n[e[pos].v].d;
      }
    }
  }

  sink_potential += n[nodes - 1].d;
}

// Returns true iff the sink is reachable
bool dijkstra() {
  n[0].d = 0;
  for (int u = 1; u < nodes; u++) {
    n[u].d = INFINITY;
  }
  heap_init();
  heap_decrease_key(0);

  while (hsize) {
    int u = heap_pop();
    for (int pos = n[u].adj; pos != NIL; pos = e[pos].next) {
      // Relax the edge.
      //
      // Relax, have a cigar, make yourself at home. Hell is full of high
      // court judges, failed saints. We've got cardinals, archbishops,
      // barristers, certified accountants, music critics, they're all
      // here. You're not alone. You're never alone, not here you're not. OK
      // break's over.
      int v = e[pos].v;
      if (e[pos].cap && n[u].d + e[pos].cost < n[v].d) {
        n[v].d = n[u].d + e[pos].cost;
        n[v].p = u;
        n[v].pe = pos; // Make a note of e[pos] so we can later push flow on it
        heap_decrease_key(v);
      }
    }
  }

  return n[nodes - 1].d < INFINITY;
}

// Determines the bottleneck capacity along the shortest path (no more than
// flow). Pushes that flow and returns it.
int push_flow(int u, int flow) {
  int v = u;
  while (v) {
    flow = min(flow, e[n[v].pe].cap);
    v = n[v].p;
  }

  // Now push it.
  while (u) {
    int pos = n[u].pe;
    e[pos].cap -= flow;
    e[pos ^ 1].cap += flow;
    u = n[u].p;
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
