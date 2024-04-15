#include <stdio.h>
#include <stdlib.h>

const int MAX_N = 1'000;

int n, bits;
int v[MAX_N];

void read_command_line_args(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: pretty-print <n> <bits>\n");
    exit(1);
  }

  n = atoi(argv[1]);
  bits = atoi(argv[2]);
}

void print_binary(int x) {
  for (int b= bits - 1; b >= 0; b--) {
    printf("%d", (x >> b) & 1);
  }
}

void print_matrix() {
  printf("     ");
  for (int i = 0; i < n; i++) {
    v[i] = i;
    printf("%*d ", bits, i);
  }
  printf("\n");

  for (int i = 0; i < n; i++) {
    printf("%2d   ", i);
    for (int j = 0; j < n; j++) {
      print_binary(v[j]);
      printf(" ");
      if (j) {
        v[j] = v[j - 1] ^ v[j];
      }
    }
    printf("\n");
  }
}

int main(int argv, char** argc) {
  read_command_line_args(argv, argc);
  print_matrix();

  return 0;
}
