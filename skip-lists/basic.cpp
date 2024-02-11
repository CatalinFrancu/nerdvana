#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unordered_set>

const int MAX_LEVELS = 20;
const int MAX_NODES = 300'000;
const int INFINITY = 2'000'000'000;

struct node {
  int val;
  int height;
  int next[MAX_LEVELS];
};

struct skip_list {
  node a[MAX_NODES + 2];
  int size;

  void init() {
    size = 2;
    a[0].height = a[1].height = MAX_LEVELS;
    a[0].val = -INFINITY;
    a[1].val = +INFINITY;
    for (int l = 0; l < MAX_LEVELS; l++) {
      a[0].next[l] = 1;
    }
  }

  int get_height() {
    int h = 1;
    while ((rand() & 1) && (h < MAX_LEVELS)) {
      h++;
    }
    return h;
  }

  void insert(int val) {
    a[size].val = val;
    a[size].height = get_height();

    int pos = 0;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[a[pos].next[l]].val < val) {
        pos = a[pos].next[l];
      }
      if (l < a[size].height) {
        a[size].next[l] = a[pos].next[l];
        a[pos].next[l] = size;
      }
    }

    size++;
  }

  bool contains(int val) {
    int pos = 0;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[a[pos].next[l]].val < val) {
        pos = a[pos].next[l];
      }
    }

    return (a[a[pos].next[0]].val == val);
  }
};

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

skip_list sl;
std::unordered_set<int> stl_set;
int values[MAX_NODES];

int main() {
  init_rng();

  for (int i = 0; i < MAX_NODES; i++) {
    values[i] = rand() % INFINITY;
  }

  sl.init();

  for (int i = 0; i < MAX_NODES; i++) {
    sl.insert(values[i]);
    stl_set.insert(values[i]);
    int j = rand() % MAX_NODES;
    assert(sl.contains(values[j]) == stl_set.contains(values[j]));
  }

  return 0;
}
