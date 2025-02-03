#include <stdio.h>
#include <vector>

const int MAX_NODES = 200'000;

struct node {
  std::vector<int> adj;
  int size;
  bool dead;
};

node nd[MAX_NODES + 1];
int freq[MAX_NODES];
int n, length;
long long answer;

void read_input_data() {
  scanf("%d %d", &n, &length);

  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }
}

/**
 * Following is the problem we are actually trying to solve.
 **/

int max(int x, int y) {
  return (x > y) ? x : y;
}

int max_depth;

// Counts the paths of length k starting from u, going up to the current
// centroid and down into another subtree.
void count_dfs(int u, int parent, int depth) {
  max_depth = max(max_depth, depth);
  answer += freq[length - depth];

  for (int v: nd[u].adj) {
    if ((!nd[v].dead) && (v != parent) && (depth < length)) {
      count_dfs(v, u, depth + 1);
    }
  }
}

// Marks the depth of every node.
void mark_dfs(int u, int parent, int depth) {
  freq[depth]++;

  for (int v: nd[u].adj) {
    if ((!nd[v].dead) && (v != parent) && (depth < length)) {
      mark_dfs(v, u, depth + 1);
    }
  }
}

// Counts the paths of length k that go trough u.
void count_paths_through(int u) {
  freq[0] = 1; // node u itself
  max_depth = 0;

  for (int v: nd[u].adj) {
    if (!nd[v].dead) {
      count_dfs(v, u, 1);
      mark_dfs(v, u, 1);
    }
  }

  for (int d = 0; d <= max_depth; d++) {
    freq[d] = 0;
  }
}

/**
 * The rest of the code (decompose(), size_dfs() and find_centroid()) is
 * boilerplate. We use it because it allow us to run a DFS from every centroid
 * and still achieve O(n log n) time.
 **/

void size_dfs(int u, int parent) {
  nd[u].size = 1;

  for (int v: nd[u].adj) {
    if ((v != parent) && !nd[v].dead) {
      size_dfs(v, u);
      nd[u].size += nd[v].size;
    }
  }
}

int find_centroid(int u, int limit) {
  for (int v: nd[u].adj) {
    if ((nd[v].size < nd[u].size) && (nd[v].size > limit)) {
      return find_centroid(v, limit);
    }
  }

  return u;
}

void decompose(int u) {
  size_dfs(u, 0);
  u = find_centroid(u, nd[u].size / 2);

  // This is what we are actually trying to solve.
  count_paths_through(u);

  nd[u].dead = true;
  for (int v: nd[u].adj) {
    if (!nd[v].dead) {
      decompose(v);
    }
  }
}

void write_answer() {
  printf("%lld\n", answer);
}

int main() {
  read_input_data();
  decompose(1);
  write_answer();

  return 0;
}
