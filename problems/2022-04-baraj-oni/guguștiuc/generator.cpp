#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

void usage() {
  fprintf(stderr, "Usage: generator <n> <q> <max_val>\n");
  _exit(1);
}

int rand2(int x, int y) {
  return rand() % (y - x + 1) + x;
}

int main(int argc, char** argv) {
  if (argc != 4) {
    usage();
  }

  int n = atoi(argv[1]);
  int q = atoi(argv[2]);
  int max_val = atoi(argv[3]);

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  printf("%d %d\n", n, q);

  for (int i = 0; i < n; i++) {
    int x, y;
    do {
      x = rand2(1, max_val);
      y = rand2(1, max_val);
    } while (x >= y);
    printf("%d %d\n", x, y);
  }

  for (int i = 0; i < q; i++) {
    int type = rand2(1, 2), t = rand2(1, max_val);
    printf("%d %d\n", type, t);
  }

  return 0;
}
