// In addition to v1, we only sort the smaller left half of 3^12 elements. We
// then iterate through the larger right half of 3^13 elements. We look those
// up using binary search.
#include <algorithm>
#include <stdio.h>

const int MAX_QUESTS = 25;
const int MAX_SUBSETS = 2'125'764; // 3^13 + 3^12
const long long INFINITY = 300'000'000;
const int NONE = -1;

const bool LEFT = false;
const bool RIGHT = true;

struct attitude {
  int a, b, c;
  int tritmask;

  bool equiv(const attitude& other) {
    return
      (b - a == other.b - other.a) &&
      (c - a == other.c - other.a);
  }

  bool complementary(const attitude& other) {
    return
      (a + other.a == b + other.b) &&
      (a + other.a == c + other.c);
  }
};

attitude att[MAX_QUESTS];
attitude sub[MAX_SUBSETS];
int n, mid, num_subsets;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d %d %d", &att[i].a, &att[i].b, &att[i].c);
  }
  mid = n / 2;
}

int best_sum;
int best_left_mask, best_right_mask;

bool too_large(attitude& x, attitude& y) {
  return
    (x.b + y.b > x.a + y.a) ||
    ((x.b + y.b == x.a + y.a) && (x.c + y.c > x.a + y.a));
}

int bin_search(attitude& h) {
  int l = 0, r = num_subsets;
  while (r - l > 1) {
    int mid = (l + r) / 2;
    if (too_large(h, sub[mid])) {
      r = mid;
    } else {
      l = mid;
    }
  }

  return h.complementary(sub[l]) ? l : NONE;
}

void process_right_attitude(attitude& h) {
  int pos = bin_search(h);
  if ((pos != NONE) && (sub[pos].a + h.a > best_sum)) {
    best_sum = sub[pos].a + h.a;
    best_left_mask = sub[pos].tritmask;
    best_right_mask = h.tritmask;
  }
}

void left_rec(int k, attitude h)  {
  if (k < 0) {
    sub[num_subsets++] = h;
  } else {
    // a, b and c stay home respectively
    left_rec(k - 1, { h.a, h.b + att[k].b, h.c + att[k].c, h.tritmask * 3});
    left_rec(k - 1, { h.a + att[k].a, h.b, h.c + att[k].c, h.tritmask * 3 + 1});
    left_rec(k - 1, { h.a + att[k].a, h.b + att[k].b, h.c, h.tritmask * 3 + 2});
  }
}

void right_rec(int k, attitude h)  {
  if (k < mid) {
    process_right_attitude(h);
  } else {
    // a, b and c stay home respectively
    right_rec(k - 1, { h.a, h.b + att[k].b, h.c + att[k].c, h.tritmask * 3});
    right_rec(k - 1, { h.a + att[k].a, h.b, h.c + att[k].c, h.tritmask * 3 + 1});
    right_rec(k - 1, { h.a + att[k].a, h.b + att[k].b, h.c, h.tritmask * 3 + 2});
  }
}

void sort_subsets() {
  std::sort(sub, sub + num_subsets, [] (attitude& p, attitude& q) {
    if (p.b - p.a != q.b - q.a) {
      return (p.b - p.a < q.b - q.a);
    } else if (p.c - p.a != q.c - q.a) {
      return (p.c - p.a < q.c - q.a);
    } else {
      return p.a > q.a;
    }
  });
}

void remove_duplicates() {
  int cnt = 1;
  for (int i = 1; i < num_subsets; i++) {
    if (!sub[i].equiv(sub[cnt - 1])) {
      sub[cnt++] = sub[i];
    }
  }
  num_subsets = cnt;
}

void print_tritmask(int tritmask, int size) {
  for (int i = 0; i < size; i++) {
    switch (tritmask % 3) {
      case 0:
        printf("MW\n");
        break;
      case 1:
        printf("LW\n");
        break;
      default:
        printf("LM\n");
    }
    tritmask /= 3;
  }
}

void print_solution() {
  if (best_sum == -INFINITY) {
    printf("Impossible\n");
  } else {
    print_tritmask(best_left_mask, mid);
    print_tritmask(best_right_mask, n - mid);
  }
}

int main() {
  read_data();

  left_rec(mid - 1, { 0, 0, 0, 0 });
  sort_subsets();
  remove_duplicates();
  best_sum = -INFINITY;
  right_rec(n - 1, { 0, 0, 0, 0 });
  print_solution();

  return 0;
}
