// Method: For the maximum number of fruit, run the classic dynamic
// programming algorithm (a[i][j] = max(a[i-1][j], a[i][j-1]) and so on), but
// on a sparse matrix. Scan fruit in row order, breaking ties in column
// order. For every fruit, the most we can collect up to it is 1 + the maximum
// in its north-west quadrant. Use a Fenwick tree to update the maxima.
//
// For the minimum difference, have each fruit store the possible numbers of
// pears we can collect up to it, among all maximal paths. If we had time and
// space, we could implement these as bit sets. To save time and space, we
// keep a list of intervals for each fruit. So [3,6], [8,9] means we can get
// 3, 4, 5, 6, 8 or 9 pears. Thus, the fruit struct has a pointer (.st) into
// the array of intervals (.in).
//
// The intervals for a fruit F are the union of intervals of fruit to its
// north-west and with max value one less than F. If F is a pear, we shift all
// the numbers up by 1. In practice the number of intervals in the union is
// small (the most I encountered with random tests was 4).
//
// A fruit can have lots of fruit to its north-west, so we use square root
// decomposition. Again, each bucket's union is a collection of disjoint
// intervals, so b[] stores pointers into an array of intervals. We process
// fruit in batches with equal q. From those unions we build the batch for
// q+1.
//
// Two nasty bugs up until submission 6011: (1) printing ints as %hd and (2)
// defining MAX_BUCKETS as sqrt(MAX_N), when it really should be
// sqrt(MAX_INTERVALS).
//
// An apple, a pear, a plum, a cherry, any good thing to make us all merry.
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define MAX_N 35'000
#define MAX_BUCKETS 6'000
#define MAX_INTERVALS 30'000'000
#define MAX_BUCKET_INTERVALS 5'000'000

typedef unsigned short u16;

typedef struct {
  u16 l, r;         // a semi-open interval [l, r)
} interval;

typedef struct {
  u16 r, c, type;   // row, column, type
  u16 q;            // quantity of fruit that we can collect
  int st;           // pointer into in (start of the collectible intervals)
} fruit;

u16 fen[MAX_N + 1]; // Fenwick tree
fruit f[MAX_N + 1];
interval in[MAX_INTERVALS], bin[MAX_BUCKET_INTERVALS];
int b[MAX_BUCKETS + 1]; // bucket pointers
int n;
int ni, nb, bs;   // number of intervals, buckets and sizes for decomposition

