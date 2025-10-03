// Complexity: O(Q log N)
//
// Method: Fenwick trees with range updates
#include "common.h"

struct fenwick_tree {
  long long v[MAX_N + 2];
  int n;

  void from_array(long long* src, int _n) {
    n = _n;
    for (int i = 1; i <= n; i++) {
      v[i] = src[i - 1];
    }
    build();
  }

  // Transforms a regular array into a Fenwick tree
  // https://stackoverflow.com/a/31070683/6022817
  void build() {
    for (int i = 1; i <= n; i++) {
      int j = i + (i & -i);
      if (j <= n) {
        v[j] += v[i];
      }
    }
  }

  void add(int pos, long long val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  long long prefix_sum(int pos) {
    long long s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }
};

struct fenwick_tree_2 {
  fenwick_tree v, w;

  void from_array(long long* src, int n) {
    v.from_array(src, n);
    w.n = n;
  }

  void range_add(int l, int r, long long val) {
    v.add(l, -val * (l - 1));
    v.add(r + 1, val * r);
    w.add(l, val);
    w.add(r + 1, -val);
  }

  long long prefix_sum(int pos) {
    return w.prefix_sum(pos) * pos + v.prefix_sum(pos);
  }

  long long range_sum(int l, int r) {
    return prefix_sum(r) - prefix_sum(l - 1);
  }
};

fenwick_tree_2 fen;

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      fen.range_add(q[i].l, q[i].r, q[i].val);
    } else {
      answer[num_answers++] = fen.range_sum(q[i].l, q[i].r);
    }
  }
}

int main() {

  read_data();
  mark_time();

  fen.from_array(v, n);
  process_ops();

  report_time("Fenwick trees");
  write_data();

  return 0;
}
