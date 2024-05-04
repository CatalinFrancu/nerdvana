#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <vector>

const int MAX_NODES = 1'000;
const int MAX_VAL = 1'000'000;
const int NIL = 0;

struct node {
  int val, pri;
  int cnt;
  int l, r;
};

struct treap {
  node v[10 * MAX_NODES + 2]; // no garbage collection
  int n;

  node make_node(int val, int pri) {
    return { .val = val, .pri = pri, .cnt = 1, .l = NIL, .r = NIL };
  }

  void init() {
    v[1] = make_node(MAX_VAL, RAND_MAX);
    n = 2;
  }

  void update_cnt(int t) {
    v[t].cnt = t ? (1 + v[v[t].l].cnt + v[v[t].r].cnt) : 0;
  }

  void split(int t, int pos, int& l, int& r) {
    if (t == NIL) {
      l = r = NIL;
    } else if (pos <= v[v[t].l].cnt) {
      split(v[t].l, pos, l, v[t].l);
      r = t;
      update_cnt(t);
    } else {
      split(v[t].r, pos - v[v[t].l].cnt - 1, v[t].r, r);
      l = t;
      update_cnt(t);
    }
  }

  void merge(int& t, int l, int r) {
    if (l == NIL) {
      t = r;
    } else if (r == NIL) {
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

  void insert(int& t, int pos, int elem) {
    if (t == NIL) {
      t = elem;
    } else if (v[elem].pri > v[t].pri) {
      split(t, pos, v[elem].l, v[elem].r);
      t = elem;
    } else if (pos <= v[v[t].l].cnt) {
      insert(v[t].l, pos, elem);
    } else {
      insert(v[t].r, pos - v[v[t].l].cnt - 1, elem);
    }
    update_cnt(t);
  }

  void insert(int pos, int val) {
    v[n++] = make_node(val, rand());
    insert(v[1].l, pos, n - 1);
  }

  void erase(int& t, int pos) {
    if (pos == v[v[t].l].cnt) {
      merge(t, v[t].l, v[t].r);
    } else if (pos < v[v[t].l].cnt) {
      erase(v[t].l, pos);
    } else {
      erase(v[t].r, pos - 1 - v[v[t].l].cnt);
    }
    update_cnt(t);
  }

  void erase(int pos) {
    erase(v[1].l, pos);
  }

  int get(int pos) {
    int t = 1;
    while (pos != v[v[t].l].cnt) {
      if (pos < v[v[t].l].cnt) {
        t = v[t].l;
      } else {
        pos -= v[v[t].l].cnt + 1;
        t = v[t].r;
      }
    }
    return v[t].val;
  }

  void print(int t) {
    if (t == NIL) {
      printf("NIL");
    } else {
      printf("(");
      print(v[t].l);
      printf(" t=%d,v=%d,p=%d,c=%d ", t, v[t].val, v[t].pri, v[t].cnt);
      print(v[t].r);
      printf(")");
    }
  }

  void print() {
    print(v[1].l);
    printf("\n");
  }
};

treap t;
std::vector<int> v;
int values[MAX_NODES];

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

void compare() {
  for (unsigned i = 0; i < v.size(); i++) {
    assert(v[i] == t.get(i));
  }
}

void insert_op() {
  int pos = rand() % (v.size() + 1);
  int val = rand() % (MAX_VAL + 1);
  v.insert(v.begin() + pos, val);
  t.insert(pos, val);
  compare();
}

void erase_op() {
  int pos = rand() % v.size();
  v.erase(v.begin() + pos);
  t.erase(pos);
  compare();
}

int main() {
  init_rng();

  t.init();

  // Fill to 50%
  for (int i = 0; i < MAX_NODES / 2; i++) {
    insert_op();
  }

  // Play with it for a while
  for (int i = 0; i < MAX_NODES * 10; i++) {
    bool coin = rand() & 1;
    if (!v.size() || (coin && (v.size() < MAX_NODES))) {
      insert_op();
    } else {
      erase_op();
    }
  }

  // Flush it
  while (v.size()) {
    erase_op();
  }

  return 0;
}
