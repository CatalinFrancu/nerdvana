// Complexity: O(QN)
//
// Method: brute force
#include "common.h"

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      array_add(v, q[i].l, q[i].r + 1, q[i].val);
    } else {
      answer[num_answers++] = array_sum(v, q[i].l, q[i].r + 1);
    }
  }
}

int main() {

  read_data();
  mark_time();

  process_ops();

  report_time("brute force");
  write_data();

  return 0;
}
