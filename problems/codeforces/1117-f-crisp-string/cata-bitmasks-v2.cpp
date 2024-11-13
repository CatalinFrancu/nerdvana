// In addition to v1, use simple bool arrays in place of bitsets.
#include <stdio.h>

const int MAX_LEN = 100'000;
const int MAX_SIGMA = 17;
const int NONE = -1;

char s[MAX_LEN + 1];
bool compat[MAX_SIGMA][MAX_SIGMA];
int freq[MAX_SIGMA];
bool bad[1 << MAX_SIGMA], global_bad[1 << MAX_SIGMA];
int n, sigma;

void read_data() {
  scanf("%d %d %s", &n, &sigma, s);
  for (int i = 0; i < sigma; i++) {
    for (int j = 0; j < sigma; j++) {
      int x;
      scanf("%d", &x);
      compat[i][j] = x;
    }
  }
}

void translate_string() {
  for (int i = 0; i < n; i++) {
    s[i] -= 'a';
    freq[(int)s[i]]++;
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void clear_bad() {
  for (int i = 0; i < (1 << sigma); i++) {
    bad[i] = false;
  }
}

void include_submasks(int x, int y) {
  // Only iterate masks that have bits x and y unset.
  unsigned mask = ((1 << sigma) - 1) ^ ((1 << x) | (1 << y));

  bad[mask] = global_bad[mask] = true;
  for (unsigned s = mask; s; s = (s - 1) & mask) {
    // s iterates through submasks of other in decreasing order. We actually
    // want to iterate in increasing order.
    unsigned t = mask ^ s;
    if (!bad[t]) {
      for (int bit = 0; bit < sigma; bit++) {
        if ((t & (1 << bit)) && bad[t ^ (1 << bit)]) {
          bad[t] = global_bad[t] = true;
        }
      }
    }
  }
}

void gen_bad_masks_for_pair(int x, int y) {
  int last_seen = NONE;
  unsigned mask = 0;
  for (int i = 0; i < n; i++) {
    if ((s[i] == x) || (s[i] == y)) {
      if (last_seen == x + y - s[i]) { // the other one
        bad[mask] = global_bad[mask] = true;
      }
      mask = 0;
      last_seen = s[i];
    } else {
      mask |= (1 << s[i]);
    }
  }

  include_submasks(x, y);
  clear_bad();
}

void gen_bad_masks() {
  for (int x = 0; x < sigma; x++) {
    for (int y = x; y < sigma; y++) {
      if (freq[x] && freq[y] && !compat[x][y]) {
        gen_bad_masks_for_pair(x, y);
      }
    }
  }
}

// Mark as bad all masks that are unreachable from 0 via any sequence of
// eliminations.
void gen_unreachable_masks() {
  for (unsigned mask = 1; mask < (1u << sigma); mask++) {
    if (!global_bad[mask]) {
      bool reachable = false;
      for (int bit = 0; bit < sigma; bit++) {
        reachable |=
          (mask & (1 << bit)) &&
          !global_bad[mask ^ (1 << bit)];
      }
      if (!reachable) {
        global_bad[mask] = true;
      }
    }
  }
}

int get_min_pop() {
  int result = n;
  for (unsigned mask = 0; mask < (1u << sigma); mask++) {
    if (!global_bad[mask]) {
      int pop = 0;
      for (int bit = 0; bit < sigma; bit++) {
        if (!(mask & (1 << bit))) {
          pop += freq[bit];
        }
      }
      result = min(result, pop);
    }
  }
  return result;
}

int main() {
  read_data();
  translate_string();
  gen_bad_masks();
  gen_unreachable_masks();
  int answer = get_min_pop();
  printf("%d\n", answer);

  return 0;
}
