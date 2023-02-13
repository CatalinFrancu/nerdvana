// Author: Cătălin Frâncu
// Complexity: O(Q log N)
//
// Method: segment tree
#include "common.h"

long long s[2 * MAX_N];

inline int min(int x, int y) {
  return (x < y) ? x : y;
}

inline int max(int x, int y) {
  return (x > y) ? x : y;
}

void st_build() {
  for (int i = n - 1; i >= 1; i--) {
    s[i] = s[2 * i] + s[2 * i + 1];
  }
}

long long st_query(int node, int pl, int pr, int l, int r) {
  if (l >= r) {
    return 0;
  } else if ((l == pl) && (r == pr)) {
    return s[node];
  } else {
    int mid = (pl + pr) >> 1;

    return
      st_query(node * 2, pl, mid, l, min(r, mid)) +
      st_query(node * 2 + 1, mid, pr, max(l, mid), r);
  }
}

void st_update(int node, int pl, int pr, int pos, int delta) {
  if (pr - pl == 1) {
    s[node] += delta;
  } else {
    int mid = (pl + pr) >> 1;
    if (pos < mid) {
      st_update(2 * node, pl, mid, pos, delta);
    } else {
      st_update(2 * node + 1, mid, pr, pos, delta);
    }
    s[node] = s[2 * node] + s[2 * node + 1];
  }
}

int main() {

  read_data();
  mark_time();

  // round n to the next power of 2
  int orig_n = n;
  while (n & (n - 1)) {
    n = (n | (n - 1)) + 1;
  }

  for (int i = 0; i < orig_n; i++) {
    s[n + i] = v[i + 1];
  }

  st_build();

  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == T_UPDATE) {
      st_update(1, 0, n, q[i].x - 1, q[i].y);
    } else {
      answer[num_answers++] = st_query(1, 0, n, q[i].x - 1, q[i].y);
    }
  }

  report_time("segment tree");
  write_data();

  return 0;
}
