// Complexity: O(Q log N)
//
// Method: Fenwick trees indexed from 0.
#include "common.h"

struct fenwick_tree {
  long long v[MAX_N];
  int n;

  void from_array(long long* src, int _n) {
    n = _n;
    for (int i = 0; i < n; i++) {
      v[i] = src[i];
    }
    build();
  }

  // Transforms a regular array into a Fenwick tree
  // https://stackoverflow.com/a/31070683/6022817
  void build() {
    for (int i = 0; i < n; i++) {
      int j = i | (i + 1);
      if (j < n) {
        v[j] += v[i];
      }
    }
  }

  void add(int pos, int val) {
    do {
      v[pos] += val;
      pos |= pos + 1;
    } while (pos < n);
  }

  long long prefix_sum(int pos) {
    long long s = 0;
    do {
      s += v[pos];
      pos = (pos & (pos + 1)) - 1;
    } while (pos >= 0);
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
      fen.add(q[i].x - 1, q[i].y);
    } else {
      answer[num_answers++] = fen.range_sum(q[i].x - 1, q[i].y - 1);
    }
  }
}

int main() {

  read_data();
  mark_time();

  fen.from_array(v, n);
  process_ops();

  report_time("Fenwick tree, 0-based");
  write_data();

  return 0;
}
