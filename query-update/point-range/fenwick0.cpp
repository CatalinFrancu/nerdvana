// Author: Cătălin Frâncu
// Complexity: O(Q log N)
//
// Method: Fenwick trees indexed from 0.
#include "common.h"

// Transforms a regular array into a Fenwick tree
// https://stackoverflow.com/a/31070683/6022817
void fenwick_build() {
  for (int i = 0; i < n; i++) {
    int j = i | (i + 1);
    if (j < n) {
      v[j] += v[i];
    }
  }
}

void fenwick_add(int pos, int val) {
  do {
    v[pos] += val;
    pos |= pos + 1;
  } while (pos <= n);
}

long long fenwick_sum(int pos) {
  long long s = 0;
  do {
    s += v[pos];
    pos = (pos & (pos + 1)) - 1;
  } while (pos >= 0);
  return s;
}

// Returns the sum of the inclusive range.
long long fenwick_range_sum(int from, int to) {
  return fenwick_sum(to) - fenwick_sum(from - 1);
}

int main() {

  read_data();
  mark_time();

  fenwick_build();

  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      fenwick_add(q[i].x, q[i].y);
    } else {
      answer[num_answers++] = fenwick_range_sum(q[i].x, q[i].y);
    }
  }

  report_time("Fenwick trees, 0-based");
  write_data();

  return 0;
}
