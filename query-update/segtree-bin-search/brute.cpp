// Complexity: O(Q log N)
//
// Method: segment tree, optimized
#include "common.h"

const int INFINITY = 1'000'000'000;

void place_sentinel() {
  v[n + 1] = INFINITY;
}

void update(int pos, int val) {
  v[pos] = val;
}

int find_sum(int target_sum) {
  int sum = v[1], i = 1;

  while (sum < target_sum) {
    sum += v[++i];
  }

  return i;
}

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].type == OP_UPDATE) {
      update(q[i].pos, q[i].val);
    } else {
      answer[num_answers++] = find_sum(q[i].val);
    }
  }
}

int main() {

  read_data();
  mark_time();

  place_sentinel();
  process_ops();

  report_time("brute force");
  write_data();

  return 0;
}
