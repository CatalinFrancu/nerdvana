// Author: Cătălin Frâncu
// Complexity: O(Q log N)
//
// Method: Fenwick trees with range updates
#include "common.h"

// v is the base tree, w is the second level
long long w[MAX_N + 1];

// Transforms a regular array into a Fenwick tree
// https://stackoverflow.com/a/31070683/6022817
void fenwick_build() {
  for (int i = 1; i <= n; i++) {
    int j = i + (i & -i);
    if (j <= n) {
      v[j] += v[i];
    }
  }
}

void fenwick_add(long long* v, int pos, long long val) {
  do {
    v[pos] += val;
    pos += pos & -pos;
  } while (pos <= n);
}

long long fenwick_sum(long long* v, int pos) {
  long long s = 0;
  while (pos) {
    s += v[pos];
    pos &= pos - 1;
  }
  return s;
}

void fenwick_range_add(int l, int r, long long val) {
  fenwick_add(v, l, -val * (l - 1));
  fenwick_add(v, r + 1, val * r);
  fenwick_add(w, l, val);
  fenwick_add(w, r + 1, -val);
}

long long fenwick_sum2(int pos) {
  return fenwick_sum(w, pos) * pos + fenwick_sum(v, pos);
}

long long fenwick_range_sum(int l, int r) {
  return fenwick_sum2(r) - fenwick_sum2(l - 1);
}

int main() {

  read_data();
  mark_time();

  fenwick_build();

  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == T_UPDATE) {
      fenwick_range_add(q[i].l, q[i].r, q[i].val);
    } else {
      answer[num_answers++] = fenwick_range_sum(q[i].l, q[i].r);
    }
  }

  report_time("Fenwick trees");
  write_data();

  return 0;
}
