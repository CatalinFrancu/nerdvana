#include <vector>

const int MAX_NODES = 300'000;
const int INF = 2'000'000'000;

struct node {
  int val;
  std::vector<int> next;
};

struct skip_list {
  node a[MAX_NODES + 2];
  int size;

  void init() {
    size = 2;
    a[0].val = -INF;
    a[1].val = +INF;
    a[0].next.resize(MAX_LEVELS);
    a[1].next.resize(MAX_LEVELS);
    for (int l = 0; l < MAX_LEVELS; l++) {
      a[0].next[l] = 1;
    }
  }

  int get_height() {
    int h = 1;
    while (coin_toss() && (h < MAX_LEVELS)) {
      h++;
    }
    return h;
  }

  void insert(int val) {
    a[size].val = val;
    int h = get_height();
    a[size].next.resize(h);

    int pos = 0;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[a[pos].next[l]].val < val) {
        pos = a[pos].next[l];
      }
      if (l < h) {
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
