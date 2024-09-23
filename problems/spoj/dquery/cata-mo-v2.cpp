// Complexity: O((n + q) sqrt n).
//
// Mo's algorithm. Alternate sorting right endpoints in increasing and
// decreasing order.
#include <algorithm>
#include <stdio.h>

const int MAX_N = 30'000;
const int BLOCK_SIZE = 173;
const int MAX_QUERIES = 200'000;
const int MAX_VAL = 1'000'000;

struct query {
  short l, r;
  int orig_index;
};

int v[MAX_N];
int f[MAX_VAL + 1], num_nonzero;
query q[MAX_QUERIES];
short answer[MAX_QUERIES]; // separate from q[] to avoid resorting at the end
int n, num_queries;

void read_array() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d", &v[i]);
  }
}

void read_queries() {
  scanf("%d", &num_queries);
  for (int i = 0; i < num_queries; i++) {
    int l, r;
    scanf("%d %d", &l, &r);
    q[i].l = l - 1;
    q[i].r = r - 1;
    q[i].orig_index = i;
  }
}

void sort_queries_by_left_block() {
  std::sort(q, q + num_queries, [](query a, query b) {
    int x = a.l / BLOCK_SIZE, y = b.l / BLOCK_SIZE;
    if (x != y) {
      return (x < y);
    } else if (x % 2) {
      return a.r > b.r;
    } else {
      return a.r < b.r;
    }
  });
}

void add(int pos) {
  f[v[pos]]++;
  num_nonzero += (f[v[pos]] == 1);
}

void remove(int pos) {
  f[v[pos]]--;
  num_nonzero -= (f[v[pos]] == 0);
}

void mo() {
  int l = 0, r = -1; // empty
  for (int i = 0; i < num_queries; i++) {
    while (l > q[i].l) {
      add(--l);
    }
    while (r < q[i].r) {
      add(++r);
    }
    while (l < q[i].l) {
      remove(l++);
    }
    while (r > q[i].r) {
      remove(r--);
    }
    answer[q[i].orig_index] = num_nonzero;
  }
}

void write_answers() {
  for (int i = 0; i < num_queries; i++) {
    printf("%d\n", answer[i]);
  }
}

int main() {
  read_array();
  read_queries();
  sort_queries_by_left_block();
  mo();

  write_answers();

  return 0;
}
