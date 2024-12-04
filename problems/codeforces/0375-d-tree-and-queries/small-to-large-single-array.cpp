// Efficient small-to-large. Rewrite of https://codeforces.com/contest/375/submission/5508178
//
// This method uses a single data structure containing
//
// 1. Each color's frequency.
// 2. Each frequency's frequency (i.e., "how many colors have frequency f?")
// 3. A Fenwick array on top of (2) to allow for range queries.
//
// The process in dfs() offers each node exclusive access to the data
// structure while computing its subtree info.
#include <list>
#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_COLORS = 100'000;
const int MAX_QUERIES = 100'000;

struct fenwick_tree {
  int v[MAX_NODES + 1];
  int total;
  int n;

  void init(int n) {
    this->n = n;
  }

  void add(int pos, int val) {
    if (pos) {
      total += val;
      do {
        v[pos] += val;
        pos += pos & -pos;
      } while (pos <= n);
    }
  }

  int sum(int pos) {
    int s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }

  int suffix_sum(int pos) {
    return (pos > n)
      ? 0
      : (total - sum(pos - 1));
  }
};

struct query {
  int orig_index;
  int min_freq;
};

struct node {
  int color;
  int heavy;
  std::list<int> adj;
  std::list<query> q;
};

node nd[MAX_NODES + 1];
int freq[MAX_COLORS + 1];
fenwick_tree fen;
int sol[MAX_QUERIES];
int n, q;

void read_data() {
  scanf("%d %d", &n, &q);
  for (int u = 1; u <= n; u++) {
    scanf("%d", &nd[u].color);
  }
  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }
  for (int i = 0; i < q; i++) {
    int u, min_freq;
    scanf("%d %d", &u, &min_freq);
    nd[u].q.push_back({i, min_freq});
  }
}

// Compute each node's heaviest child. Remove each parent's node from the
// adjacency list. Return the node's subtree size.
int size_dfs(int u, int parent) {
  int size = 1, max_c_size = 0;

  std::list<int>::iterator it = nd[u].adj.begin();
  while (it != nd[u].adj.end()) {
    int v = *it;
    if (v == parent) {
      nd[u].adj.erase(it++);
    } else {
      int c = size_dfs(v, u);
      size += c;
      if (!nd[u].heavy || (c > max_c_size)) {
        nd[u].heavy = v;
        max_c_size = c;
      }
      it++;
    }
  }

  return size;
}

void change_freq(int color, int delta) {
  fen.add(freq[color], -1);
  freq[color] += delta;
  fen.add(freq[color], +1);
}

void include_exclude_dfs(int u, int delta) {
  change_freq(nd[u].color, delta);
  for (int v: nd[u].adj) {
    include_exclude_dfs(v, delta);
  }
}

void dfs(int u) {
  for (int v: nd[u].adj) {
    if (v != nd[u].heavy) {
      dfs(v);
      include_exclude_dfs(v, -1);
    }
  }

  if (nd[u].heavy) {
    dfs(nd[u].heavy);
  }

  for (int v: nd[u].adj) {
    if (v != nd[u].heavy) {
      include_exclude_dfs(v, +1);
    }
  }

  change_freq(nd[u].color, +1);

  for (query q: nd[u].q) {
    sol[q.orig_index] = fen.suffix_sum(q.min_freq);
  }
}

void write_solution() {
  for (int i = 0; i < q; i++) {
    printf("%d\n", sol[i]);
  }
}

int main() {
  read_data();
  size_dfs(1, 0);
  fen.init(n);
  dfs(1);
  write_solution();

  return 0;
}
