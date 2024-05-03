#include <assert.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef __gnu_pbds::tree<
  int,
  __gnu_pbds::null_type,
  std::less<int>,
  __gnu_pbds::rb_tree_tag,
  __gnu_pbds::tree_order_statistics_node_update
  > ordered_set;

const int MAX_NODES = 300'000;
const int INF = 2'000'000'000;
const int NIL = 0;

struct node {
  int key, pri;
  int cnt;
  int l, r;
};

struct treap {
  node v[MAX_NODES + 2];
  int n;

  node make_node(int key, int pri) {
    return { .key = key, .pri = pri, .cnt = 1, .l = NIL, .r = NIL };
  }

  void init() {
    v[1] = make_node(INF, INF);
    n = 2;
  }

  void update_cnt(int t) {
    v[t].cnt = 1 + v[v[t].l].cnt + v[v[t].r].cnt;
  }

  void split(int t, int key, int& l, int& r) {
    if (t == NIL) {
      l = r = NIL;
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

  void insert(int& t, int elem) {
    if (t == NIL) {
      t = elem;
    } else if (v[elem].pri > v[t].pri) {
      split(t, v[elem].key, v[elem].l, v[elem].r);
      t = elem;
    } else {
      insert(v[t].key <= v[elem].key ? v[t].r : v[t].l, elem);
    }
    update_cnt(t);
  }

  int search(int key) {
    int t = 1;
    while ((t != NIL) && (key != v[t].key)) {
      t = (key < v[t].key) ? v[t].l : v[t].r;
    }
    return t;
  }

  void insert(int key) {
    v[n++] = make_node(key, rand());
    insert(v[1].l, n - 1);
  }

  bool contains(int key) {
    return search(key) != NIL;
  }

  int order_of(int key) {
    int result = 0;
    int t = 1;
    while (t != NIL) {
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
    int t = 1;
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
ordered_set stl_set;
int values[MAX_NODES];

int main() {
  init_rng();

  t.init();

  for (int i = 0; i < MAX_NODES; i++) {
    int x = rand() % INF;
    // For simplicity, don't deal with multisets.
    if (!t.contains(x)) {
      t.insert(x);
      stl_set.insert(x);
      assert(t.order_of(x) == (int)stl_set.order_of_key(x));
    }
  }

  for (int i = stl_set.size() - 1; i >= 0; i--) {
    assert(t.kth_element(i) == *stl_set.find_by_order(i));
  }

  return 0;
}
