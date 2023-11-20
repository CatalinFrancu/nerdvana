#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_N 1'000'000
#define NIL -1

int v[MAX_N], p[MAX_N];

void usage() {
  fprintf(stderr, "Usage: gen <n> <chain_length> <dense_node> <num_ops> <max_value>\n");
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

void generate_values(int n, int max_value) {
  for (int i = 0; i < n; i++) {
    printf("%d ", 1 + rand() % max_value);
  }
  printf("\n");
}

void generate_tree(int n, int chain_length, int dense_node) {
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

  for (int i = 0; i < n; i++) {
    int x = v[i], y = p[v[i]];
    if (y != NIL) {
      // Sometimes flip the parent and child.
      if (rand() % 2) {
        int tmp = x;
        x = y;
        y = tmp;
      }
      printf("%d %d\n", 1 + x, 1 + y);
    }
  }
}

void generate_ops(int n, int num_ops, int max_value) {
  while (num_ops--) {
    if (rand() % 2) {
      // query
      printf("1 %d %d\n", 1 + rand() % n, 1 + rand() % n);
    } else {
      // update
      printf("0 %d %d\n", 1 + rand() % n, 1 + rand() % max_value);
    }
  }
}

void generate_lca_queries(int n, int num_ops) {
  // Generate pairs of distinct nodes
  printf("%d\n", num_ops);
  while (num_ops--) {
    int u = rand() % n, v;
    do {
      v = rand() % n;
    } while (u == v);
    printf("%d %d\n", 1 + u, 1 + v);
  }
}

int main(int argc, char** argv) {
  if (argc != 6) {
    usage();
  }

  int n = atoi(argv[1]);
  int chain_length = atoi(argv[2]);
  int dense_node = atoi(argv[3]);
  int num_ops = atoi(argv[4]);
  int max_value = atoi(argv[5]);

  assert(chain_length + dense_node <= n);

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  printf("%d %d\n", n, num_ops);
  generate_values(n, max_value);
  generate_tree(n, chain_length, dense_node);
  generate_ops(n, num_ops, max_value);

  return 0;
}
