#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

void usage() {
  fprintf(stderr, "Usage: generator <scenarios> <n> <k> <max_val>\n");
  _exit(1);
}

int rand2(int x, int y) {
  return rand() % (y - x + 1) + x;
}

int main(int argc, char** argv) {
  if (argc != 5) {
    usage();
  }

  int scenarios = atoi(argv[1]);
  int n = atoi(argv[2]);
  long long k = atoll(argv[3]);
  int max_val = atoi(argv[4]);

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  printf("%d\n", scenarios);
  while (scenarios--) {
    printf("%d %lld\n", n, k);
    for (int i = 0; i < n; i++) {
      printf("%d ", rand2(1, max_val));
    }
    printf("\n");
  }

  return 0;
}
