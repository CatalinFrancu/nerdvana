// Author: Cătălin Frâncu
// Complexity: O(QN)
//
// Method: brute force
#include "common.h"

int main() {

  read_data();
  mark_time();

  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == T_UPDATE) {
      while (q[i].l <= q[i].r) {
        v[q[i].l++] += q[i].val;
      }
    } else {
      long long sum = 0;
      while (q[i].l <= q[i].r) {
        sum += v[q[i].l++];
      }
      answer[num_answers++] = sum;
    }
  }

  report_time("brute force");
  write_data();

  return 0;
}
