#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <unordered_set>

#define T_APPLE 1
#define T_PEAR 2

typedef std::pair<int, int> point;

struct hashFunction {
  size_t operator()(const point &p) const {
    return p.first ^ p.second;
  }
};

void usage() {
  fprintf(stderr, "Usage: generator <n> <num_apples>\n");
  _exit(1);
}

int rand2(int x, int y) {
  return rand() % (y - x + 1) + x;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    usage();
  }

  int n = atoi(argv[1]);
  int apples = atoi(argv[2]);
  int pears = n - apples;

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  printf("%d\n", n);

  std::unordered_set<point, hashFunction> s;
  for (int i = 0; i < n; i++) {
    point p;
    int t;

    do {
      p.first = rand2(1, n);
      p.second = rand2(1, n);
    } while (s.find(p) != s.end());
    s.insert(p);

    if (rand() % (apples + pears) < apples) {
      t = 1;
      apples--;
    } else {
      t = 2;
      pears--;
    }

    printf("%d %d %d\n", p.first, p.second, t);
  }

  return 0;
}
