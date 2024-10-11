#include <stdio.h>

const int MAX_VAL = 1'000'000;

// spf[x] = smallest divisor of x
int spf[MAX_VAL + 1];
int mobius[MAX_VAL + 1];
int freq[MAX_VAL + 1];

void read_data() {
  int n, x;
  scanf("%d", &n);
  while (n--) {
    scanf("%d", &x);
    freq[x]++;
  }
}

void compute_smallest_factors() {
  spf[1] = 1;
  for (int i = 2; i <= MAX_VAL; i++) {
    if (!spf[i])  {
      for (int j = i; j <= MAX_VAL; j += i) {
        if (!spf[j]) {
          spf[j] = i;
        }
      }
    }
  }
}

void compute_mobius() {
  mobius[1] = 1;
  for (int i = 2; i <= MAX_VAL; i++) {
    if (spf[i] == spf[i / spf[i]]) {
      mobius[i] = 0;
    } else {
      mobius[i] = -mobius[i / spf[i]];
    }
  }
}

int count_multiples(int d) {
  int result = 0;
  for (int i = d; i <= MAX_VAL; i += d) {
    result += freq[i];
  }
  return result;
}

long long count_coprime_pairs() {
  long long result = 0;

  for (int d = 1; d <= MAX_VAL; d++) {
    if (mobius[d]) { // speed optimization
      long long num_mult = count_multiples(d);
      result += mobius[d] * num_mult * (num_mult - 1) / 2;
    }
  }

  return result;
}

void write_answer(long long answer) {
  printf("%lld\n", answer);
}

int main() {
  read_data();
  compute_smallest_factors();
  compute_mobius();
  long long answer = count_coprime_pairs();
  write_answer(answer);

  return 0;
}

// Consider the solution based on inclusion-exclusion. For every number x and
// for each of its divisors d it will ask: how many multiples of d have I seen
// before? It will then add that count to a global total, with a +/- sign
// depending on d's number of prime factors (aka the Mobius function).
//
// Let us now tally the same counts not by number, but by divisor. Divisor d
// will come up for every multiple of d. First it will count 0 previous
// numbers, then 1, then 2 and so on, always with the same sign. Overall,
// divisor d will contribute k * (k-1) / 2 * mobius[d], where k is the number
// of multiples of d among the n input numbers.
