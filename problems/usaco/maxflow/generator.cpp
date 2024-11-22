// TODO shuffle the nodes
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int n, k;

void usage() {
  fprintf(stderr, "Usage: generator <n> <k>\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Generates a tree with n nodes and k paths.\n");
  exit(1);
}

void parseCommandLineArgs(int argc, char** argv) {
  if (argc != 3) {
    usage();
  }

  n = atoi(argv[1]);
  k = atoi(argv[2]);
}

void initRng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

int rand2(int min, int max) {
  return min + rand() % (max - min + 1);
}

int main(int argc, char** argv) {
  parseCommandLineArgs(argc, argv);
  initRng();

  printf("%d %d\n", n, k);
  for (int i = 2; i <= n; i++) {
    printf("%d %d\n", i, rand2(1, i - 1));
  }
  for (int i = 0; i < k; i++) {
    printf("%d %d\n", rand2(1, n), rand2(1, n));
  }

  return 0;
}
