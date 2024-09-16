#include <stdio.h>

const int MAX_N = 200'000;
const int T_UPDATE = 1;

struct naive_array {
  long long v[MAX_N];
  int n;

  void init(int n) {
    this->n = n;
  }

  void set(int pos, int val) {
    v[pos] = val;
  }

  void add_progression(int l, int r) {
    for (int i = l, val = 1; i <= r; i++, val++) {
      v[i] += val;
    }
  }

  long long range_sum(int l, int r) {
    long long sum = 0;
    for (int i = l; i <= r; i++) {
      sum += v[i];
    }
    return sum;
  }
};

naive_array s;
int n, num_ops;

void read_array() {
  scanf("%d %d", &n, &num_ops);
  s.init(n);
  for (int i = 0; i < n; i++) {
    int x;
    scanf("%d", &x);
    s.set(i, x);
  }
}

void process_ops() {
  while (num_ops--) {
    int type, l, r;
    scanf("%d %d %d", &type, &l, &r);
    l--;
    r--;
    if (type == T_UPDATE) {
      s.add_progression(l, r);
    } else {
      printf("%lld\n", s.range_sum(l, r));
    }
  }
}

int main() {
  read_array();
  process_ops();

  return 0;
}
