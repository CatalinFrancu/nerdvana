// Autor: Cătălin Frâncu
// Complexitate: O(QN), constantă mică
//
// Metodă: Forță brută, dar citește / neagă valorile cîte 64 deodată.
#include <stdio.h>

#define MAX_N 300'000

#define T_TOGGLE 1
#define T_COUNT 2

// pentru conversia de la indice în vector la word + bit
#define SHIFT 6
#define MASK 0x3f

typedef unsigned long long u64;

char buf[MAX_N];
u64 bs[1 + (MAX_N >> SHIFT)];

inline int popcount_partial(u64 x, int l, int r) {
  // caz particular: (1ull << 64) - 1 nu merge
  if (l > 0 || r < 64) {
    x >>= l;
    x &= (1ull << (r - l)) - 1;
  }

  return __builtin_popcountll(x);
}

int range_popcount(int l, int r) {
  int word_l = l >> SHIFT, bit_l = l & MASK;
  int word_r = r >> SHIFT, bit_r = r & MASK;

  if (word_l ==  word_r) {
    // toți biții sînt în aceeași variabilă
    return popcount_partial(bs[word_l], bit_l, bit_r);
  } else {
    // adună începutul, sfîrșitul, și toate variabilele complete din mijloc
    int sum = popcount_partial(bs[word_l], bit_l, 64);
    sum += popcount_partial(bs[word_r], 0, bit_r);
    for (int i = word_l + 1; i < word_r; i++) {
      sum += __builtin_popcountll(bs[i]);
    }
    return sum;
  }
}

inline void toggle(int pos) {
  bs[pos] = ~bs[pos];
}

inline void toggle_partial(int pos, int l, int r) {
  if (l == 0 && r == 64) {
    toggle(pos);
  } else {
    u64 mask = ((1ull << (r - l)) - 1) << l;
    bs[pos] ^= mask;
  }
}

void range_toggle(int l, int r) {
  int word_l = l >> SHIFT, bit_l = l & MASK;
  int word_r = r >> SHIFT, bit_r = r & MASK;

  if (word_l ==  word_r) {
    toggle_partial(word_l, bit_l, bit_r);
  } else {
    toggle_partial(word_l, bit_l, 64);
    toggle_partial(word_r, 0, bit_r);
    for (int i = word_l + 1; i < word_r; i++) {
      toggle(i);
    }
  }
}

int main() {
  int n, q, t, l, r;

  FILE* fin = fopen("shperl.in", "r");
  FILE* fout = fopen("shperl.out", "w");
  fscanf(fin, "%d %d ", &n, &q);
  fread(buf, 1, n, fin);
  for (int i = 0; i < n; i++) {
    if (buf[i] == '1') {
      bs[i >> SHIFT] |= 1ull << (i & MASK);
    }
  }

  while (q--) {
    fscanf(fin, "%d %d %d", &t, &l, &r);
    l--; // vom lucra pe intervalul [l,r), indexat de la 0
    if (t == T_TOGGLE) {
      range_toggle(l, r);
    } else {
      fprintf(fout, "%d\n", range_popcount(l, r));
    }
  }

  fclose(fin);
  fclose(fout);

  return 0;
}
