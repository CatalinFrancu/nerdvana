#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unordered_set>

const int MAX_NODES = 300'000;
const int INF = 2'000'000'000;

struct treap {
  int key, pri;
  treap *l, *r;

  treap() {
    key = INF;
    pri = INF;
    l = r = NULL;
  }

  treap(int key) {
    this->key = key;
    pri = rand();
    l = r = NULL;
  }

  void split(treap* t, int key, treap** l, treap** r) {
    if (!t) {
      *l = *r = NULL;
    } else if (t->key <= key) {
      split(t->r, key, &t->r, r);
      *l = t;
    } else {
      split(t->l, key, l, &t->l);
      *r = t;
    }
  }

  void insert(treap** t, treap* elem) {
    if (!*t) {
      *t = elem;
    } else if (elem->pri > (*t)->pri) {
      split(*t, elem->key, &elem->l, &elem->r);
      *t = elem;
    } else {
      insert((*t)->key <= elem->key ? &(*t)->r : &(*t)->l, elem);
    }
  }

  void insert(int key) {
    treap* elem = new treap(key);
    this->insert(&this->l, elem);
  }

  treap* search(int key) {
    treap* t = this;
    while (t && key != t->key) {
      t = (key < t->key) ? t->l : t->r;
    }
    return t;
  }

  bool contains(int key) {
    return search(key) != NULL;
  }
};

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

treap* t;
std::unordered_set<int> stl_set;
int values[MAX_NODES];

int main() {
  init_rng();

  for (int i = 0; i < MAX_NODES; i++) {
    values[i] = rand() % INF;
  }

  t = new treap();

  for (int i = 0; i < MAX_NODES; i++) {
    t->insert(values[i]);
    stl_set.insert(values[i]);
    int j = rand() % MAX_NODES;
    assert(t->contains(values[j]) == stl_set.contains(values[j]));
  }

  return 0;
}
