#include <stdio.h>

const int MAX_N = 500'000;
const int MAX_START = 10'000'000;
const int FAIL = -1;

int v[MAX_N];
int n, s;

void read_data(FILE* f) {
  fscanf(f, "%d %d", &n, &s);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &v[i]);
  }
}

int get(int pos) {
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

bool fits(int start) {
  for (int i = 0; i < n; i++) {
    if (get(start + i) != v[i]) {
      return false;
    }
  }
  return true;
}

int find_start_position() {
  for (int start = 0; start < MAX_START; start++) {
    if (fits(start)) {
      return start;
    }
  }

  return FAIL;
}

void process_test(FILE* fin, FILE* fout) {
  read_data(fin);
  int answer = find_start_position();
  fprintf(fout, "%d\n", answer);
}

int main() {
  FILE* fin = fopen("pscfft.in", "r");
  FILE* fout = fopen("pscfft.out", "w");
  int num_tests;
  fscanf(fin, "%d", &num_tests);
  while (num_tests--) {
    process_test(fin, fout);
  }
  fclose(fin);
  fclose(fout);

  return 0;
}
