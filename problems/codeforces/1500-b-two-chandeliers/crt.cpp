// Instead of the given anger, we work with a patience value which is one less
// than the anger value. We rephrase the question as "How many days will this
// patience last Vasya?". The original answer is the rephrased answer +1.
//
// The chandeliers have a period of P=[n, m] days. During these P days there
// will be some overlaps, but the rest of the days will consume
// patience. Therefore, we can compute the whole number of periods, then work
// on the last, incomplete period.
//
// For the last period, we collect the days when chandeliers match using the
// Chinese Remainder Theorem. This is nontrivial because the moduli are not
// necessarily coprime. See the last section of this page:
//
// https://sites.millersville.edu/bikenaga/number-theory/chinese-remainder-theorem/chinese-remainder-theorem.html
//
// After this, sort the overlapping days and run a simple linear search to
// find how long the remaining patience will last.
#include <algorithm>
#include <numeric>
#include <stdio.h>

const int MAX_N = 500'000;
const int MAX_VAL = 2 * MAX_N;
const int NONE = -1;

int a[MAX_N], b[MAX_N];
int pos_in_b[MAX_VAL + 1];
long long overlap[MAX_N];
int n, m, num_overlaps;
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

// Find x s.t. x ≡ a (mod n) and x ≡ b (mod m). Returns NONE if impossible.
long long solve_crt(int a, int b) {
  int g = std::gcd(n, m);
  long long l = lcm(n, m);
  if ((a - b) % g) {
    return NONE;
  }

  int red_n = n / g;
  int red_m = m / g;
  int p = inverse(red_n, red_m);
  int k = (a - b) / g;
  long long result = (a - (long long)p * k * n) % l;
  if (result < 0) {
    result = l - (-result) % l;
  }
  return result;
}

void clear_pos_in_b() {
  for (int val = 0; val <= MAX_VAL; val++) {
    pos_in_b[val] = NONE;
  }
}

void read_data() {
  scanf("%d %d %lld", &n, &m, &patience);
  patience--;

  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
  }

  for (int i = 0; i < m; i++) {
    scanf("%d", &b[i]);
    pos_in_b[b[i]] = i;
  }
}

int collect_overlaps() {
  int cnt = 0;
  for (int i = 0; i < n; i++) {
    int j = pos_in_b[a[i]];
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

  long long period = lcm(n, m);
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
  clear_pos_in_b();
  read_data();
  long long answer = find_kth_patient_day();
  write_answer(answer);

  return 0;
}
