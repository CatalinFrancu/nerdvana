#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int MAX_NODES = 300'000;

struct node {
  int key, pri;
  int cnt;
  int l, r;
};

struct treap {
  node v[MAX_NODES + 1];
  int n, root;

  node make_node(int key, int pri) {
    return { .key = key, .pri = pri, .cnt = 1, .l = 0, .r = 0 };
  }

  void init() {
    n = 1; // mark node 0 as used since we use 0 to mean NULL
    root = 0;
  }

  void update_cnt(int t) {
    v[t].cnt = 1 + v[v[t].l].cnt + v[v[t].r].cnt;
  }

  void split(int t, int key, int& l, int& r) {
    if (!t) {
      l = r = 0;
    } else if (v[t].key <= key) {
      split(v[t].r, key, v[t].r, r);
      l = t;
      update_cnt(t);
    } else {
      split(v[t].l, key, l, v[t].l);
      r = t;
      update_cnt(t);
    }
  }

  void merge(int& t, int l, int r) {
    if (!l) {
      t = r;
    } else if (!r) {
      t = l;
    } else if (v[l].pri > v[r].pri) {
      merge(v[l].r, v[l].r, r);
      t = l;
    } else {
      merge(v[r].l, l, v[r].l);
      t = r;
    }
    update_cnt(t);
  }

  void insert(int& t, int elem) {
    if (!t) {
      t = elem;
    } else if (v[elem].pri > v[t].pri) {
      split(t, v[elem].key, v[elem].l, v[elem].r);
      t = elem;
    } else {
      insert(v[t].key <= v[elem].key ? v[t].r : v[t].l, elem);
    }
    update_cnt(t);
  }

  void insert(int key) {
    v[n] = make_node(key, rand());
    insert(root, n++);
  }

  void erase(int& t, int key) {
    if (v[t].key == key) {
      merge(t, v[t].l, v[t].r);
    } else {
      erase(key < v[t].key ? v[t].l : v[t].r, key);
    }
    update_cnt(t);
  }

  void erase(int key) {
    erase(root, key);
  }

  int search(int key) {
    int t = root;
    while (t && (key != v[t].key)) {
      t = (key < v[t].key) ? v[t].l : v[t].r;
    }
    return t;
  }

  bool contains(int key) {
    return search(key) != 0;
  }

  int order_of(int key) {
    int result = 0;
    int t = root;
    while (t) {
      if (key > v[t].key) {
        result += 1 + v[v[t].l].cnt;
        t = v[t].r;
      } else {
        t = v[t].l;
      }
    }

    return result;
  }

  int kth_element(int k) {
    int t = root;
    while (k != v[v[t].l].cnt) {
      if (k < v[v[t].l].cnt) {
        t = v[t].l;
      } else {
        k = k - 1 - v[v[t].l].cnt;
        t = v[t].r;
      }
    }
    return v[t].key;
  }
};

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

treap t;
int values[MAX_NODES];

int main() {
  init_rng();

  t.init();

  for (int i = 0; i < MAX_NODES; i++) {
    do {
      values[i] = rand() % 1'000'000'000;
    } while (t.contains(values[i]));
    t.insert(values[i]);
  }

  for (int i = 0; i < MAX_NODES; i++) {
    assert(t.contains(values[i]));
    t.erase(values[i]);
    assert(!t.contains(values[i]));
  }

  return 0;
}
