// Graph generator
// Contract:
// * with directed = 0, does not allow self loops or multiple edges
// * with directed = 1, does not allow self loops or multiple edges, but
//   DOES allow edges of the form u -> v and v -> u
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <unordered_set>
#include <utility>

typedef std::pair<int, int> edge;

void usage() {
  fprintf(stderr, "Usage: cata-gen <n> <m> <bool_directed>\n");
  _exit(1);
}

struct hashFunction {
  size_t operator()(const edge &e) const {
    return e.first ^ e.second;
  }
};

int rand2(int x, int y) {
  return rand() % (y - x + 1) + x;
}

int main(int argc, char** argv) {
  if (argc != 4) {
    usage();
  }

  int n = atoi(argv[1]);
  int m = atoi(argv[2]);
  int directed = atoi(argv[3]);

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  printf("%d %d\n", n, m);

  std::unordered_set<edge, hashFunction> s;
  edge e, erev;
  bool has_e, has_erev;
  while (m--) {
    do {
      e = { rand() % n, rand() % n };
      erev = { e.second, e.first };
      has_e = (s.find(e) != s.end());
      has_erev = (s.find(erev) != s.end());
    } while ((e.first == e.second) ||
             (directed && has_e) ||
             (!directed && (has_e || has_erev)));
    s.insert(e);
    printf("%d %d\n", e.first, e.second);
  }

  return 0;
}
