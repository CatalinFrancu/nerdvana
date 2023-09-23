// Complexity: O(QN)
//
// Method: brute force
#include "common.h"

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      mat[q[i].row1][q[i].col1] += q[i].val;
    } else {
      answer[num_answers++] = matrix_sum(mat, q[i].row1, q[i].col1,
                                         q[i].row2 + 1, q[i].col2 + 1);
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
