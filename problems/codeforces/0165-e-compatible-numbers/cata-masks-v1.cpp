/**
 * Let fit[mask] be a number (any number) from among the given n whose binary
 * representation is a submask of mask. Obviously, when we read an input value
 * a[i] = x we can set fit[x] = x. For example, if the input array contains
 * the value 5 = 101b, then we could set fit[5] = fit[13] = fit[1029] = 5 (and
 * the same for any other index which has bits 0 and 2 set).
 *
 * We can fully compute fit[] by extending it one bit at a time. For a given
 * mask, try every submask obtained by clearing one bit from mask. If
 * fit[submask] is set, then set fit[mask] = fit[submask]. If no submask is
 * satisfactory, then leave fit[mask] unset.
 *
 * Once fit[] is complete we can generate the output array. For a given a[i] =
 * x, let mask be x's complement against 1111'1111'1111b. If fit[mask] is set,
 * then that is the answer. Otherwise print -1.
 */
#include <stdio.h>

const int MAX_N = 1'000'000;
const int MAX_BITS = 22;
const int MAX_VALUE = 1 << MAX_BITS;

int a[MAX_N];
int fit[1 << MAX_BITS];
int n;

void readInputData() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
    fit[a[i]] = a[i];
  }
}

void computeFits() {
  for (int mask = 1; mask < MAX_VALUE; mask++) {
    if (!fit[mask]) {
      // Make a copy of mask. Then isolate and clear one bit at a time.
      int copy = mask;
      int fitFromSubmask = 0;
      while (copy && !fitFromSubmask) {
        int singleBit = copy & -copy;
        int submask = mask ^ singleBit;
        fitFromSubmask = fit[submask];
        copy ^= singleBit;
      }
      fit[mask] = fitFromSubmask;
    }
  }
}

void writeAnswers() {
  for (int i = 0; i < n; i++) {
    int neg = (MAX_VALUE - 1) ^ a[i];
    int answer = fit[neg] ? fit[neg] : -1;
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
