// Complexity: O(Q log N)
//
// Method: segment tree, iterative implementation
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

  // [l, r] closed interval
  long long query(int l, int r) {
    long long sum = 0;

    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {    // l is a right child
        sum += s[l++];
      }
      l >>= 1;

      if (!(r & 1)) { // r is a left child
        sum += s[r--];
      }
      r >>= 1;
    }
    // During the last iteration, l and r may point to the same node, in which
    // case the node will be added exactly once, either by l or by r.

    return sum;
  }

  void update(int pos, int delta) {
    for (pos += n; pos; pos >>= 1) {
      s[pos] += delta;
    }
  }
};

segment_tree st;

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      st.update(q[i].x - 1, q[i].y);
    } else {
      answer[num_answers++] = st.query(q[i].x - 1, q[i].y - 1);
    }
  }
}

int main() {

  read_data();
  mark_time();

  st.init(v, n);
  process_ops();

  report_time("segment tree, iterative");
  write_data();

  return 0;
}
