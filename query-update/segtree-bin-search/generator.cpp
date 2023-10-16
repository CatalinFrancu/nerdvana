#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int MAX_N = 500'000;
const int OP_UPDATE = 1;
const int OP_QUERY = 2;

int v[MAX_N + 1];
int size, maxValue, numUpdates, numQueries;
int total;

void usage() {
  fprintf(stderr, "Usage: generator <size> <maxValue> <numUpdates> <numQueries>\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Generates an array of n random integers in [0, maxValue).\n");
  fprintf(stderr, "Then generates numUpdates updates and numQueries queries.\n");
  fprintf(stderr, "All partial sum queries are between 0 and the sum of all elements.\n");
  exit(1);
}

void parseCommandLineArgs(int argc, char** argv) {
  if (argc != 5) {
    usage();
  }

  size = atoi(argv[1]);
  maxValue = atoi(argv[2]);
  numUpdates = atoi(argv[3]);
  numQueries = atoi(argv[4]);
}

void initRng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

int rand2(int x, int y) {
  return rand() % (y - x + 1) + x;
}

void emitArray() {
  printf("%d %d\n", size, numUpdates + numQueries);

  total = 0;
  for (int i = 0; i < size; i++) {
    v[i] = rand() % maxValue;
    total += v[i];
    printf("%d ", v[i]);
  }
  printf("\n");
}

int pickOp() {
  int numOps = numUpdates + numQueries;
  return (rand() % numOps < numUpdates)
    ? OP_UPDATE
    : OP_QUERY;
}

void emitUpdate() {
  int pos = rand2(1, size);
  int val = rand() % maxValue;
  total = total - v[pos] + val;
  v[pos] = val;
  printf("%d %d %d\n", OP_UPDATE, pos, val);
  numUpdates--;
}

void emitQuery() {
  int psum = rand2(0, total + 1);
  printf("%d %d\n", OP_QUERY, psum);
  numQueries--;
}

void emitOps() {
  while (numUpdates + numQueries)  {
    int op = pickOp();

    if (op == OP_UPDATE) {
      emitUpdate();
    } else {
      emitQuery();
    }
  }
}

int main(int argc, char** argv) {
  parseCommandLineArgs(argc, argv);
  initRng();

  emitArray();
  emitOps();

  return 0;
}
