#include <stdio.h>

const int MAX_N = 500'000;
const int T_QUERY = 0;

int v[MAX_N + 1];
int n;

void read_array() {
  scanf("%d", &n);

  for (int i = 1; i <= n; i++) {
    scanf("%d", &v[i]);
  }
}

int process_query(int l, int r, int val) {
  int cnt = 0;
  while (l <= r) {
    cnt += (v[l++] >= val);
  }
  return cnt;
}

void process_update(int pos, int val) {
  v[pos] = val;
}

void process_ops() {
  int num_ops, type, pos1, pos2, val;
  scanf("%d", &num_ops);

  while (num_ops--) {
    scanf("%d", &type);
    if (type == T_QUERY) {
      scanf("%d %d %d", &pos1, &pos2, &val);
      int count = process_query(pos1, pos2, val);
      printf("%d\n", count);
    } else {
      scanf("%d %d", &pos1, &val);
      process_update(pos1, val);
    }
  }
}

int main() {
  read_array();
  process_ops();

  return 0;
}
