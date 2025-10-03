// Complexity: O(Q log N)
//
// Method: segment tree with lazy updates, implemented iteratively.
// Invariant: s[node] + lazy[node] * size is the real sum.
#include "common.h"

struct segment_tree {
  long long s[MAX_SEGTREE_NODES];
  long long lazy[MAX_SEGTREE_NODES]; // quantity to add to every element spanned
  int bits;

  void init(long long* v, int _n) {
    n = next_power_of_2(_n);
    bits = 31 - __builtin_clz(n);
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

  void push_path(int node) {
    for (int b = bits, size = n; b; b--, size >>= 1) {
      int x = node >> b;
      s[x] += lazy[x] * size;
      lazy[2 * x] += lazy[x];
      lazy[2 * x + 1] += lazy[x];
      lazy[x] = 0;
    }
  }

  void pull_path(int node) {
    for (int x = node / 2, size = 1; x; x /= 2, size <<= 1) {
      s[x] =
        s[2 * x] + lazy[2 * x] * size +
        s[2 * x + 1] + lazy[2 * x + 1] * size;
    }
  }

  long long query(int l, int r) {
    long long sum = 0;
    int size = 1;

    l += n;
    r += n;
    push_path(l);
    push_path(r);

    while (l <= r)  {
      if (l & 1) {
        sum += s[l] + lazy[l] * size;
        l++;
      }
      l >>= 1;

      if (!(r & 1)) {
        sum += s[r] + lazy[r] * size;
        r--;
      }
      r >>= 1;
      size <<= 1;
    }

    return sum;
  }

  void update(int l, int r, int delta) {
    l += n;
    r += n;
    int lc = l, rc = r;
    push_path(l);
    push_path(r);

    while (l <= r)  {
      if (l & 1) {
        lazy[l++] += delta;
      }
      l >>= 1;

      if (!(r & 1)) {
        lazy[r--] += delta;
      }
      r >>= 1;
    }

    pull_path(lc);
    pull_path(rc);
  }
};

segment_tree st;

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      st.update(q[i].l - 1, q[i].r - 1, q[i].val);
    } else {
      answer[num_answers++] = st.query(q[i].l - 1, q[i].r - 1);
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
