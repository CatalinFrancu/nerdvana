#include <assert.h>
#include <stdio.h>

#define MAX_N 2'000
#define MAX_Q 100'000

bool a[MAX_N + 1][MAX_N + 1];

int main() {
  int n, q;

  scanf("%d %d", &n, &q);

  assert(n >= 3);
  assert(n <= MAX_N);

  assert(q >= 1);
  assert(q <= MAX_Q);

  while (q--) {
    int t, l1, c1, l2, c2;
    assert(scanf("%d %d %d %d %d\n", &t, &l1, &c1, &l2, &c2) == 5);
    assert((t == 1) || (t == 2));
    if (t == 1) {
      assert((1 <= l1) && (l1 < l2) && (l2 <= n));
      assert((1 <= c1) && (c1 < c2) && (c2 <= n));
      for (int i = l1; i <= l2; i++) {
        assert(!a[i][c1]);
        assert(!a[i][c2]);
        a[i][c1] = a[i][c2] = true;
      }
      for (int i = c1 + 1; i < c2; i++) {
        assert(!a[l1][i]);
        assert(!a[l2][i]);
        a[l1][i] = a[l2][i] = true;
      }
    } else {
      assert((l1 != l2) || (c1 != c2));
    }
  }


  return 0;
}
