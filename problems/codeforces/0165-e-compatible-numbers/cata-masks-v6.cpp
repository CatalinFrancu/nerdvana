// Same as v5, but stores a single bit and recovers answers later.
#include <stdio.h>

const int MAX_N = 1'000'000;
const int MAX_BITS = 22;
const int MAX_VALUE = 1 << MAX_BITS;

struct bitset {
  unsigned v[MAX_VALUE / 32];

  void set(int pos) {
    v[pos >> 5] |= 1 << (pos & 31);
  }

  bool get(int pos) {
    return v[pos >> 5] >> (pos & 31) & 1;
  }
};

int a[MAX_N];
bitset fit;
int n;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
    fit.set(a[i]);
  }
}

void compute_fits() {
  for (int bit = 0; bit < MAX_BITS; bit++) {
    for (int mask = 1; mask < MAX_VALUE; mask++) {
      if ((mask & (1 << bit)) && fit.get(mask ^ (1 << bit))) {
        fit.set(mask);
      }
    }
  }
}

// Find an input number that fits into mask.
int recover_number(int mask) {
  int copy = mask;
  while (copy) {
    int singleBit = copy & -copy;
    int submask = mask ^ singleBit;
    if (fit.get(submask)) {
      // Found a bit to remove. Reset the process.
      mask = copy = submask;
    } else {
      copy ^= singleBit;
    }
  }
  return mask;
}

void write_answers() {
  for (int i = 0; i < n; i++) {
    int neg = (MAX_VALUE - 1) ^ a[i];
    int answer = fit.get(neg)
      ? recover_number(neg)
      : -1;
    printf("%d ", answer);
  }
  printf("\n");
}

int main() {
  read_data();
  compute_fits();
  write_answers();

  return 0;
}
