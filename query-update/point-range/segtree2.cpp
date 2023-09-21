// Author: Cătălin Frâncu
// Complexity: O(Q log N)
//
// Method: segment tree, optimized
#include "common.h"

long long s[2 * MAX_N];

void st_build() {
  for (int i = n - 1; i >= 1; i--) {
    s[i] = s[2 * i] + s[2 * i + 1];
  }
}

// [l, r] closed interval
long long st_query(int l, int r) {
  long long sum = 0;

  l += n;
  r += n;

  while (l <= r)  {
    // take l and advance if it is a right child
    if (l & 1) {
      sum += s[l++];
    }
    l >>= 1;

    // take r and advance if it is a left child
    if (!(r & 1)) {
      sum += s[r--];
    }
    r >>= 1;
  }
  // During the last iteration, l and r may point to the same node, in which
  // case the node will be added exactly once, either by l or by r.

  return sum;
}

void st_update(int pos, int delta) {
  for (pos += n; pos > 1; pos >>= 1) {
    s[pos] += delta;
  }
}

int main() {

  read_data();
  mark_time();

  // round n to the next power of 2
  int orig_n = n;
  while (n & (n - 1)) {
    n += (n & -n);
  }

  for (int i = 0; i < orig_n; i++) {
    s[n + i] = v[i + 1];
  }

  st_build();

  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      st_update(q[i].x - 1, q[i].y);
    } else {
      answer[num_answers++] = st_query(q[i].x - 1, q[i].y - 1);
    }
  }

  report_time("segment tree, iterative");
  write_data();

  return 0;
}
