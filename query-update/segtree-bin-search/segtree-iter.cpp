// Complexity: O(Q log N)
//
// Method: segment tree, optimized
#include "common.h"

const int INFINITY = 1'000'000'000;

int s[2 * MAX_N];
int orig_n;

void augment_n() {
  orig_n = n;
  n = next_power_of_2(n);
}

void st_update(int pos, int val) {
  int delta = val - s[pos + n];
  for (pos += n; pos; pos >>= 1) {
    s[pos] += delta;
  }
}

void st_build() {
  for (int i = n - 1; i >= 1; i--) {
    s[i] = s[2 * i] + s[2 * i + 1];
  }

  st_update(orig_n, INFINITY);
}

int st_bin_search(int sum) {
  int pos = 1;

  while (pos < n) {
    pos *= 2;
    if (sum > s[pos]) {
      sum -= s[pos++];
    }
  }

  return pos - n;
}

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].type == OP_UPDATE) {
      st_update(q[i].pos - 1, q[i].val);
    } else {
      answer[num_answers++] = 1 + st_bin_search(q[i].val);
    }
  }
}

int main() {

  read_data();
  mark_time();

  augment_n();
  copy_array(s + n, v + 1, orig_n);
  st_build();
  process_ops();

  report_time("segment tree, iterative");
  write_data();

  return 0;
}
