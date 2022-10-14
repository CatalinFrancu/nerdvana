// Autor: Cătălin Frâncu
// Complexitate: O(QN)
//
// Metodă: Forță brută.
#include <stdio.h>

#define MAX_N 300'000

#define T_TOGGLE 1
#define T_COUNT 2

char v[MAX_N];

int main() {
  int n, q, t, l, r;

  FILE* fin = fopen("shperl.in", "r");
  FILE* fout = fopen("shperl.out", "w");
  fscanf(fin, "%d %d ", &n, &q);
  fread(v + 1, 1, n, fin);
  for (int i = 1; i <= n; i++) {
    v[i] -= '0';
  }

  while (q--) {
    fscanf(fin, "%d %d %d", &t, &l, &r);
    if (t == T_TOGGLE) {
      while (l <= r) {
        v[l] = !v[l];
        l++;
      }
    } else {
      int sum = 0;
      while (l <= r) {
        sum += v[l++];
      }
      fprintf(fout, "%d\n", sum);
    }
  }

  fclose(fin);
  fclose(fout);

  return 0;
}
