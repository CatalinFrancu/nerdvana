// Method: Square root decomposition. Compute partial xors and renumber them
// so that we can index them. Store ordered lists of positions for every
// distinct value. Divide the array into B buckets of size S. For every pair
// of buckets (l, r) precompute the tightest interval between any elements in
// buckets [0...l] and [r...B-1]. Now for a query [l,r] handle three cases:
//
// 1. Full buckets to the left and right: use the precomputed table.
//
// 2. Loose elements on the left: look up each element in the ordered list of
// positions past r, using binary search.
//
// 3. Loose elements on the right: mirror of (2).
#include <algorithm>
#include <math.h>
#include <stdio.h>

#define MAX_BUCKETS 2832
#define MAX_BUCKET_SIZE 177
#define MAX_N (MAX_BUCKETS * MAX_BUCKET_SIZE)
#define INF (MAX_N + 1)
#define NONE -1

int v[MAX_N + 1]; // partial xor's
int pos[MAX_N + 1];
int first[MAX_N + 2];
int last[MAX_N + 1];

// bb[l][r] = the tightest interval between the buckets [0...l] and [r...nb-1]
int bb[MAX_BUCKETS][MAX_BUCKETS];
int n, distinct;
int bs, nb;

inline int max(int x, int y) {
  return (x > y) ? x : y;
}

inline int min(int x, int y) {
  return (x < y) ? x : y;
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

// Returns the leftmost occurence of val at or after position p, or NONE if
// val does not occur at or after p.
int leftmost(int val, int p) {
  int l = first[val] - 1, r = first[val + 1] - 1; // (l, r]
  while (r - l > 1) {
    int m = (l + r) >> 1;
    if (pos[m] < p) {
      l = m;
    } else {
      r = m;
    }
  }

  // Special case: p > every position where val occurs. Regular binary search
  // would return pos[r].
  return (pos[r] >= p) ? pos[r] : NONE;
}

void preprocess_values() {
  // sort positions by partial xor value, then by position
  for (int i = 0; i < n; i++) {
    pos[i] = i;
  }
  std::sort(pos, pos + n, [](int a, int b) {
    return (v[a] < v[b]) || ((v[a] == v[b]) && (a < b));
  });

  // renumber values starting from 0; collect positions
  int from = -1;
  distinct = 0;
  for (int i = 0; i < n; i++) {
    if (v[pos[i]] != from) {
      from = v[pos[i]];
      first[distinct++] = i;
    }
    v[pos[i]] = distinct - 1;
  }
  first[distinct] = n;
  // Now pos[first[i]...first[i+1]] contains a sorted list of positions where
  // value i occurs.
}

void preprocess_buckets() {
  for (int i = 0; i < distinct; i++) {
    last[i] = NONE;
  }
  for (int l = 0; l < nb; l++) {
    // add bucket l to last[]
    for (int i = l * bs; i < (l + 1) * bs; i++) {
      last[v[i]] = i;
    }

    // iterate buckets from nb - 1 to l + 2
    for (int r = nb - 1; r >= l + 2; r--) {
      // inherit the answer from b[l][r + 1]
      bb[l][r] = (r == nb - 1) ? INF : bb[l][r + 1];
      int start = r * bs, end = (r + 1) * bs - 1;
      for (int i = end; i >= start; i--) {
        if (last[v[i]] != NONE) {
          bb[l][r] = min(bb[l][r], i - last[v[i]]);
        }
      }
    }
  }
}

int query(int l, int r) {
  int bl = l / bs, br = r / bs;
  int result = INF;

  if ((bl > 0) && (br < nb - 1)) {
    result = bb[bl - 1][br + 1];
  }

  for (int i = bl * bs; i <= l; i++) {
    int other = leftmost(v[i], r);
    if (other != NONE) {
      result = min(result, other - i);
    }
  }

  for (int i = (br + 1) * bs - 1; i >= r; i--) {
    int other = rightmost(v[i], l);
    if (other != NONE) {
      result = min(result, i - other);
    }
  }

  return (result == INF) ? NONE : result;
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

  nb = 4 * sqrt(n + 1);
  bs = n / nb + 1;

  // pad with values identical to v[n] so that they will never be the answer
  while (n < nb * bs) {
    v[n + 1] = v[n];
    n++;
  }

  preprocess_values();
  preprocess_buckets();

  int num_queries;
  scanf("%d", &num_queries);
  while (num_queries--) {
    int l, r;
    scanf("%d %d", &l, &r);
    printf("%d\n", query(l, r + 1));
  }

  return 0;
}
