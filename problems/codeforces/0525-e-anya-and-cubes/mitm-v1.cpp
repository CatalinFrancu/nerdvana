// Method: Meet in the middle.
//
// If k=n, then the naive solution would consider 3^n possibilities. For every
// number we can (0) not take it, (1) take the number itself or (2) take the
// number's factorial. Instead, let us do these for the left 13 and right 12
// numbers, for a (hypothetical) total of 3^13 + 3^12 ≈ 2 million choices.
//
// We only generate and store the valid possibilities, not exceeding S or k.
//
// To merge the two halves, we sort them and use the two pointer technique to
// find elements whose sum is S. If the left value is x and the right value is
// y=S-x, then we consider the ranges of x values in the left-hand side and y
// values in the right-hand side. We sort those by the number of stickers used
// and run a nested two pointer technique to count the pairs using no more k
// stickers.
#include <algorithm>
#include <stdio.h>

const int MAX_N = 25;
const int MAX_FACTORIAL = 18;   // 19! > 10^16
const int MAX_LEFT = 1'594'323; // 3^13
const int MAX_RIGHT = 531'441;  // 3^12

int a[MAX_N], n, num_stickers;
long long target_sum;
long long fact[MAX_FACTORIAL + 1];

struct info {
  long long sum;
  char stickers;

  bool before(info other) {
    return (sum < other.sum) ||
      ((sum == other.sum) & (stickers < other.stickers));
  }
};

info left[MAX_LEFT + 2], right[MAX_RIGHT + 2];
int num_left, num_right;

void precompute_factorials() {
  fact[0] = 1;
  for (int i = 1; i <= MAX_FACTORIAL; i++) {
    fact[i] = fact[i - 1] * i;
  }
}

void read_data() {
  scanf("%d %d %lld", &n, &num_stickers, &target_sum);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
  }
}

struct subset_generator {
  int end;
  info* dest;
  int size;

  void rec(int k, long long sum, char stickers) {
    if (k == end) {
      dest[size++] = { sum, stickers };
      return;
    }

    rec(k + 1, sum, stickers);
    if (sum + a[k] <= target_sum) {
      rec(k + 1, sum + a[k], stickers);
    }
    if ((stickers < num_stickers) &&
        (a[k] <= MAX_FACTORIAL) &&
        (sum + fact[a[k]] <= target_sum)) {
      rec(k + 1, sum + fact[a[k]], stickers + 1);
    }
  }

  void run(int start, int end, info* dest, int& size) {
    this->end = end;
    this->dest = dest;
    this->size = 0;

    rec(start, 0, 0);

    size = this->size;
    // infinite sentinels
    dest[size++] = { -(target_sum + 1), 0 };
    dest[size++] = { (target_sum + 1), 0 };

    std::sort(dest, dest + size, [](info a, info b) {
      return a.before(b);
    });
  }
};

subset_generator sub_gen;

// Capsule to isolate some variables.
struct half_merger {
  int l, r;
  long long result;

  void merge_equal_sums() {
    long long l_sum = left[l].sum;
    int r_start = r;
    while (right[r_start].sum == right[r].sum) {
      r_start--;
    }

    while (left[l].sum == l_sum) {
      if ((r > r_start) &&
          (left[l].stickers + right[r].stickers > num_stickers)) {
        r--;
      } else {
        result += r - r_start;
        l++;
      }
    }

    r = r_start;
  }

  long long run() {
    result = 0;
    l = 0;
    r = num_right - 1;

    while (l < num_left) {
      if (left[l].sum + right[r].sum < target_sum) {
        l++;
      } else if (left[l].sum + right[r].sum > target_sum) {
        r--;
      } else {
        merge_equal_sums();
      }
    }

    return result;
  }
};

half_merger merger;

int main() {
  precompute_factorials();
  read_data();
  sub_gen.run(n / 2, n, left, num_left);
  sub_gen.run(0, n / 2, right, num_right);
  long long ans = merger.run();
  printf("%lld\n", ans);

  return 0;
}
