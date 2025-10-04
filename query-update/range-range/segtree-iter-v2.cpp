// Complexity: O(Q log N)
//
// Method: segment tree with lazy updates, implemented iteratively.
// Invariant: s[node] + lazy[node] * size is the real sum.
//
// v2: Implement push_path recursively without bit hacks. This is about 10%
// slower.
#include "common.h"

struct segment_tree_node {
  long long s;
  long long lazy; // quantity to add to every element spanned
};

struct segment_tree {
  segment_tree_node v[MAX_SEGTREE_NODES];
  int n;

  void init(long long* src, int _n) {
    n = next_power_of_2(_n);
    for (int i = 0; i < _n; i++) {
      v[n + i].s = src[i];
    }
    build();
  }

  void build() {
    for (int i = n - 1; i >= 1; i--) {
      v[i].s = v[2 * i].s + v[2 * i + 1].s;
    }
  }

  void push(int node, int size) {
    v[node].s += v[node].lazy * size;
    v[2 * node].lazy += v[node].lazy;
    v[2 * node + 1].lazy += v[node].lazy;
    v[node].lazy = 0;
  }

  void push_path(int node, int size) {
    if (node) {
      push_path(node / 2, size * 2);
      push(node, size);
    }
  }

  void pull(int node, int size) {
    v[node].s =
      v[2 * node].s + v[2 * node].lazy * size / 2 +
      v[2 * node + 1].s + v[2 * node + 1].lazy * size / 2;
  }

  void pull_path(int node) {
    for (int x = node / 2, size = 2; x; x /= 2, size <<= 1) {
      pull(x, size);
    }
  }

  long long query(int l, int r) {
    long long sum = 0;
    int size = 1;

    l += n;
    r += n;
    push_path(l / 2, 2);
    push_path(r / 2, 2);

    while (l <= r)  {
      if (l & 1) {
        sum += v[l].s + v[l].lazy * size;
        l++;
      }
      l >>= 1;

      if (!(r & 1)) {
        sum += v[r].s + v[r].lazy * size;
        r--;
      }
      r >>= 1;
      size <<= 1;
    }

    return sum;
  }

  void update(int l, int r, int delta) {
    l += n;
    r += n;
    int lc = l, rc = r;
    push_path(l / 2, 2);
    push_path(r / 2, 2);

    while (l <= r)  {
      if (l & 1) {
        v[l++].lazy += delta;
      }
      l >>= 1;

      if (!(r & 1)) {
        v[r--].lazy += delta;
      }
      r >>= 1;
    }

    pull_path(lc);
    pull_path(rc);
  }
};

segment_tree st;

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      st.update(q[i].l - 1, q[i].r - 1, q[i].val);
    } else {
      answer[num_answers++] = st.query(q[i].l - 1, q[i].r - 1);
    }
  }
}

int main() {

  read_data();
  mark_time();

  st.init(v, n);
  process_ops();

  report_time("segment tree, iterative v2");
  write_data();

  return 0;
}
