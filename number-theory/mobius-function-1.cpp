// Taken from https://mathoverflow.net/a/99545
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
  for (int i = 1; i <= n; i++) {
    u[i] = 1;
  }

  for (int i = 2; i * i <= n; i++) {
    if (u[i] == 1) {
      for (int j = i; j <= n; j += i) {
        u[j] *= -i;
      }
      for (int j = i * i; j <= n; j += i * i) {
        u[j] = 0;
      }
    }
  }

  for (int i = 2; i <= n; i++) {
    if (u[i] == i) {
      u[i] = 1;
    } else if (u[i] == -i) {
      u[i] = -1;
    } else if (u[i] < 0) {
      u[i] = 1;
    } else if (u[i] > 0) {
      u[i] = -1;
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
