// A smarter brute force algorithm. Find the rightmost occurrence of v[r]
// before or at position l. Let us assume we find it at position q. This gives
// us a solution of size r - q. Do the same for positions r+1, r+2 and so
// on. How far do we need to go? No further than r + (l - q), because that
// would yield worse (longer) solutions than the one we already have.
#include <algorithm>
#include <stdio.h>

#define MAX_N 500000
#define INFINITY (MAX_N + 1)
#define NONE -1

int v[MAX_N + 1]; // partial xor's
int pos[MAX_N + 1];
int first[MAX_N + 2];
int n, distinct;

int max(int x, int y) {
  return (x > y) ? x : y;
}

// Returns the rightmost occurence of val at or before position p, or NONE if
// val does not occur at or before p.
int rightmost(int val, int p) {
  int l = first[val], r = first[val + 1]; // [l, r)
  while (r - l > 1) {
    int m = (l + r) >> 1;
    if (pos[m] > p) {
      r = m;
    } else {
      l = m;
    }
  }

  // Special case: p < every position where val occurs. Regular binary search
  // would return pos[l].
  return (pos[l] <= p) ? pos[l] : NONE;
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

    int end = r, best = INFINITY;
    // while we have room to advance, and hopes to improve the existing solution
    while ((end <= n) && (end - l < best)) {
      int start = rightmost(v[end], l);
      if ((start != NONE) && (end - start < best)) {
        best = end - start;
      }
      end++;
    }

    printf("%d\n", (best == INFINITY) ? NONE : best);      
  }

  return 0;
}
