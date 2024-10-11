// As featured in https://kilonova.ro/submissions/84735
#include <stdio.h>
#include <stdlib.h>

const int MAX_N = 10'000'000;

int u[MAX_N + 1];

int parse_command_line(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <n>\n", argv[0]);
    exit(1);
  }

  return atoi(argv[1]);
}

void compute_mobius(int n) {
  u[1] = 1;
  for (int i = 1; i <= n; i++) {
    if (u[i]) { // optional speed optimization
      for (int j = 2 * i; j <= n; j += i) {
        u[j] -= u[i];
      }
    }
  }
}

void print_mobius(int n) {
  for (int i = 1; i <= n; i++) {
    printf("%3d:%2d ", i, u[i]);
    if (i % 10 == 0) {
      printf("\n");
    }
  }
}

int main(int argc, char** argv) {
  int n = parse_command_line(argc, argv);

  compute_mobius(n);
  // print_mobius(n);

  return 0;
}
