#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int OP_UPDATE = 1;
const int OP_QUERY = 2;

typedef struct {
  int left, right;
} range;

int size, maxValue, numUpdates, numQueries, delta, p1, p2;
int root, half;

void usage() {
  fprintf(stderr, "Usage: generator <size> <maxValue> <numUpdates> <numQueries> <delta> <p1> <p2>\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Generates an array of n random integers in [0, maxValue).\n");
  fprintf(stderr, "Then generates numUpdates updates and numQueries queries as follows:\n");
  fprintf(stderr, "* p1%% of the queries will be smaller than sqrt(n)\n");
  fprintf(stderr, "* p2%% of the queries will be between sqrt(n) and n/2\n");
  fprintf(stderr, "* the remaining queries will be larger than n/2\n");
  fprintf(stderr, "Updates will be between -delta and +delta.\n");
  fprintf(stderr, "All queries are [closed,closed] and all indices are 1-based.\n");
  exit(1);
}

void parseCommandLineArgs(int argc, char** argv) {
  if (argc != 8) {
    usage();
  }

  size = atoi(argv[1]);
  maxValue = atoi(argv[2]);
  numUpdates = atoi(argv[3]);
  numQueries = atoi(argv[4]);
  delta = atoi(argv[5]);
  p1 = atoi(argv[6]);
  p2 = atoi(argv[7]);
}

void computeOtherValues() {
  root = sqrt(size);
  half = size / 2;
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
  for (int i = 0; i < size; i++) {
    printf("%d ", rand() % maxValue);
  }
  printf("\n");
}

int pickOp() {
  int numOps = numUpdates + numQueries;
  return (rand() % numOps < numUpdates)
    ? OP_UPDATE
    : OP_QUERY;
}

int pickQueryLen() {
  int p = rand() % 100;
  if (p < p1) {
    return rand2(1, root - 1);
  } else if (p < p1 + p2) {
    return rand2(root, half - 1);
  } else {
    return rand2(half, size);
  }
}

range pickRange() {
  range r;
  int len = pickQueryLen();
  r.left = rand2(1, size - len + 1);
  r.right = r.left + len - 1;
  assert((1 <= r.left) && (r.left <= r.right) && (r.right <= size));
  return r;
}

void emitUpdate(range r) {
  int val = rand2(-delta, delta);
  printf("%d %d %d %d\n", OP_UPDATE, r.left, r.right, val);
  numUpdates--;
}

void emitQuery(range r) {
  printf("%d %d %d\n", OP_QUERY, r.left, r.right);
  numQueries--;
}

void emitOps() {
  while (numUpdates + numQueries)  {
    int op = pickOp();
    range r = pickRange();

    if (op == OP_UPDATE) {
      emitUpdate(r);
    } else {
      emitQuery(r);
    }
  }
}

int main(int argc, char** argv) {
  parseCommandLineArgs(argc, argv);
  computeOtherValues();
  initRng();

  emitArray();
  emitOps();

  return 0;
}
