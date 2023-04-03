#include <stdio.h>

#define MAX_N 561

int mod_pow(int b, int e, int mod) {
  long long result = 1;

  while (e) {
    if (e & 1) {
      result = result * b % mod;
    }
    b = (long long)b * b % mod;
    e >>= 1;
  }

  return result;
}

int main() {
  for (int n = 3; n <= MAX_N; n++) {
    int cnt = 0;
    for (int a = 2; a < n; a++) {
      int r = mod_pow(a, n, n);
      cnt += (r == a);
    }
    printf("n = %d: %d of %d pass the test\n", n, cnt, n - 2);
  }
  return 0;
}
