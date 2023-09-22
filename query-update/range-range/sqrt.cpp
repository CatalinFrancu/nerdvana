// Complexity: O(Q sqrt N)
//
// Method: Square root decomposition
#include "common.h"
#include <math.h>

// bsum: sum of the elements in each bucket as written in the array
// bdelta: value to add to every element
long long bsum[MAX_BUCKETS], bdelta[MAX_BUCKETS];
int bs, nb;

// naively computes the sum of the range [l, r)
long long fragment_sum(int l, int r, int bucket) {
  long long sum = (r - l) * bdelta[bucket];
  while (l < r) {
    sum += v[l++];
  }
  return sum;
}

// computes the sum of the range [l, r)
long long range_sum(int l, int r) {
  int bl = l / bs, br = r / bs;
  if (bl == br) {
    return fragment_sum(l, r, bl);
  } else {
    // loose ends
    long long sum =
      fragment_sum(l, (bl + 1) * bs, bl) +
      fragment_sum(br * bs, r, br);

    // buckets spanned entirely
    for (int i = bl + 1; i < br; i++) {
      sum += bsum[i] + bs * bdelta[i];
    }
    return sum;
  }
}

// naively adds val the range [l, r), contained in bucket
void fragment_add(int l, int r, int bucket, int val) {
  bsum[bucket] += (r - l) * val;
  while (l < r) {
    v[l++] += val;
  }
}

void range_add(int l, int r, int val) {
  int bl = l / bs, br = r / bs;
  if (bl == br) {
    fragment_add(l, r, bl, val);
  } else {
    // loose ends
    fragment_add(l, (bl + 1) * bs, bl, val);
    fragment_add(br * bs, r, br, val);

    // fully spanned buckets
    for (int i = bl + 1; i < br; i++) {
      bdelta[i] += val;
    }
  }
}

int main() {

  read_data();
  mark_time();

  nb = sqrt(n + 1);
  bs = n / nb + 1;

  for (int i = 0; i < nb; i++) {
    int bucket_start = i * bs;
    for (int j = 0; j < bs; j++) {
      bsum[i] += v[j + bucket_start];
    }
  }

  for (int i = 0; i < num_queries; i++) {
    q[i].r++; // use the [x, y) interval, 1-based
    if (q[i].t == OP_UPDATE) {
      range_add(q[i].l, q[i].r, q[i].val);
    } else {
      answer[num_answers++] = range_sum(q[i].l, q[i].r);
    }
  }

  report_time("SQRT decomposition");
  write_data();

  return 0;
}
