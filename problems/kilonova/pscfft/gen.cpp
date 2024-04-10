#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int max_s, max_n, max_start;

int min(int x, int y) {
  return (x < y) ? x : y;
}

void read_command_line_args(int argc, char** argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: gen <max_s> <max_n> <max_start>\n");
    exit(1);
  }

  max_s = atoi(argv[1]);
  max_n = atoi(argv[2]);
  max_start = atoi(argv[3]);
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

int get(int pos, int s) {
  if (s == 1) {
    return 0;
  }

  int result = 0;
  while (pos) {
    result += pos;
    pos /= s;
  }
  return result % s;
}

void write_string() {
  int s = rand2(1, max_s);
  int n = rand2(1, max_n);
  int start = rand2(1, max_start);

  printf("1\n");
  printf("%d %d\n", n, s);
  for (int i = start; i < start + n; i++) {
    printf("%d ", get(i, s));
  }
  printf("\n");
}

int main(int argv, char** argc) {
  read_command_line_args(argv, argc);
  init_rng();
  write_string();

  return 0;
}
