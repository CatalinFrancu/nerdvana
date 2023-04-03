#include <stdio.h>
#include <stdlib.h>

void usage() {
  fprintf(stderr, "Usage: extended-euclid <a> <b>\n");
  exit(1);
}

void extended_euclid_recursive(int a, int b, int& d, int& x, int& y) {
  if (!b) {
    d = a;
    x = 1;
    y = 0;
  } else {
    int xp, yp;
    extended_euclid_recursive(b, a % b, d, xp, yp);
    x = yp;
    y = xp - (a / b) * yp;
  }
}

void extended_euclid_iterative(int a, int b, int& d, int& x, int& y) {
  x = 1;
  y = 0;
  int xp = 0, yp = 1;
  while (b) {
    int q = a / b;
    int tmp = b; b = a - q * b; a = tmp;
    tmp = xp; xp = x - q * xp; x = tmp;
    tmp = yp; yp = y - q * yp; y = tmp;
  }
  d = a;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    usage();
  }

  int a = atoi(argv[1]);
  int b = atoi(argv[2]);

  int d, x, y;
  extended_euclid_recursive(a, b, d, x, y);
  printf("Recursive: %d = %d x %d + %d x %d\n", d, x, a, y, b);
  extended_euclid_iterative(a, b, d, x, y);
  printf("Iterative: %d = %d x %d + %d x %d\n", d, x, a, y, b);

  return 0;
}
