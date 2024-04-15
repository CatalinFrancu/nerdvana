#include <stdio.h>
#include <stdlib.h>

const int MAX_N = 1'000;

int n, bit;
int v[MAX_N];

void read_command_line_args(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: pretty-print <n> <bit>\n");
    exit(1);
  }

  n = atoi(argv[1]);
  bit = atoi(argv[2]);
}

void print_bit(int x) {
  putchar(((x >> bit) & 1) ? '-' : ' ');
}

void print_matrix() {
  printf("    ");
  for (int i = 0; i < n; i++) {
    v[i] = i;
    printf("%2d", i);
  }
  printf("\n");

  for (int i = 0; i < n; i++) {
    printf("%2d   ", i);
    for (int j = 0; j < n; j++) {
      print_bit(v[j]);
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
