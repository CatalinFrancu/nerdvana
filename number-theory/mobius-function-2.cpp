// Adapted from https://www.geeksforgeeks.org/program-for-mobius-function-set-2/
#include <stdio.h>
#include <stdlib.h>

const int MAX_N = 10'000'000;

int factor[MAX_N + 1], u[MAX_N + 1];

int parse_command_line(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <n>\n", argv[0]);
    exit(1);
  }

  return atoi(argv[1]);
}

void compute_smallest_factors(int n) {
  factor[1] = 1;
  for (int i = 2; i <= n; i++) {
    if (!factor[i])  {
      for (int j = i; j <= n; j += i) {
        if (!factor[j]) {
          factor[j] = i;
        }
      }
    }
  }
}

void compute_mobius(int n) {
  u[1] = 1;
  for (int i = 2; i <= n; i++) {
    if (factor[i] == factor[i / factor[i]]) {
      u[i] = 0;
    } else {
      u[i] = -u[i / factor[i]];
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

  compute_smallest_factors(n);
  compute_mobius(n);
  // print_mobius(n);

  return 0;
}
