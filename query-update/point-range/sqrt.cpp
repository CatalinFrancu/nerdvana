// Complexity: O(Q sqrt N)
//
// Method: Square root decomposition
#include "common.h"
#include <math.h>

long long b[MAX_BUCKETS];
int bs, nb;

void init_buckets() {
  nb = sqrt(n);
  bs = n / nb + 1;

  for (int i = 0; i < nb; i++) {
    int bucket_start = i * bs;
    for (int j = 0; j < bs; j++) {
      b[i] += v[j + bucket_start];
    }
  }
}

// naively computes the sum of the range [l, r)
long long fragment_sum(int l, int r) {
  return array_sum(v, l, r);
}

long long bucket_sum(int l, int r) {
  return array_sum(b, l, r);
}

// computes the sum of the range [l, r)
long long range_sum(int l, int r) {
  int bl = l / bs, br = r / bs;
  if (bl == br) {
    return fragment_sum(l, r);
  } else {
    return
      // loose ends
      fragment_sum(l, (bl + 1) * bs) +
      fragment_sum(br * bs, r) +
      // buckets spanned entirely
      bucket_sum(bl + 1, br);
  }
}

void point_add(int pos, int val) {
  v[pos] += val;
  b[pos / bs] += val;
}

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      point_add(q[i].x - 1, q[i].y);
    } else {
      answer[num_answers++] = range_sum(q[i].x - 1, q[i].y);
    }
  }
}

int main() {

  read_data();
  mark_time();

  init_buckets();
  process_ops();

  report_time("SQRT decomposition");
  write_data();

  return 0;
}
