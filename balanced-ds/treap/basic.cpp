#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unordered_set>

const int MAX_NODES = 300'000;
const int INF = 2'000'000'000;
const int NIL = -1;

struct node {
  int key, pri;
  int l, r;
};

struct treap {
  node v[MAX_NODES + 1];
  int n;

  void init() {
    v[0] = {.key = INF, .pri = INF, .l = NIL, .r = NIL };
    n = 1;
  }

  void split(int t, int key, int& l, int& r) {
    if (t == NIL) {
      l = r = NIL;
    } else if (v[t].key <= key) {
      split(v[t].r, key, v[t].r, r);
      l = t;
    } else {
      split(v[t].l, key, l, v[t].l);
      r = t;
    }
  }

  void insert(int& t, int elem) {
    if (t == NIL) {
      t = elem;
    } else if (v[elem].pri > v[t].pri) {
      split(t, v[elem].key, v[elem].l, v[elem].r);
      t = elem;
    } else {
      insert(v[t].key <= v[elem].key ? v[t].r : v[t].l, elem);
    }
  }

  int search(int key) {
    int t = 0;
    while ((t != NIL) && (key != v[t].key)) {
      t = (key < v[t].key) ? v[t].l : v[t].r;
    }
    return t;
  }

  void insert(int key) {
    v[n++] = { .key = key, .pri = rand(), .l = NIL, .r = NIL };
    insert(v[0].l, n - 1);
  }

  bool contains(int key) {
    return search(key) != NIL;
  }
};

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

treap t;
std::unordered_set<int> stl_set;
int values[MAX_NODES];

int main() {
  init_rng();

  for (int i = 0; i < MAX_NODES; i++) {
    values[i] = rand() % INF;
  }

  t.init();

  for (int i = 0; i < MAX_NODES; i++) {
    t.insert(values[i]);
    stl_set.insert(values[i]);
    int j = rand() % MAX_NODES;
    assert(t.contains(values[j]) == stl_set.contains(values[j]));
  }

  return 0;
}