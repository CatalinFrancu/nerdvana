// Usage: verif <in_file> <ok_file> <out_file>.
//
// Only checks the surprise value. Does not check if the files exist, if the
// solution is a permutation etc.
#include <stdio.h>
#include <stdlib.h>

const int MAX_STUDENTS = 10;
const int MAX_QUESTIONS = 10'000;

FILE *fin, *fout, *fok;
int expect[MAX_STUDENTS];
bool solved[MAX_STUDENTS][MAX_QUESTIONS];
int pout[MAX_QUESTIONS], pok[MAX_QUESTIONS];
int n, m;

void open_files(int argc, char** argv) {
  fin = fopen(argv[1], "rt");
  fok = fopen(argv[2], "rt");
  fout = fopen(argv[3], "rt");
}

void read_input() {
  fscanf(fin, "%d %d", &n, &m);
  for (int i = 0; i < n; i++) {
    fscanf(fin, "%d ", &expect[i]);
  }
  for (int stud = 0; stud < n; stud++) {
    for (int q = 0; q < m; q++) {
      solved[stud][q] = fgetc(fin) - '0';
    }
    fgetc(fin);
  }
}

void read_points(FILE* f, int* dest) {
  for (int i = 0; i < m; i++) {
    fscanf(f, "%d", &dest[i]);
  }
}

int compute_surprise(int* p) {
  int surprise = 0;

  for (int stud = 0; stud < n; stud++) {
    int sum = 0;
    for (int q = 0; q < m; q++) {
      if (solved[stud][q]) {
        sum += p[q];
      }
    }
    surprise += abs(sum - expect[stud]);
  }

  return surprise;
}

void verif_test(int test_no) {
  read_input();
  read_points(fok, pok);
  read_points(fout, pout);
  int ok_surprise = compute_surprise(pok);
  int out_surprise = compute_surprise(pout);
  if (ok_surprise != out_surprise) {
    fprintf(stderr, "ERROR: Test %d ok_surprise=%d out_surprise = %d\n",
            test_no, ok_surprise, out_surprise);
  }
}

void close_files() {
  fclose(fin);
  fclose(fok);
  fclose(fout);
}

int main(int argc, char** argv) {
  open_files(argc, argv);
  int num_tests;
  fscanf(fin, "%d", &num_tests);
  for (int t = 1; t <= num_tests; t++) {
    verif_test(t);
  }
  close_files();

  return 0;
}
