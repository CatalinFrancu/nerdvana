#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int MAX_N = 100'000;
const int NIL = -1;

int v[MAX_N], p[MAX_N];
int n, chain_length, dense_node, q, max_color, max_f;

void read_command_line_args(int argc, char** argv) {
  if (argc != 7) {
    fprintf(stderr, "Usage: gen <n> <chain_length> <dense_node> <q> <max_color> <max_f>\n");
    exit(1);
  }

  n = atoi(argv[1]);
  chain_length = atoi(argv[2]);
  dense_node = atoi(argv[3]);
  q = atoi(argv[4]);
  max_color = atoi(argv[5]);
  max_f = atoi(argv[6]);

  assert(chain_length + dense_node <= n);
}

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int seed = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(seed);
}

int rand2(int min, int max) {
  return min + rand() % (max - min + 1);
}

void shuffle(int* v, int n) {
  for (int i = 0; i < n; i++) {
    int j = rand() % (i + 1);
    int tmp = v[i];
    v[i] = v[j];
    v[j] = tmp;
  }
}

void generate_colors() {
  printf("%d %d\n", n, q);
  for (int u = 1; u <= n; u++) {
    printf("%d ", rand2(1, max_color));
  }
}

void generate_tree() {
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

void generate_queries() {
  printf("\n");
  for (int i = 0; i < q; i++) {
    printf("%d %d\n", rand2(1, n), rand2(1, max_f));
  }
}

int main(int argc, char** argv) {
  read_command_line_args(argc, argv);
  init_rng();

  generate_colors();
  generate_tree();
  generate_queries();

  return 0;
}
