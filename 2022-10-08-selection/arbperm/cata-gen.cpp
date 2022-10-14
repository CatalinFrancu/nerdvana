// Permutation generator for Arbperm
// Author: Cătălin Frâncu
//
// WARNING! This generator doesn't ensure that there exist K permutations to
// the right of P. Some of my sources assert that.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define MIN_N 3
#define MAX_N 100'000
#define MIN_K 1
#define MAX_K 1'000'000'000

int v[MAX_N];

void usage() {
  fprintf(stderr, "Usage: cata-gen <n> <k>\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Generates a random permutation of n elements.\n");
  _exit(1);
}

int main(int argc, char** argv) {
  if (argc != 3) {
    usage();
  }

  int n = atoi(argv[1]);
  int k = atoi(argv[2]);

  // Sanity checks.
  assert((MIN_N <= n) && (n <= MAX_N));
  assert((MIN_K <= k) && (k <= MAX_K));

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  for (int i = 0; i < n; i++) {
    int k = rand() % (i + 1);
    v[i] = v[k];
    v[k] = i + 1;
  }

  printf("%d %d\n", n, k);
  for (int i = 0; i < n; i++) {
    printf("%d ", v[i]);
  }
  printf("\n");

  return 0;
}
