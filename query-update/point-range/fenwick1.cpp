// Complexity: O(Q log N)
//
// Method: Fenwick trees
#include "common.h"

struct fenwick_tree {
  long long v[MAX_N + 1];
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

  void add(int pos, int val) {
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

  long long range_sum(int from, int to) {
    return prefix_sum(to) - prefix_sum(from - 1);
  }
};

fenwick_tree fen;

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      fen.add(q[i].x, q[i].y);
    } else {
      answer[num_answers++] = fen.range_sum(q[i].x, q[i].y);
    }
  }
}

int main() {

  read_data();
  mark_time();

  fen.from_array(v, n);
  process_ops();

  report_time("Fenwick tree, 1-based");
  write_data();

  return 0;
}
