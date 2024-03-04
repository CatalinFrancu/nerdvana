// Comments at the end.
#include <stdio.h>

const int MAX_N = 19;
const int MAX_K = 4;
const int MOD = 40'099;

// Every position here encodes a (l,r) combination, hence the quadratic size.
typedef int vector[MAX_K * MAX_K];
typedef int matrix[MAX_K * MAX_K][MAX_K * MAX_K];

int z, k, n;
int daily_demand[MAX_N + 1];

// Utility functions. They are boring. But that's the point. Programming
// should not be an esoteric activity. There should be no obscurantism. There
// should be no surprises. The meat of the program is hard enough without
// these blocks polluting it. If you want a thrill, go ride the roller
// coaster.

void vec_zero(vector a) {
  for (int i = 0; i < k * k; i++) {
    a[i] = 0;
  }
}

int vec_sum(vector a) {
  int sum = 0;
  for (int i = 0; i < k * k; i++) {
    sum += a[i];
  }

  return sum % MOD;
}

void vec_copy(vector src, vector dest) {
  for (int i = 0; i < k * k; i++) {
    dest[i] = src[i];
  }
}

void mat_copy(matrix src, matrix dest) {
  for (int r = 0; r < k * k; r++) {
    vec_copy(src[r], dest[r]);
  }
}

void mat_identity(matrix a) {
  for (int r = 0; r < k * k; r++) {
    vec_zero(a[r]);
    a[r][r] = 1;
  }
}

// a *= b
void mat_mult(matrix a, matrix b) {
  static matrix aux;

  for (int r = 0; r < k * k; r++) {
    for (int c = 0; c < k * k; c++) {
      long long x = 0;
      for (int i = 0; i < k * k; i++) {
        x += a[r][i] * b[i][c];
      }
      aux[r][c] = x % MOD;
    }
  }
  mat_copy(aux, a);
}

// Clobbers a.
void mat_power(matrix a, int n, matrix dest) {
  mat_identity(dest);
  while (n) {
    if (n & 1) {
      mat_mult(dest, a);
    }
    mat_mult(a, a);
    n >>= 1;
  }
}

void read_data() {
  FILE* f = fopen("marfa.in", "r");
  fscanf(f, "%d %d %d", &z, &k, &n);

  for (int i = 1; i <= n; i++) {
    fscanf(f, "%d", &daily_demand[i]);
  }
  fclose(f);
}

// Given counts of possibilities for every (l<k, r<k) state, computes the new
// counts after a day with the given demand.
void advance_day(vector a, int demand) {
  static vector aux;
  vec_zero(aux);

  for (int l = 0; l < k; l++) {
    for (int r = 0; r < k; r++) {
      int q = a[l * k + r];
      if (demand == 0) {
        // reset l and r to 0
        aux[0] += q;
      } else if (demand == 1) {
        // either increase l and reset r or viceversa
        if (l < k - 1) {
          aux[(l + 1) * k] += q;
        }
        if (r < k - 1) {
          aux[r + 1] += q;
        }
      } else {
        // increase both l and r
        if ((l < k - 1) && (r < k - 1)) {
          aux[(l + 1) * k + (r + 1)] += q;
        }
      }
    }
  }

  vec_copy(aux, a);
}

void make_transition_matrices(matrix week, matrix rem) {
  vector a;

  for (int l = 0; l < k; l++) {
    for (int r = 0; r < k; r++) {
      vec_zero(a);
      a[l * k + r] = 1;

      for (int dow = 1; dow <= n; dow++) { // day of week
        advance_day(a, daily_demand[dow]);
        if (dow == z % n) {
          vec_copy(a, rem[l * k + r]);
        }
      }

      vec_copy(a, week[l * k + r]);
    }
  }
}

int count_ways() {
  matrix week, rem, all;

  make_transition_matrices(week, rem);
  mat_power(week, z / n, all);
  if (z % n) {
    mat_mult(all, rem);
  }

  // We begin in state 0 and may end up in any state.
  return vec_sum(all[0]);
}

void write_answer(int answer) {
  FILE* f = fopen("marfa.out", "w");
  fprintf(f, "%d\n", answer);
  fclose(f);
}

int main() {
  read_data();
  int answer = count_ways();
  write_answer(answer);

  return 0;
}

// We can use dynamic programming for a solution in O(k^2 * z). What
// information do we need in order to define a state fully?
//
// 1. The current day.
// 2. The number of consecutive days the left half was used (must be < k).
// 3. The number of consecutive days the right half was used (must be < k).
//
// Therefore, let C[d][l][r] the number of ways to reach day d such that the
// left and right halves were used in the last l and r days, respectively. We
// can use forward DP to build C[d+1]. Let D be the demand on day d+1. Three
// cases arise:
//
// 1. If D = 0, then from C[d][l][r] we can only go to C[d+1][0][0] because
// necessarily we will not be using either half of the truck.
//
// 2. If D = 1, then from C[d][l][r] we can either go to C[d+1][l+1][0] or to
// C[d+1][0][r+1], depending on which half we use. Any or both choices may be
// invalid if l+1 = k or r+1 = k.
//
// 3. If D = 2, then from C[d][l][r] we can only go to C[d+1][l+1][r+1] and
// only if l < k and r < k.
//
// Now we begin with C[0][0][0] = 1 and we need the sum over all C[z][*][*].
// This works, but we need something faster. Since z is too large for anything
// linear in z, we should aim to find a transition matrix. We can use fast
// exponentiation on that matrix to reduce the running time to O(size^3 * log
// z). But the recurrence above won't suffice, because the transition matrix
// between days depends on the daily demands.
//
// We must find some invariant, and that is the transition matrix for an
// entire week. Call this matrix T. Then T[(l1,r1)][(l2,r2)] is the number of
// ways to begin the week with (l1,r1) uses of the two halves and end it with
// (l2,r2) uses. Note that T has size 16x16 when k=4.
//
// We can compute T naively. Start from every possible (l1,r1) (that is, k^2
// possibilities), then use the C matrix above. The total running time is
// O(k^4 * n).
//
// If z is divisible by n, that's all there is to it. We raise T to the power
// z/n, then we add up all the values on the first row (because we begin on
// day 0 in state (l=0,r=0) and we can and up in any state). If z % n is
// nonzero, then we need to multiply T by a matrix that's very similar to T,
// but only for the first z % n days. This matrix is actually just a partial
// result while we are computing T, so we create both at the same time.
//
// And they lived happily ever after.
