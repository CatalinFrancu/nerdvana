// Complexity: O(Q log N)
//
// Method: segment tree with lazy updates, implemented recursively.
// Invariant: s[node] + lazy[node] * size is the real sum.
#include "common.h"

inline int min(int x, int y) {
  return (x < y) ? x : y;
}

inline int max(int x, int y) {
  return (x > y) ? x : y;
}

struct segment_tree {
  long long s[MAX_SEGTREE_NODES];
  long long lazy[MAX_SEGTREE_NODES]; // quantity to add to every element spanned
  int n;

  void init(long long* v, int _n) {
    n = next_power_of_2(_n);
    for (int i = 0; i < _n; i++) {
      s[n + i] = v[i];
    }
    build();
  }

  void build() {
    for (int i = n - 1; i >= 1; i--) {
      s[i] = s[2 * i] + s[2 * i + 1];
    }
  }

  void push(int node, int size) {
    s[node] += lazy[node] * size;
    lazy[2 * node] += lazy[node];
    lazy[2 * node + 1] += lazy[node];
    lazy[node] = 0;
  }

  void pull(int node, int size) {
    s[node] =
      s[2 * node] + lazy[2 * node] * size / 2 +
      s[2 * node + 1] + lazy[2 * node + 1] * size / 2;
  }

  long long _query(int node, int pl, int pr, int l, int r) {
    if (l >= r) {
      return 0;
    } else if ((l == pl) && (r == pr)) {
      return s[node] + lazy[node] * (r - l);
    } else {
      push(node, pr - pl);
      int mid = (pl + pr) >> 1;
      return
        _query(node * 2, pl, mid, l, min(r, mid)) +
        _query(node * 2 + 1, mid, pr, max(l, mid), r);
    }
  }

  long long query(int l, int r) {
    return _query(1, 0, n, l, r);
  }

  void _update(int node, int pl, int pr, int l, int r, int delta) {
    if (l >= r) {
      return;
    } else if ((l == pl) && (r == pr)) {
      lazy[node] += delta;
    } else {

      push(node, pr - pl);
      int mid = (pl + pr) >> 1;
      _update(2 * node, pl, mid, l, min(r, mid), delta);
      _update(2 * node + 1, mid, pr, max(l, mid), r, delta);
      pull(node, pr - pl);

    }
  }

  void update(int l, int r, int delta) {
    _update(1, 0, n, l, r, delta);
  }
};

segment_tree st;

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      st.update(q[i].l - 1, q[i].r, q[i].val);
    } else {
      answer[num_answers++] = st.query(q[i].l - 1, q[i].r);
    }
  }
}

int main() {

  read_data();
  mark_time();

  st.init(v, n);
  process_ops();

  report_time("segment tree, recursive");
  write_data();

  return 0;
}
