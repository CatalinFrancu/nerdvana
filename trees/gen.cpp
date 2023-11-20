// Tree generator. The tree is guaranteed to have a chain of length at least
// chain_length and a node with at least dense_node neighbors.
// Can also generate various types of node values, queries and updates.
// NOTE: Nodes are numbered from 0.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_N 1'000'000
#define NIL -1

int v[MAX_N], p[MAX_N];

void usage() {
  fprintf(stderr, "Usage: gen <n> <chain_length> <dense_node> <op_type> [...]\n");
  fprintf(stderr, "  op_type = 0:\n");
  fprintf(stderr, "    no values or updates, just the tree\n");
  fprintf(stderr, "  op_type = 1 <num_ops> <max_value>:\n");
  fprintf(stderr, "    initial values, delta updates per node, queries per node\n");
  fprintf(stderr, "  op_type = 2 <num_ops>:\n");
  fprintf(stderr, "    no values, no updates, queries are pairs of nodes\n");
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
      printf("%d %d\n", 1 + x, 1 + y);
    }
  }
}

void generate_ops(int n, int num_ops, int max_value) {
  // Generate and print the initial values.
  for (int i = 0; i < n; i++) {
    v[i] = rand() % (max_value + 1);
    printf("%d ", v[i]);
  }
  printf("\n");

  printf("%d\n", num_ops);
  while (num_ops--) {
    int u = rand() % n;
    if (rand() % 2) {
      // update -- make sure we stay within max_value
      int delta = rand() % (max_value + 1) - v[u];
      v[u] += delta;
      printf("1 %d %d\n", 1 + u, delta);
    } else {
      // query
      printf("2 %d\n", 1 + u);
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
  if (argc < 5) {
    usage();
  }

  int n = atoi(argv[1]);
  int chain_length = atoi(argv[2]);
  int dense_node = atoi(argv[3]);
  int op_type = atoi(argv[4]);

  assert(chain_length + dense_node <= n);

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  generate_tree(n, chain_length, dense_node);

  int num_ops, max_value;
  switch (op_type) {
    case 0:
      // Nothing left to do.
      break;

    case 1:
      if (argc != 7) {
        usage();
      }
      num_ops = atoi(argv[5]);
      max_value = atoi(argv[6]);
      generate_ops(n, num_ops, max_value);
      break;

    case 2:
      if (argc != 6) {
        usage();
      }
      num_ops = atoi(argv[5]);
      generate_lca_queries(n, num_ops);
      break;

    default:
      assert(false);
  }

  return 0;
}
