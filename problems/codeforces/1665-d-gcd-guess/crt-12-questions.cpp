// Implementation of the idea in https://codeforces.com/contest/1665/submission/236496990
//
// Uses 12 questions.
//
// The 23-question solution in the editorial shows that, if the answer to (a,
// a + q) is a multiple of q, then x + a and x + a + q are multiples of q, and
// therefore x ≡ -a (mod q). We say that a satisfies x modulo q.
//
// To simplify the code, we flip the logic. We don't care about -a (the
// remainder of x modulo q), but about a itself (the question which satisfies
// x modulo q). Once we have the a's for a convenient set of moduli, we will
// take their complements, get the actual remainders, then solve the CRT to
// find x.
//
// Working modulo 23 is inefficient. It takes 22 questions to satisfy x modulo
// 23. We can do much better: 12 questions to satisfy x modulo 49! Here is
// how. If we know the remainder modulo b^e, then we can learn the remainder
// modulo b^{e+1} using b-1 questions. For example, is 2 satisfies x modulo 7,
// then one of 2/9/16/23/30/37/44 will satisfy x modulo 49. We try at most 6
// of these choices and if none of them satisfies x, the last one will.
//
// Therefore, it takes
// * 6*2  = 12 questions to satisfy x mod 7^2;
// * 4*2  =  8 questions to satisfy x mod 5^2;
// * 2*6  = 12 questions to satisfy x mod 3^6;
// * 1*11 = 11 questions to satisfy x mod 2^11;
// , and note that 2^11 * 3^6 * 5^2 * 7^2 = 1,828,915,200.
//
// We can ask questions about several moduli in parallel by solving the
// CRT. For example, we can simultaneously test if
// * 86 satisfies x modulo 2^7;
// * 28 satisfies x modulo 3^4;
// * 9 satisfies x modulo 25;
// * 33 satisfies x modulo 49.
//
// All in all, we need max(12, 8, 12, 11) = 12 questions.
#include <stdio.h>

const int NUM_MODULI = 4;

struct congruence {
  int base;
  int exponent;
  int modulus;      // synonym for base^exponent
  int max_exponent; // final goal
  int rem;          // current candidate for satisfying x mod base^exponent

  congruence(int base, int max_exponent) {
    this->base = base;
    this->max_exponent = max_exponent;
  }

  void reset() {
    exponent = 1;
    modulus = base;
    // Test candidates in decreasing order to avoid corner cases with 0.
    rem = base - 1;
  }

  void advance(int gcd) {
    // Have we managed to satisfy x modulo base^exponent?
    bool found = false;
    if (gcd % modulus == 0) {
      found = true;
    } else if (rem < 2 * modulus / base) {
      // By process of elimination, the next candidate is satisfactory.
      rem -= modulus / base;
      found = true;
    }

    if (found) {
      // Learned that rem is satisfactory modulo modulus.
      exponent++;
      if (!is_solved()) {
        modulus *= base;
        // Start at the top again.
        rem += (base - 1) * modulus / base;
      }
    } else {
      rem -= modulus / base;
    }
  }

  bool is_solved() {
    return exponent > max_exponent;
  }

  // Prepare the congruence for the final CRT: x ≡ rem % modulus.
  void flip() {
    exponent = 0; // Mark it as unsolved.
    rem = (modulus - rem) % modulus;
  }
};

congruence c[NUM_MODULI] = {
  congruence(2, 11),
  congruence(3, 6),
  congruence(5, 2),
  congruence(7, 2),
};

unsigned ask(unsigned a, unsigned b) {
  printf("? %u %u\n", a, b);
  fflush(stdout);

  int response;
  scanf("%d", &response);
  return response;
}

void extended_euclid_iterative(int a, int b, int& d, int& x, int& y) {
  x = 1;
  y = 0;
  int xp = 0, yp = 1;
  while (b) {
    int q = a / b;
    int tmp = b; b = a - q * b; a = tmp;
    tmp = xp; xp = x - q * xp; x = tmp;
    tmp = yp; yp = y - q * yp; y = tmp;
  }
  d = a;
}

int inverse(int x, int mod) {
  int y, k, d;
  extended_euclid_iterative(x, mod, d, y, k);
  return (y >= 0) ? y : (y + mod);
}

bool finished() {
  bool all_solved = true;
  for (int i = 0; i < NUM_MODULI; i++) {
    all_solved &= c[i].is_solved();
  }
  return all_solved;
}

void solve_crt(unsigned& solution, unsigned& big_mod) {
  big_mod = 1;
  for (int i = 0; i < NUM_MODULI; i++) {
    if (!c[i].is_solved()) {
      big_mod *= c[i].modulus;
    }
  }

  solution = 0;
  for (int i = 0; i < NUM_MODULI; i++) {
    if (!c[i].is_solved()) {
      int prod_exclude = big_mod / c[i].modulus;
      int inv = inverse(prod_exclude, c[i].modulus);
      long long all = (long long)c[i].rem * prod_exclude % big_mod * inv;
      solution = (all + solution) % big_mod;
    }
  }
}

void learn_from_question() {
  unsigned common_rem, big_mod;
  solve_crt(common_rem, big_mod);
  int gcd = ask(common_rem, common_rem + big_mod);

  for (int i = 0; i < NUM_MODULI; i++) {
    if (!c[i].is_solved()) {
      c[i].advance(gcd);
    }
  }
}

void solve_test_case() {
  for (int i = 0; i < NUM_MODULI; i++) {
    c[i].reset();
  }

  while (!finished()) {
    learn_from_question();
  }

  for (int i = 0; i < NUM_MODULI; i++) {
    c[i].flip();
  }

  unsigned x, big_mod;
  solve_crt(x, big_mod);
  printf("! %u\n", x);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test_case();
  }

  return 0;
}
