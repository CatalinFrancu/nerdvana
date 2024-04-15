#include <stdio.h>

int we_meet_again_mr_sierpinski(int r, int c) {
  return (r + c) == (r ^ c);
}

int get_value(int r, int c) {
  int sum = 0;

  for (int b = 1; b <= c; b *= 2) {
    sum += b * we_meet_again_mr_sierpinski(r + b, c - b);
  }

  return sum;
}

int main() {
  FILE* fin = fopen("xor.in", "r");
  FILE* fout = fopen("xor.out", "w");

  int q, i, j, a, b, m;

  fscanf(fin, "%d %d %d %d %d %d", &q, &i, &j, &a, &b, &m);
  while (q--) {
    fprintf(fout, "%d\n", get_value(i, j));
    i = (a * i + b) % m;
    j = (a * j + b) % m;
  }

  fclose(fin);
  fclose(fout);

  return 0;
}
