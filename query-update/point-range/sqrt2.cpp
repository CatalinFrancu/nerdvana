// Author: Cătălin Frâncu
// Complexity: O(Q sqrt N)
//
// Method: Square root decomposition with hard-coded, power-of-2 bucket size.
#include "common.h"

long long b[MAX_BUCKETS];
int nb;

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
  int bl = l >> P2_BUCKET_BITS, br = r >> P2_BUCKET_BITS;
  if (bl == br) {
    return fragment_sum(l, r);
  } else {
    // loose ends
    long long sum = fragment_sum(l, (bl + 1) << P2_BUCKET_BITS);
    sum += fragment_sum(br << P2_BUCKET_BITS, r);

    // buckets spanned entirely
    for (int i = bl + 1; i < br; i++) {
      sum += b[i];
    }
    return sum;
  }
}

void point_add(int pos, int val) {
  v[pos] += val;
  b[pos >> P2_BUCKET_BITS] += val;
}

int main() {

  read_data();
  mark_time();

  nb = (n >> P2_BUCKET_BITS) + 1;

  for (int i = 0; i < nb; i++) {
    int bucket_start = i << P2_BUCKET_BITS;
    for (int j = 0; j < P2_BUCKET_SIZE; j++) {
      b[i] += v[j + bucket_start];
    }
  }

  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      point_add(q[i].x, q[i].y);
    } else {
      q[i].y++; // use the [x, y) interval, 1-based
      answer[num_answers++] = range_sum(q[i].x, q[i].y);
    }
  }

  report_time("SQRT decomposition (2^k bucket)");
  write_data();

  return 0;
}
