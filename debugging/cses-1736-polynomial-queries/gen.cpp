#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int N = 50;
const int Q = 3;
const int MAX_VAL = 1;

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int micros = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(micros);
}

void write_array() {
  printf("%d %d\n", N, Q);
  for (int i = 0; i < N; i++) {
    printf("%d ", rand() % MAX_VAL + 1);
  }
  printf("\n");
}

void write_queries() {
  for (int i = 0; i < Q; i++) {
    int op = rand() % 2 + 1;
    int a = rand() % N + 1;
    int b = rand() % N + 1;
    if (a > b) {
      int tmp = a;
      a = b;
      b = tmp;
    }
    printf("%d %d %d\n", op, a, b);
  }
}

int main(int argc, char** argv) {
  init_rng();
  write_array();
  write_queries();

  return 0;
}
