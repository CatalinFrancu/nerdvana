// Method: Like dp-intervals.cpp, but stores a bitset for each fruit
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define MAX_N 35'000
#define MAX_U64 547 // ceil(MAX_N/64)

typedef unsigned long long u64;

typedef struct {
  int r, c, type;
  int q;
  u64 bs[MAX_U64];
} fruit;

int fen[MAX_N + 1]; // Fenwick tree
fruit f[MAX_N + 1];
int n;

int max(int x, int y) {
  return (x > y) ? x : y;
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

// max update: fen[pos] increases to val
void fenwick_update(int pos, int val) {
  while (pos <= n && fen[pos] < val) {
    fen[pos] = val;
    pos += pos & -pos;
  }
}

// partial max: returns max(val[1...pos])
int fenwick_max(int pos) {
  int m = 0;
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

// gets bit number pos
int bs_get(u64* bs, int pos) {
  return (bs[pos >> 6] >> (pos & 63)) & 1;
}

// sets bit number pos
void bs_set(u64* bs, int pos) {
  bs[pos >> 6] |= 1 << (pos & 63);
}

// dest |= src from bit 0 to bit size inclusively
void bs_union(u64* dest, u64* src, int size) {
  int last_word = (1 + size) >> 6;
  for (int i = 0; i <= last_word; i++) {
    dest[i] |= src[i];
  }
}

// shifts everything up by one, up to bit size inclusively
void bs_shift(u64* bs, int size)  {
  int last_word = (2 + size) >> 6;
  for (int i = last_word; i >= 1; i--) {
    bs[i] = (bs[i] << 1) + (bs[i - 1] >> 63);
  }
  bs[0] = bs[0] << 1;
}

// returns the minimum difference between apples and pears given by this bitset
int bs_dist(u64* bs, int target) {
  // c1 and c2 are distinct for odd target, identical for even target
  int c1 = target / 2, c2 = (target + 1) / 2;

  int result = 0;
  while (!bs_get(bs, c1) && !bs_get(bs, c2)) {
    c1--;
    c2++;
    result++;
  }

  return c2 - c1;
}

// Process the fruit batch [start, end). They all have the same q value, which
// is either 1 or one more than the previous batch, [prev, start).
void process_batch(int start, int end, int prev) {
  if (f[start].q == 1) {

    // Base case. Pears get exactly 1 pears. Apples get exactly 0 pears.
    for (int i = start; i < end; i++) {
      bs_set(f[i].bs, f[i].type);
    }

  } else {

    int l = prev;
    for (int i = start; i < end; i++) {
      // find the first fruit which is dominated by i (there has to be one)
      while (f[l].r > f[i].r) {
        l++;
      }

      // merge bitsets of dominated fruit into f[i].bs
      int r = l;
      while ((r < start) && (f[r].c <= f[i].c)) {
        bs_union(f[i].bs, f[r++].bs, f[i].q);
      }

      if (f[i].type) {
        bs_shift(f[i].bs, f[i].q);
      }
    }

  }
}

int main() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d %d %d", &f[i].r, &f[i].c, &f[i].type);
    // 0 for apples, 1 for pears
    f[i].type--;
  }

  compute_max_fruit();

  // sort by q value, then identify and process batches of equal q value
  std::sort(f, f + n, [](fruit a, fruit b) {
    return (a.q < b.q) || (a.q == b.q && a.c < b.c);
  });

  int start = 0, prev = 0;
  f[n].q = n + 1;
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
  for (int i = prev; i < n; i++) {
    min_diff = min(min_diff, bs_dist(f[i].bs, max_fruit));
  }

  printf("%d %d\n", max_fruit, min_diff);
  return 0;
}
