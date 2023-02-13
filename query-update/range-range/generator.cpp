#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define T_UPDATE 1
#define T_SUM 2

void usage() {
  fprintf(stderr, "Usage: generator <n> <m> <q1> <q2> <delta> <p1> <p2>\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Generates an array of n random integers in [0, m).\n");
  fprintf(stderr, "Then generates q1 updates and q2 queries as follows:\n");
  fprintf(stderr, "* p1%% of the queries will be smaller than sqrt(n)\n");
  fprintf(stderr, "* p2%% of the queries will be between sqrt(n) and n/2\n");
  fprintf(stderr, "* the remaining queries will be larger than n/2\n");
  fprintf(stderr, "Updates will be between -delta and +delta.\n");
  fprintf(stderr, "All queries are [closed,closed] and all indices are 1-based.\n");
  _exit(1);
}

int rand2(int x, int y) {
  return rand() % (y - x + 1) + x;
}

int main(int argc, char** argv) {
  if (argc != 8) {
    usage();
  }

  int n = atoi(argv[1]);
  int m = atoi(argv[2]);
  int q[3] = { 0, atoi(argv[3]), atoi(argv[4]) };
  int delta = atoi(argv[5]);
  int p1 = atoi(argv[6]);
  int p2 = atoi(argv[7]);

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  printf("%d %d\n", n, q[1] + q[2]);
  for (int i = 0; i < n; i++) {
    printf("%d ", rand() % m);
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
    assert((0 <= l) && (l <= r) && (r <= n));

    if (t == T_UPDATE) {
      int val = rand2(-delta, delta);
      printf("%d %d %d %d\n", t, l, r, val);
    }  else {
      printf("%d %d %d\n", t, l, r);
    }

  }

  return 0;
}