int max(int x, int y) {
  return (x > y) ? x : y;
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

// max update: fen[pos] increases to val
void fenwick_update(int pos, u16 val) {
  while (pos <= n && fen[pos] < val) {
    fen[pos] = val;
    pos += pos & -pos;
  }
}

// partial max: returns max(val[1...pos])
u16 fenwick_max(int pos) {
  u16 m = 0;
  while (pos) {
    m = max(m, fen[pos]);
    pos &= pos - 1;
  }
  return m;
}

// computes the number of fruit collectible up to each fruit
void compute_max_fruit() {
  std::sort(f, f + n, [](fruit a, fruit b) {
    return (a.r < b.r) || (a.r == b.r && a.c < b.c);
  });

  for (int i = 0; i < n; i++) {
    f[i].q = 1 + fenwick_max(f[i].c);
    fenwick_update(f[i].c, f[i].q);
  }
}

// When the intervals of the previous batch are no longer needed, moves the
// intervals of [start, end) to position 0 in in[].
void move_intervals(int start, int end) {
  int delta = f[start].st;
  for (int i = 0; i < ni - delta; i++) {
    in[i] = in[i + delta];
  }
  for (int i = start; i < end; i++) {
    f[i].st -= delta;
  }
  ni -= delta;
}

// Computes the union of intervals src1[0...count1), src2[0...count2) and
// src3[0...count3) into dest[0...). src2 and src3 may be null, in which case
// count2 and count3 should be 0. Returns the number of intervals in the
// union.
int find_union(interval* src1, int count1,
               interval* src2, int count2,
               interval* src3, int count3,
               interval* dest, int shift) {
  // copy all the intervals and sort them
  memcpy(dest, src1, count1 * sizeof(interval));
  memcpy(dest + count1, src2, count2 * sizeof(interval));
  memcpy(dest + count1 + count2, src3, count3 * sizeof(interval));
  int total = count1 + count2 + count3;
  std::sort(dest, dest + total, [](interval a, interval b) {
    return a.l < b.l;
  });

  // take the first interval automatically, then incorporate the rest
  int size = 1; // size of the union so far
  for (int i = 1; i < total; i++) {
    if (dest[i].l > dest[size - 1].r) {
      dest[size++] = dest[i]; // start a new disjoint interval
    } else {
      dest[size - 1].r = max(dest[size - 1].r, dest[i].r);
    }
  }

  // shift everything right by one for pears
  if (shift) {
    for (int i = 0; i < size; i++) {
      dest[i].l++;
      dest[i].r++;
    }
  }

  return size;

}

// We only need three sources for queries spanning multiple buckets. For
// single-bucket queries, one source suffices.
int find_union_single(interval* src, int count, interval* dest, int shift) {
  return find_union(src, count, NULL, 0, NULL, 0, dest, shift);
}

// precomputes unions of chunks of intervales sized sqrt(ni)
void sqrt_build() {
  nb = sqrt(ni);
  bs = (ni - 1) / nb + 1;
  int size = 0;

  for (int i = 0; i < nb; i++) {
    b[i] = size;
    int start = i * bs, end = min(start + bs, ni);
    size += find_union_single(in + start, end - start, bin + size, 0);
  }

  b[nb] = size;
}

// computes the choices for the given fruit as the union of in[from...to)
void sqrt_query(int fruit, int from, int to) {
  f[fruit].st = ni;

  int bf = from / bs, bt = to / bs;

  if (bf == bt) {
    // single bucket
    ni += find_union_single(in + from, to - from, in + ni, f[fruit].type);
  } else {
    // loose ends left and right plus precomputed buckets
    ni += find_union(in + from, (bf + 1) * bs - from,
                     bin + b[bf + 1], b[bt] - b[bf + 1],
                     in + bt * bs, to - bt * bs,
                     in + ni, f[fruit].type);
  }
}

// Process the fruit batch [start, end). They all have the same q value, which
// is either 1 or one more than the previous batch, [prev, start).
void process_batch(int start, int end, int prev) {
  if (f[start].q == 1) {

    // Base case. Pears get exactly 1 pears. Apples get exactly 0 pears.
    for (int i = start; i < end; i++) {
      f[i].st = ni;
      in[ni++] = { f[i].type, (u16)(f[i].type + 1) };
    }

  } else {

    // Recurrent case. Get intervals by taking appropriate unions of
    // intervals from [prev, start).
    int l = prev, r = prev;
    for (int i = start; i < end; i++) {
      // find the first and last fruit which are dominated by i
      while ((l < start) && (f[l].r > f[i].r)) {
        l++;
      }
      while ((r < start) && (f[r].c <= f[i].c)) {
        r++;
      }

      sqrt_query(i, f[l].st, f[r].st);
    }

    move_intervals(start, end);

  }

  f[end].st = ni;
  sqrt_build();
}

// returns the minimum difference between apples and pears given by this interval
int dist(interval in, int target) {
  // c1 and c2 are distinct for odd target, identical for even target
  int c1 = target / 2, c2 = (target + 1) / 2;

  if ((in.l <= c2) && (in.r > c1)) {
    return target % 2; // in spans c1 or c2 or both
  } else if (in.l > c2) {
    return 2 * in.l - target;
  } else {
    return target - 2 * (in.r - 1);
  }
}

int main() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%hd %hd %hd", &f[i].r, &f[i].c, &f[i].type);
    // 0 for apples, 1 for pears
    f[i].type--;
  }

  compute_max_fruit();

  // sort by q value, then identify and process batches of equal q value
  std::sort(f, f + n, [](fruit a, fruit b) {
    return (a.q < b.q) || (a.q == b.q && a.c < b.c);
  });

  f[n].q = f[n - 1].q + 1; // sentinel
  int start = 0, prev = 0;

  while (start < n) {
    int end = start;
    while (f[end].q == f[start].q) {
      end++;
    }

    process_batch(start, end, prev);
    prev = start;
    start = end;
  }

  // for the highest q value, find the minimum difference
  int max_fruit = f[prev].q, min_diff = MAX_N;
  for (int i = f[prev].st; i < ni; i++) {
    min_diff = min(min_diff, dist(in[i], max_fruit));
  }

  printf("%d %d\n", max_fruit, min_diff);
  return 0;
}
