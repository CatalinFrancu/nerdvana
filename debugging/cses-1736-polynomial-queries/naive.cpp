#include <stdio.h>

const int MAX_N = 200'000;
const int T_UPDATE = 1;

long long v[MAX_N + 1];
int n, num_ops;

void read_array() {
  scanf("%d %d", &n, &num_ops);
  for (int i = 1; i <= n; i++) {
    scanf("%lld", &v[i]);
  }
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

void process_ops() {
  while (num_ops--) {
    int type, l, r;
    scanf("%d %d %d", &type, &l, &r);
    if (type == T_UPDATE) {
      add_progression(l, r);
    } else {
      printf("%lld\n", range_sum(l, r));
    }
  }
}

int main() {
  read_array();
  process_ops();

  return 0;
}
