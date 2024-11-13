/**
 * Let fit[mask] be a boolean indicating that there exists a number among the
 * given n whose binary representation is a submask of mask. Obviously, when
 * we read an input value a[i] = x we can set fit[x] = true. For example, if
 * the input array contains the value 5 = 101b, then we could set fit[5] =
 * fit[13] = fit[1029] = true (and the same for any other index which has bits
 * 0 and 2 set).
 *
 * We can fully compute fit[] by extending it one bit at a time. For a given
 * mask, try every submask obtained by clearing one bit from mask. If
 * fit[submask] is set, then set fit[mask] = true. If no submask is
 * satisfactory, then fit[mask] = false.
 *
 * Once fit[] is complete we can generate the output array. For a given a[i] =
 * x, let mask be x's complement against 1111'1111'1111b. If fit[mask] is set,
 * then there exists a compatible number. But which one is it? To recover the
 * answer start subtracting bits from mask while it is possible to find a
 * submask such that fit[submask] = true. When that is no longer possible, the
 * remaining mask must have been an input number.
 */
#include <stdio.h>

const int MAX_N = 1'000'000;
const int MAX_VALUE = 1 << 22;

typedef unsigned char u8;

int a[MAX_N];
u8 fit[MAX_VALUE / 8];
int n;

void fitSet(int pos, bool val) {
  u8& c = fit[pos >> 3];
  int shift = pos & 7;
  u8 mask = 1 << shift;
  c = (c & ~mask) | (val << shift);
}

bool fitGet(int pos) {
  u8& c = fit[pos >> 3];
  int shift = pos & 7;
  return (c >> shift) & 1;
}

void readInputData() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
    fitSet(a[i], true);
  }
}

void computeFits() {
  for (int mask = 1; mask < MAX_VALUE; mask++) {
    // Make a copy of mask. Then isolate and clear one bit at a time.
    int copy = mask;
    bool fitFromSubmask = fitGet(mask);
    while (copy && !fitFromSubmask) {
      int singleBit = copy & -copy;
      int submask = mask ^ singleBit;
      fitFromSubmask = fitGet(submask);
      copy ^= singleBit;
    }
    fitSet(mask, fitFromSubmask);
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
