#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

void usage() {
  fprintf(stderr, "Usage: generator <n> <q> <max_val>\n");
  _exit(1);
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

  printf("%d\n", n);

  for (int i = 0; i < n; i++) {
    printf("%d ", rand() % max_val);
  }
  printf("\n");

  printf("%d\n", q);

  while (q--) {
    int x, y;
    do {
      x = rand() % n;
      y = rand() % n;
    } while (x > y);
    printf("%d %d\n", x, y);
  }

  return 0;
}
