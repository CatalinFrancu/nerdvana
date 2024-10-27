#include <stdio.h>
#include <stdlib.h>

const int MAX_N = 100;

int v[MAX_N], n;
int answer;

void read_input(char* filename) {
  FILE* fin = fopen(filename, "r");
  fscanf(fin, "%d", &n);
  for (int i = 0; i < n; i++) {
    fscanf(fin, "%d", &v[i]);
  }
  fclose(fin);
}

void read_output(char* filename) {
  FILE* fout = fopen(filename, "r");
  fscanf(fout, "%d", &answer);
  fclose(fout);
}

void verify_output() {
  if (answer == -1) {
    printf("WARNING: Answer is -1\n");
    return;
  }

  int pc = __builtin_popcount(answer ^ v[0]);
  for (int i = 1; i < n; i++) {
    if (__builtin_popcount(answer ^ v[i]) != pc) {
      printf("ERROR! Wrong answer\n");
      exit(1);
    }
  }
}

int main(int argc, char** argv) {
  read_input(argv[1]);
  read_output(argv[2]);
  verify_output();

  return 0;
}
