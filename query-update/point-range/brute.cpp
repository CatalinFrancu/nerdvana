// Complexity: O(QN)
//
// Method: brute force
#include "common.h"

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      v[q[i].x] += q[i].y;
    } else {
      answer[num_answers++] = array_sum(v, q[i].x, q[i].y + 1);
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
