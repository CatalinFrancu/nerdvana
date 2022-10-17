// Author: Cătălin Frâncu
// Complexity: O(Q sqrt N)
//
// Method: Square root decomposition with careless divisions.
#include "common.h"
#include <math.h>

long long b[MAX_BUCKETS];
int bs, nb;

// naively computes the sum of the range [l, r)
long long fragment_sum(int l, int r) {
  long long sum = 0;
  while (l < r) {
    sum += v[l++];
  }
  return sum;
}

// computes the sum of the range [l, r)
long long range_sum(int l, int r) {
  int bl = l / bs, br = r / bs;
  if (bl == br) {
    return fragment_sum(l, r);
  } else {
    // loose ends
    long long sum = 0;
    do {
      sum += v[l++];
    } while (l % bs);

    while (r % bs) {
      sum += v[--r];
    }

    // buckets spanned entirely
    for (int i = bl + 1; i < br; i++) {
      sum += b[i];
    }
    return sum;
  }
}

void point_add(int pos, int val) {
  v[pos] += val;
  b[pos / bs] += val;
}

int main() {

  read_data();
  mark_time();

  nb = sqrt(n + 1);
  bs = n / nb + 1;

  for (int i = 0; i < nb; i++) {
    int bucket_start = i * bs;
    for (int j = 0; j < bs; j++) {
      b[i] += v[j + bucket_start];
    }
  }

  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == T_UPDATE) {
      point_add(q[i].x, q[i].y);
    } else {
      q[i].y++; // use the [x, y) interval, 1-based
      answer[num_answers++] = range_sum(q[i].x, q[i].y);
    }
  }

  report_time("SQRT decomposition w/ divisions");
  write_data();

  return 0;
}
