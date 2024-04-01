// Method: Same as v1, but using STL data structures.
#include <queue>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 500;
const int NIL = 0;
const int INFINITY = 32'767;

struct left_node {
  std::vector<short> adj;
  short dist;
  short pair;
};

struct right_node {
  short pair;
};

left_node l[MAX_NODES + 1];
right_node r[MAX_NODES + 1];
int num_l, num_r;

void read_data() {
  int m, u, v;

  scanf("%d %d %d", &num_l, &num_r, &m);
  for (int i = 1; i <= m; i++) {
    scanf("%d %d", &u, &v);
    l[u].adj.push_back(v);
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
      for (int v: l[u].adj) {
        int p = r[v].pair;
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

  for (int v: l[u].adj) {
    int p = r[v].pair;
    if ((l[p].dist == l[u].dist + 1) && dfs(p)) {
      l[u].pair = v;
      r[v].pair = u;
      return true;
    }
  }

  l[u].dist = INFINITY;
  return false;
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
  printf("%d\n", hopcroft_karp());
  recover_matching();

  return 0;
}
