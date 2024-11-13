#include <stdio.h>
#include <stdlib.h>

const int MAX_N = 1'000'000;
const int NONE = -1;

int a[MAX_N], n;

void readInputData(char* filename) {
  FILE* f = fopen(filename, "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &a[i]);
  }
  fclose(f);
}

bool compatible(int x, int y) {
  return (x & y) == 0;
}

void verifyNoCompatible(int pos) {
  for (int i = 0; i < n; i++) {
    if (compatible(a[i], a[pos])) {
      printf("ERROR: a[%d]=%d should have reported compatible element a[%d]=%d.\n",
             pos, a[pos], i, a[i]);
      exit(1);
    }
  }
}

bool inArray(int val) {
  int j = 0;
  while ((j < n) && (a[j] != val)) {
    j++;
  }

  return (j < n);
}

void verifCompatible(int pos, int val) {
  if (!compatible(a[pos], val)) {
    printf("ERROR: a[%d]=%d reports incompatible element %d.\n",
           pos, a[pos], val);
    exit(1);
  }

  if (!inArray(val)) {
    printf("ERROR: a[%d]=%d reports nonexistent element %d.\n",
           pos, a[pos], val);
    exit(1);
  }
}

void verifyOutputData(char* filename) {
  FILE* f = fopen(filename, "r");

  for (int i = 0; i < n; i++) {
    int x;
    if (fscanf(f, "%d", &x) != 1) {
      printf("ERROR: Premature EOF at element %d.\n", i);
      exit(1);
    }
    if (x == NONE) {
      verifyNoCompatible(i);
    } else {
      verifCompatible(i, x);
    }
  }
  fclose(f);
}

int main(int argc, char** argv) {
  readInputData(argv[1]);
  verifyOutputData(argv[2]);

  return 0;
}
