// Test generator for Shperl
// Author: Cătălin Frâncu
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define MIN_N 3
#define MAX_N 300'000
#define MIN_Q 1
#define MAX_Q 300'000

void usage() {
  fprintf(stderr, "Usage: cata-gen <n> <q1> <q2> <p1> <p2>\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Generates a random array of n bits. Then generates q1 toggles and q2 queries as follows:\n");
  fprintf(stderr, "* p1%% of the queries will be smaller than sqrt(n)\n");
  fprintf(stderr, "* p2%% of the queries will be between sqrt(n) and n/2\n");
  fprintf(stderr, "* the remaining queries will be larger than n/2\n");
  _exit(1);
}

int rand2(int x, int y) {
  return rand() % (y - x + 1) + x;
}

int main(int argc, char** argv) {
  if (argc != 6) {
    usage();
  }

  int n = atoi(argv[1]);
  int q[3] = { 0, atoi(argv[2]), atoi(argv[3]) };
  int p1 = atoi(argv[4]);
  int p2 = atoi(argv[5]);

  // sanity checks
  assert((MIN_N <= n) && (n <= MAX_N));
  assert((MIN_Q <= q[1] + q[2]) && (q[1] + q[2] <= MAX_Q));

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  printf("%d %d\n", n, q[1] + q[2]);
  for (int i = 0; i < n; i++) {
    printf("%d", rand() % 2);
  }
  printf("\n");

  int sq = sqrt(n), half = n / 2;

  while (q[1] + q[2])  {
    int t = (rand() % (q[1] + q[2]) < q[1]) ? 1 : 2;
    q[t]--;

    int p = rand() % 100, size;
    if (p < p1) {
      size = rand2(1, sq - 1);
    } else if (p < p1 + p2) {
      size = rand2(sq, half - 1);
    } else {
      size = rand2(half, n);
    }

    int l = rand2(1, n - size + 1), r = l + size - 1;
    printf("%d %d %d\n", t, l, r);

    // more sanity checks
    assert((1 <= l) && (l <= r) && (r <= n));
  }

  return 0;
}
