#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int n, maxValue;

void usage() {
  fprintf(stderr, "Usage: generator <n> <maxValue>\n");
  exit(1);
}

void parseCommandLineArgs(int argc, char** argv) {
  if (argc != 3) {
    usage();
  }

  n = atoi(argv[1]);
  maxValue = atoi(argv[2]);
}

void initRng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

int rand2(int x, int y) {
  return rand() % (y - x + 1) + x;
}

void writeData() {
  printf("%d\n", n);

  for (int i = 0; i < n; i++) {
    printf("%d ", rand2(1, maxValue));
  }
  printf("\n");
}

int main(int argc, char** argv) {

  parseCommandLineArgs(argc, argv);
  initRng();
  writeData();

  return 0;
}
