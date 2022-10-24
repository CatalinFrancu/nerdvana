// Method: Square root decomposition. Compute partial xors and renumber them
// so they become indexable. Sort queries by increasing left end. Scan the
// array from left to right. Compute two pieces of information at each
// position l:
//
// (1) last[x] = rightmost position before l of every element x.
// (2) best[b] = tightest interval between [0...l] and every element in bucket b.
//
// With this information we can answer queries starting at l in O(sqrt(n)):
// handle full buckets in O(1) per bucket and loose elements in O(1) per
// element.
//
// When advancing from l to l + 1, last[] can easily be updated in O(1). We
// can also update each bucket in O(1). This requires pointers from l + 1 to
// the first element in each bucket that is equal to v[l + 1]. In general we
// precompute, for every position i, a pointer to the first element j > i such
// that v[j] = v[i] and j is not in i's bucket.
#include <algorithm>
#include <math.h>
#include <stdio.h>

#define MAX_BUCKETS 354
#define MAX_BUCKET_SIZE 1416
#define MAX_N (MAX_BUCKETS * MAX_BUCKET_SIZE)
#define MAX_Q 500000
#define INF (MAX_N + 1)
#define NONE -1

typedef struct {
  int l, r, ord, answer;
} query;

int v[MAX_N + 1]; // partial xor's
int pos[MAX_N + 1];
int ptr[MAX_N + 1];
int last[MAX_N + 1];
int best[MAX_BUCKETS];
query q[MAX_Q];

int n, num_queries;
int bs, nb;

inline int min(int x, int y) {
  return (x < y) ? x : y;
}

void preprocess_values() {
  
  // sort positions by partial xor value, then by decreasing position
  for (int i = 0; i <= n; i++) {
    pos[i] = i;
  }
  std::sort(pos, pos + n + 1, [](int a, int b) {
    return (v[a] < v[b]) || ((v[a] == v[b]) && (a > b));
  });

  // renumber values starting from 0; create pointers
  int prev = -1, distinct = 0;
  for (int i = 0; i <= n; i++) {
    if (v[pos[i]] == prev) {
      v[pos[i]] = distinct - 1;
      bool same_bucket = (pos[i] / bs == pos[i - 1] / bs);
      ptr[pos[i]] = same_bucket
        ? ptr[pos[i - 1]]
        : pos[i - 1];
    } else {
      // begin new streak from the end of the array
      prev = v[pos[i]];
      v[pos[i]] = distinct++;
      ptr[pos[i]] = NONE;
    }
  }
}

int answer_query(int l, int r) {
  int result = INF;

  // go incrementally until the next bucket
  int b = r / bs + 1, boundary = min(b * bs, n + 1);
  while (r < boundary) {
    result = min(result, r - last[v[r]]);
    r++;
  }

  // go bucket by bucket for the remainder of the array
  while (b < nb) {
    result = min(result, best[b++]);
  }

  return (result == INF) ? NONE : result;
}

void scan() {
  for (int i = 0; i <= n; i++) {
    last[i] = -INF;
  }
  for (int i = 0; i < nb; i++) {
    best[i] = INF;
  }

  int qi = 0;
  for (int l = 0; l <= n; l++) {
    // update last
    last[v[l]] = l;

    // update best
    for (int i = ptr[l]; i != NONE; i = ptr[i]) {
      int b = i / bs;
      best[b] = min(best[b], i - l);
    }

    // answer queries starting at l
    while (qi < num_queries && q[qi].l == l) {
      q[qi].answer = answer_query(q[qi].l, q[qi].r);
      qi++;
    }
  }
}

int main() {
  scanf("%d", &n);

  // read the array and compute partial xor's
  v[0] = 0;
  for (int i = 1; i <= n; i++) {
    scanf("%d", &v[i]);
    v[i] ^= v[i - 1];
  }

  nb = 0.5 * sqrt(n + 1); // determined experimentally
  bs = n / nb + 1;

  preprocess_values();

  // read the queries and sort them by their left end
  scanf("%d", &num_queries);
  for (int i = 0; i < num_queries; i++) {
    scanf("%d %d", &q[i].l, &q[i].r);
    q[i].r++;
    q[i].ord = i;
  }
  std::sort(q, q + num_queries, [](query a, query b) {
    return (a.l < b.l);
  });

  scan();

  std::sort(q, q + num_queries, [](query a, query b) {
    return (a.ord < b.ord);
  });

  for (int i = 0; i < num_queries; i++) {
    printf("%d\n", q[i].answer);
  }

  return 0;
}
