#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int SIGMA = 4;
const char ALPHABET[5] = "ACGT";
const int OP_UPDATE = 1;
const int OP_QUERY = 2;

int n, num_updates, num_queries, min_qlen, max_qlen;

void read_command_line_args(int argc, char** argv) {
  if (argc != 6) {
    fprintf(stderr, "Usage: gen <n> <num_updates> <num_queries> <min_qlen> <max_qlen>\n");
    exit(1);
  }

  n = atoi(argv[1]);
  num_updates = atoi(argv[2]);
  num_queries = atoi(argv[3]);
  min_qlen = atoi(argv[4]);
  max_qlen = atoi(argv[5]);
}

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int seed = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(seed);
}

int rand2(int a, int b) {
  return a + rand() % (b - a + 1);
}

char random_char() {
  return ALPHABET[rand() % SIGMA];
}

void write_update() {
  printf("%d %d %c\n", OP_UPDATE, rand2(1, n), random_char());
}

void write_query() {
  int l = rand2(1, n);
  int r = rand2(1, n);
  if (l > r) {
    int tmp = l;
    l = r;
    r = tmp;
  }
  printf("%d %d %d ", OP_QUERY, l, r);

  int qlen = rand2(min_qlen, max_qlen);
  while (qlen--) {
    putchar(random_char());
  }
  printf("\n");
}

void write_data() {
  for (int i = 0; i < n; i++) {
    putchar(random_char());
  }
  putchar('\n');
  printf("%d\n", num_updates + num_queries);

  while (num_updates || num_queries) {
    if (rand() % (num_updates + num_queries) < num_updates) {
      write_update();
      num_updates--;
    } else {
      write_query();
      num_queries--;
    }
  }
}

int main(int argc, char** argv) {
  read_command_line_args(argc, argv);
  init_rng();
  write_data();

  return 0;
}
