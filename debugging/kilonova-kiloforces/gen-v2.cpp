#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int N = 5;
const int Q = 1;
const int MAX_P = 10;
const int MAX_T = 10;

bool used[MAX_T + 1];

int main() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int seed = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(seed);

  printf("%d %d\n", N, Q);

  for (int i = 0; i < N; i++) {
    printf("%d ", rand() % MAX_P + 1);
  }
  printf("\n");

  for (int i = 0; i < N; i++) {
    int t;
    do {
      t = rand() % MAX_T + 1;
    } while (used[t]);
    printf("%d ", t);
    used[t] = true;
  }
  printf("\n");

  for (int i = 0; i < Q; i++) {
    int l = rand() % N + 1;
    int r = rand() % N + 1;
    if (l > r) {
      int tmp = l;
      l = r;
      r = tmp;
    }
    printf("%d %d\n", l, r);
  }

  return 0;
}
