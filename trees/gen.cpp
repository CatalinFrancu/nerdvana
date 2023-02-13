// Tree generator. The tree is guaranteed to have a chain of length at least
// chain_length and a node with at least dense_node neighbors.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_N 1'000'000
#define NIL -1

int v[MAX_N], p[MAX_N];

void usage() {
  fprintf(stderr, "Usage: gen <n> <chain_length> <dense_node>\n");
  _exit(1);
}

void shuffle(int* v, int n) {
  for (int i = 0; i < n; i++) {
    int j = rand() % (i + 1);
    int tmp = v[i];
    v[i] = v[j];
    v[j] = tmp;
  }
}

int main(int argc, char** argv) {
  if (argc != 4) {
    usage();
  }

  int n = atoi(argv[1]);
  int chain_length = atoi(argv[2]);
  int dense_node = atoi(argv[3]);

  assert(chain_length + dense_node <= n);

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  for (int i = 0; i < n; i++) {
    v[i] = i;
    p[i] = NIL;
  }

  shuffle(v, n);

  // Make a chain with nodes [0, chain_length).
  for (int i = 1; i < chain_length; i++) {
    p[v[i]] = v[i - 1];
    // printf("Chain: %d %d\n", v[i], p[v[i]]);
  }

  // Connect nodes [chain_length, n - dense_node) to random nodes before them.
  for (int i = chain_length; i < n - dense_node; i++) {
    p[v[i]] = v[rand() % i];
    // printf("Other: %d %d\n", v[i], p[v[i]]);
  }

  // Connect nodes from [n - dense_node, n) to the same parent.
  int parent = rand() % (n - dense_node);
  for (int i = n - dense_node; i < n; i++) {
    p[v[i]] = v[parent];
    // printf("Star: %d %d\n", v[i], p[v[i]]);
  }

  // Don't print the edges in this particular order.
  shuffle(v, n);

  printf("%d\n", n);
  for (int i = 0; i < n; i++) {
    int x = v[i], y = p[v[i]];
    if (y != NIL) {
      // Sometimes flip the parent and child.
      if (rand() % 2) {
        int tmp = x;
        x = y;
        y = tmp;
      }
      printf("%d %d\n", x + 1, y + 1);
    }
  }

  return 0;
}
