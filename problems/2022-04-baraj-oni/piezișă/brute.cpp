// Simple brute-force algorithm to validate our preprocessing of partial xors.
#include <algorithm>
#include <stdio.h>

#define MAX_N 500000

int v[MAX_N + 1]; // partial xor's
int pos[MAX_N + 1];
int first[MAX_N + 2];
int n, distinct;

int max(int x, int y) {
  return (x > y) ? x : y;
}

int main() {
  scanf("%d", &n);

  // read data and compute partial xor's
  v[0] = 0;
  for (int i = 1; i <= n; i++) {
    int x;
    scanf("%d", &x);
    v[i] = v[i - 1] ^ x;
  }

  // sort positions by partial xor value, then by position
  for (int i = 0; i <= n; i++) {
    pos[i] = i;
  }
  std::sort(pos, pos + n + 1, [](int a, int b) {
    return (v[a] < v[b]) || ((v[a] == v[b]) && (a < b));
  });

  // renumber values starting from n; collect positions
  int from = -1;
  distinct = 0;
  for (int i = 0; i <= n; i++) {
    if (v[pos[i]] != from) {
      from = v[pos[i]];
      first[distinct++] = i;
    }
    v[pos[i]] = distinct - 1;
  }
  first[distinct] = n + 1;
  // Now pos[first[i]...first[i+1]] contains a sorted list of positions where
  // value i occurs.

  int num_queries;
  scanf("%d", &num_queries);
  while (num_queries--) {
    int l, r;
    scanf("%d %d", &l, &r);
    r++;

    int extra = 0, found = false;
    while ((r - l + extra < n) && !found) {
      int start = max(l - extra, 0), end = start + r - l + extra;
      while ((start <= l) && (end <= n) && !found) {
        found = v[start] == v[end];
        start++;
        end++;
      }

      extra++;
    }

    printf("%d\n", found ? (r - l + extra - 1) : -1);
  }

  return 0;
}
