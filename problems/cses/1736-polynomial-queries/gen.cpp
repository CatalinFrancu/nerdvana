#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int n, q, max_val;

void read_command_line_args(int argc, char** argv) {
  if (argc < 4) {
    fprintf(stderr, "Usage: gen <n> <q> <max_val>\n");
    exit(1);
  }

  n = atoi(argv[1]);
  q = atoi(argv[2]);
  max_val = atoi(argv[3]);
}

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int micros = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(micros);
}

int rand2(int x, int y) {
  return rand() % (y - x + 1) + x;
}

void write_array() {
  printf("%d %d\n", n, q);
  for (int i = 0; i < n; i++) {
    printf("%d ", rand2(1, max_val));
  }
  printf("\n");
}

void write_queries() {
  while (q--) {
    int a = rand2(1, n), b = rand2(1, n);
    if (a > b) {
      int tmp = a;
      a = b;
      b = tmp;
    }
    printf("%d %d %d\n", rand2(1, 2), a, b);
  }
}

int main(int argc, char** argv) {
  read_command_line_args(argc, argv);
  init_rng();

  write_array();
  write_queries();

  return 0;
}
