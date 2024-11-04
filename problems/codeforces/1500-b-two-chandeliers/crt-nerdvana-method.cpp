// Instead of the given anger, we work with a patience value which is one less
// than the anger value. We rephrase the question as "How many days will this
// patience last Vasya?". The original answer is the rephrased answer +1.
//
// The chandeliers have a period of P=[m, n] days. During these P days there
// will be some overlaps, but the rest of the days will consume patience.
// Therefore, we can compute the whole number of periods, then work on the
// last, incomplete period.
//
// For the last period, collect the days when chandeliers match using the
// Chinese Remainder Theorem. Then sort the overlapping days and run a linear
// search to find how long the remaining patience will last.
#include <algorithm>
#include <numeric>
#include <stdio.h>

const int MAX_N = 500'000;
const int MAX_VAL = 2 * MAX_N;
const int NONE = -1;

struct chandelier {
  int n;
  int c[MAX_N];
};

chandelier p, q;
int pos_in_q[MAX_VAL + 1];
long long overlap[MAX_N];
int num_overlaps;
long long patience;

struct euclid {
  int d, x, y;
};

long long lcm(int a, int b) {
  return (long long)a * b / std::gcd(a, b);
}

euclid extended_euclid(int a, int b) {
  euclid e;
  e.x = 1;
  e.y = 0;
  int xp = 0, yp = 1;
  while (b) {
    int q = a / b;
    int tmp = b; b = a - q * b; a = tmp;
    tmp = xp; xp = e.x - q * xp; e.x = tmp;
    tmp = yp; yp = e.y - q * yp; e.y = tmp;
  }
  e.d = a;
  return e;
}

int inverse(int a, int mod) {
  euclid e = extended_euclid(a, mod);
  return (e.x >= 0) ? e.x : (e.x + mod);
}

// Find x s.t. x ≡ a (mod p.n) and x ≡ b (mod q.n). Returns NONE if
// impossible.
//
// Method: The existence condition is that gcd(p.n, q.n) divides b - a. By
// definition, x = k * p.n + a for some k. Then from the second congruence,
//
// k * p.n + a ≡ b (mod q.n) ⇒
// k * p.n = (b - a) + l * q.n for some l
//
// Divide by g = gcd(p.n, q.n) which is possible according to the existence
// condition.
//
// k * (p.n / g) = (b - a) / g + l * (q.n / g)
//
// Now p.n / g and q.n / g are coprime so we can take inverses
//
// k = (b - a) / g * [ (p.n / g)^{-1} (modulo q.n / g) ]
long long solve_crt(int a, int b) {
  int g = std::gcd(p.n, q.n);
  if ((b - a) % g) {
    return NONE;
  }

  int red_m = p.n / g;
  int red_n = q.n / g;
  int inv = inverse(red_m, red_n);
  long long k = (long long)(b - a) / g * inv;
  long long l = lcm(p.n, q.n);
  long long x = ((k + l) * p.n + a) % l; // handle negative k
  return x;
}

void clear_pos_in_q() {
  for (int val = 0; val <= MAX_VAL; val++) {
    pos_in_q[val] = NONE;
  }
}

void read_data() {
  scanf("%d %d %lld", &p.n, &q.n, &patience);
  patience--;

  for (int i = 0; i < p.n; i++) {
    scanf("%d", &p.c[i]);
  }

  for (int i = 0; i < q.n; i++) {
    scanf("%d", &q.c[i]);
    pos_in_q[q.c[i]] = i;
  }
}

int collect_overlaps() {
  int cnt = 0;
  for (int i = 0; i < p.n; i++) {
    int j = pos_in_q[p.c[i]];
    if (j != NONE) {
      long long day = solve_crt(i, j);
      if (day != NONE) {
        overlap[num_overlaps++] = day;
      }
    }
  }
  return cnt;
}

long long count_days_last_period(long long patience) {
  if (!num_overlaps) {
    return patience;
  }

  std::sort(overlap, overlap + num_overlaps);

  int i = 0;
  while ((i < num_overlaps) && (patience >= overlap[i] - i)) {
    i++;
  }
  // Here, i is the first overlap day where we don't make it.

  return patience + i;
}

long long find_kth_patient_day() {
  collect_overlaps();

  long long period = lcm(p.n, q.n);
  long long anger_per_period = period - num_overlaps;
  long long num_periods = patience / anger_per_period;
  patience %= anger_per_period;

  long long days_last_period = count_days_last_period(patience);

  return num_periods * period + days_last_period + 1;
}

void write_answer(long long answer) {
  printf("%lld\n", answer);
}

int main() {
  clear_pos_in_q();
  read_data();
  long long answer = find_kth_patient_day();
  write_answer(answer);

  return 0;
}
