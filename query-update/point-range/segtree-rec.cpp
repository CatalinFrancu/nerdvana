// Complexity: O(Q log N)
//
// Method: segment tree, recursive implementation
#include "common.h"

struct segment_tree {
  long long s[MAX_SEGTREE_NODES];
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

  // [l, r) semi-closed interval
  long long _query(int node, int pl, int pr, int l, int r) {
    if (l >= r) {
      return 0;
    } else if ((l == pl) && (r == pr)) {
      return s[node];
    } else {
      int mid = (pl + pr) >> 1;

      return
        _query(node * 2, pl, mid, l, min(r, mid)) +
        _query(node * 2 + 1, mid, pr, max(l, mid), r);
    }
  }

  long long query(int l, int r) {
    return _query(1, 0, n, l, r);
  }

  void _update(int node, int pl, int pr, int pos, int delta) {
    if (pr - pl == 1) {
      s[node] += delta;
    } else {
      int mid = (pl + pr) >> 1;
      if (pos < mid) {
        _update(2 * node, pl, mid, pos, delta);
      } else {
        _update(2 * node + 1, mid, pr, pos, delta);
      }
      s[node] = s[2 * node] + s[2 * node + 1];
    }
  }

  void update(int pos, int delta) {
    _update(1, 0, n, pos, delta);
  }
};

segment_tree st;

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      st.update(q[i].x - 1, q[i].y);
    } else {
      answer[num_answers++] = st.query(q[i].x - 1, q[i].y);
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
