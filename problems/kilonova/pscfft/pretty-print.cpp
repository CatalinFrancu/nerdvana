#include <stdio.h>
#include <stdlib.h>

const int MAX_LENGTH = 100'000'000;

int s, num_levels, len;
unsigned char buf[MAX_LENGTH];

void read_command_line_args(int argc, char** argv) {
  s = atoi(argv[1]);
  num_levels = atoi(argv[2]);
}

void create_string() {
  len = 1;
  buf[0] = 0;
  for (int i = 0; i < num_levels; i++) {
    for (int step = 1; step < s; step++) {
      for (int j = 0; j < len; j++) {
        buf[step * len + j] = (buf[j] + step) % s;
      }
    }
    len *= s;
  }
}

void write_string() {
  for (int i = 0; i < len; i++) {
    putchar(buf[i] + '0');
    if ((i + 1) % (s * s * s * s * s) == 0) {
      printf("\n\n\n");
    } else if ((i + 1) % (s * s * s * s) == 0) {
      printf("\n\n");
    } else if ((i + 1) % (s * s * s) == 0) {
      printf("\n");
    } else if ((i + 1) % (s * s) == 0) {
      printf("    ");
    } else if ((i + 1) % s == 0) {
      printf(" ");
    }
  }
  putchar('\n');
}

int main(int argv, char** argc) {
  read_command_line_args(argv, argc);
  create_string();
  write_string();

  return 0;
}
