// Complexity: O(Q sqrt N)
//
// Method: Square root decomposition with careless divisions.
#include "common.h"
#include <math.h>

long long b[MAX_BUCKETS];
int bs, nb;

void init_buckets() {
  nb = sqrt(n + 1);
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
    // loose ends
    long long sum = 0;
    do {
      sum += v[l++];
    } while (l % bs);

    while (r % bs) {
      sum += v[--r];
    }

    // buckets spanned entirely
    sum += bucket_sum(bl + 1, br);
    return sum;
  }
}

void point_add(int pos, int val) {
  v[pos] += val;
  b[pos / bs] += val;
}

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      point_add(q[i].x, q[i].y);
    } else {
      q[i].y++; // use the [x, y) interval, 1-based
      answer[num_answers++] = range_sum(q[i].x, q[i].y);
    }
  }
}

int main() {

  read_data();
  mark_time();

  init_buckets();
  process_ops();

  report_time("SQRT decomposition w/ divisions");
  write_data();

  return 0;
}
