#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int n, num_colors, max_cost;

void read_command_line_args(int argc, char** argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: gen <n> <num_colors> <max_cost>\n");
    exit(1);
  }

  n = atoi(argv[1]);
  num_colors = atoi(argv[2]);
  max_cost = atoi(argv[3]);
}

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int seed = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(seed);
}

int rand2(int a, int b) {
  return a + rand() % (b - a + 1);
}

void write_data() {
  printf("%d %d\n", n, num_colors);

  for (int i = 0; i < n; i++) {
    printf("%d ", rand2(1, num_colors));
  }
  printf("\n");

  for (int i = 0; i < num_colors; i++) {
    printf("%d ", rand2(1, max_cost));
  }
  printf("\n");
}

int main(int argc, char** argv) {
  read_command_line_args(argc, argv);
  init_rng();
  write_data();

  return 0;
}
