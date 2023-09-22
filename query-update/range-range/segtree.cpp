// Complexity: O(Q log N)
//
// Method: segment tree with lazy updates
// Invariant: s[node] + lazy[node] * size is the real sum.
#include "common.h"

long long s[2 * MAX_N];
long long lazy[2 * MAX_N]; // quantity to add to every element spanned

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

void st_push(int node, int size) {
  s[node] += lazy[node] * size;
  lazy[2 * node] += lazy[node];
  lazy[2 * node + 1] += lazy[node];
  lazy[node] = 0;
}

long long st_query(int node, int pl, int pr, int l, int r) {
  if (l >= r) {
    return 0;
  } else if ((l == pl) && (r == pr)) {
    return s[node] + lazy[node] * (r - l);
  } else {
    st_push(node, pr - pl);
    int mid = (pl + pr) >> 1;
    return
      st_query(node * 2, pl, mid, l, min(r, mid)) +
      st_query(node * 2 + 1, mid, pr, max(l, mid), r);
  }
}

void st_update(int node, int pl, int pr, int l, int r, int delta) {
  if (l >= r) {
    return;
  } else if ((l == pl) && (r == pr)) {
    lazy[node] += delta;
  } else {

    st_push(node, pr - pl);
    int mid = (pl + pr) >> 1, child_size = (pr - pl) >> 1;
    st_update(2 * node, pl, mid, l, min(r, mid), delta);
    st_update(2 * node + 1, mid, pr, max(l, mid), r, delta);
    s[node] =
      s[2 * node] + lazy[2 * node] * child_size +
      s[2 * node + 1] + lazy[2 * node + 1] * child_size;

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
    if (q[i].t == OP_UPDATE) {
      st_update(1, 0, n, q[i].l - 1, q[i].r, q[i].val);
    } else {
      answer[num_answers++] = st_query(1, 0, n, q[i].l - 1, q[i].r);
    }
  }

  report_time("segment tree");
  write_data();

  return 0;
}
