// Method: The Hopcroft-Karp algorithm as described on
// https://en.wikipedia.org/wiki/Hopcroft%E2%80%93Karp_algorithm#Pseudocode
#include <queue>
#include <stdio.h>

const int MAX_NODES = 500;
const int MAX_EDGES = 1'000;
const int NIL = 0;
const int INFINITY = 32'767;

struct edge {
  short v, next;
};

struct left_node {
  short adj;
  short dist;
  short pair;
};

struct right_node {
  short pair;
};

edge e[MAX_EDGES + 1];
left_node l[MAX_NODES + 1];
right_node r[MAX_NODES + 1];
int num_l, num_r;

void read_data() {
  int m, u, v;

  scanf("%d %d %d", &num_l, &num_r, &m);
  for (int i = 1; i <= m; i++) {
    scanf("%d %d", &u, &v);
    e[i] = { (short)v, l[u].adj };
    l[u].adj = i;
  }
}

bool bfs() {
  std::queue<short> q;
  for (int u = 1; u <= num_l; u++) {
    if (l[u].pair == NIL) {
      l[u].dist = 0;
      q.push(u);
    } else {
      l[u].dist = INFINITY;
    }
  }
  l[NIL].dist = INFINITY;

  while (!q.empty()) {
    int u = q.front();
    q.pop();
    if (l[u].dist < l[NIL].dist) {
      for (int pos = l[u].adj; pos; pos = e[pos].next) {
        int v = e[pos].v, p = r[v].pair;
        if (l[p].dist == INFINITY) {
          l[p].dist = l[u].dist + 1;
          q.push(p);
        }
      }
    }
  }

  return l[NIL].dist != INFINITY;
}

bool dfs(int u) {
  if (u == NIL) {
    return true;
  }

  bool found = false;
  int pos = l[u].adj;
  while (!found && pos) {
    int v = e[pos].v, p = r[v].pair;
    if ((l[p].dist == l[u].dist + 1) && dfs(p)) {
      l[u].pair = v;
      r[v].pair = u;
      found = true;
    }
    pos = e[pos].next;
  }

  if (found) {
    return true;
  } else {
    l[u].dist = INFINITY;
    return false;
  }
}

int hopcroft_karp() {
  for (int u = 1; u <= num_l; u++) {
    l[u].pair = NIL;
  }
  for (int v = 1; v <= num_r; v++) {
    r[v].pair = NIL;
  }

  int size = 0;
  while (bfs()) {
    for (int u = 1; u <= num_l; u++) {
      if ((l[u].pair == NIL) && dfs(u)) {
        size++;
      }
    }
  }
  return size;
}

void recover_matching() {
  for (int u = 1; u <= num_l; u++) {
    if (l[u].pair != NIL) {
      printf("%d %d\n", u, l[u].pair);
    }
  }
}

int main() {
  read_data();
  int max_flow = hopcroft_karp();
  printf("%d\n", max_flow);
  recover_matching();

  return 0;
}
