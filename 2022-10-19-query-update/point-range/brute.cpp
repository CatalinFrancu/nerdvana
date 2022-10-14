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
      v[q[i].x] += q[i].y;
    } else {
      long long sum = 0;
      while (q[i].x <= q[i].y) {
        sum += v[q[i].x++];
      }
      answer[num_answers++] = sum;
    }
  }

  report_time("brute force");
  write_data();

  return 0;
}
