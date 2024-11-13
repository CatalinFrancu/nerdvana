/**
 * Same as v3 with some possible optimizations.
 *
 * 1. Store bitsets on ints instead of chars.
 * 2. One-liners for bitset functions.
 * 3. Added inlines.
 */
#include <stdio.h>

const int MAX_N = 1'000'000;
const int MAX_VALUE = 1 << 22;

int a[MAX_N];
unsigned fit[MAX_VALUE / 32];
int n;

inline void fitSet(int pos) {
  fit[pos >> 5] |= 1 << (pos & 31);
}

inline bool fitGet(int pos) {
  return (fit[pos >> 5] >> (pos & 31)) & 1;
}

void readInputData() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
    fitSet(a[i]);
  }
}

void computeFits() {
  for (int mask = 1; mask < MAX_VALUE; mask++) {
    if (!fitGet(mask)) {
      // Make a copy of mask. Then isolate and clear one bit at a time.
      int copy = mask;
      bool fitFromSubmask = false;
      while (copy && !fitFromSubmask) {
        int singleBit = copy & -copy;
        int submask = mask ^ singleBit;
        fitFromSubmask = fitGet(submask);
        copy ^= singleBit;
      }
      if (fitFromSubmask) {
        fitSet(mask);
      }
    }
  }
}

// Find an input number that fits into mask.
int recoverNumber(int mask) {
  int copy = mask;
  while (copy) {
    int singleBit = copy & -copy;
    int submask = mask ^ singleBit;
    if (fitGet(submask)) {
      // Found a bit to remove. Reset the process.
      mask = copy = submask;
    } else {
      copy ^= singleBit;
    }
  }
  return mask;
}

void writeAnswers() {
  for (int i = 0; i < n; i++) {
    int neg = (MAX_VALUE - 1) ^ a[i];
    int answer = fitGet(neg)
      ? recoverNumber(neg)
      : -1;
    printf("%d ", answer);
  }
  printf("\n");
}

int main() {
  readInputData();
  computeFits();
  writeAnswers();

  return 0;
}
