#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

const int TRIALS = 100;
const int N = 1'000'000;
const int DECAY = 100;
const int SIGMA = 6;
const int INFINITY = 1'000'000;

struct sort_suffix_array {
  const char* s;
  int n, len;

  // The suffix array. sa[i] = the i-th smallest substring
  int sa[N + 1];
  // ord[i] = sa^{-1}. ord[i] = the order of s[i...end] (with duplicates)
  int ord[N + 1];
  // cl[i] = the class of sa[i], with duplicates
  int cl[N + 1];

  void init(char* s, int n) {
    this->s = s;
    this->n = n;
  }

  bool cmp(int i, int j) {
    if (ord[i] != ord[j]) {
      return (ord[i] < ord[j]);
    }

    i += len;
    j += len;

    return (i < n && j < n)
      ? (ord[i] < ord[j])
      : (i > j);
  }

  void build() {
    for (int i = 0; i < n; i++) {
      sa[i] = i;
      ord[i] = s[i];
      cl[i] = 0;
    }

    len = 1;
    while (cl[n - 1] != n - 1) {
      std::sort(sa, sa + n, [this](int i, int j) {
        return cmp(i, j);
      });
      for (int i = 1; i < n; i++) {
        cl[i] = cl[i - 1] + cmp(sa[i - 1], sa[i]);
      }
      for (int i = 1; i < n; i++) {
        ord[sa[i]] = cl[i];
      }
      len *= 2;
    }
  }

  void verify() {
    for (int i = 0; i < n- 1; i++) {
      assert(strcmp(s + sa[i], s + sa[i + 1]) <= 0);
    }
  }
};

struct radix_suffix_array {
  const char* s;
  int p[N + 1], c[N + 1], p2[N + 1], c2[N + 1];
  int cnt[256];
  int n, num_classes;

  void init(char* s, int n) {
    this->s = s;
    this->n = n;
  }

  int circ(int x, int n) {
    return (x >= n) ? (x - n) : x;
  }

  void build_single_letters() {
    // Letter frequencies and partial sums.
    memset(cnt, 0, sizeof(int) * 256);
    for (int i = 0; i < n; i++) {
      cnt[(int)s[i]]++;
    }
    for (int i = 1; i <= 'z'; i++) {
      cnt[i] += cnt[i - 1];
    }
    // Now cnt[x] is the number of letters <= x.

    // Make p store the order of length 1 substrings.
    for (int i = 0; i < n; i++) {
      p[--cnt[(int)s[i]]] = i;
    }

    // Compute character classes in the order given by p.
    c[p[0]] = 0;
    for (int i = 1; i < n; i++) {
      c[p[i]] = c[p[i - 1]] + (s[p[i]] != s[p[i - 1]]);
    }
    num_classes = 1 + c[p[n - 1]];
  }

  void build_prefix_len(int len) {
    // Class frequencies and partial sums.
    memset(cnt, 0, sizeof(int) * num_classes);
    for (int i = 0; i < n; i++) {
      cnt[c[i]]++;
    }
    for (int i = 1; i < num_classes; i++) {
      cnt[i] += cnt[i - 1];
    }
    // Now cnt[x] is the frequency of classes <= x.

    // Redistribuite values in p[] in classes of sizes given by cnt, in the
    // order given by p2[].
    for (int i = 0; i < n; i++) {
      p2[i] = circ(p[i] - len + n, n);
    }
    for (int i = n - 1; i >= 0; i--) {
      p[--cnt[c[p2[i]]]] = p2[i];
    }

    // Recompute the new classes in c2, then move them in c.
    c2[p[0]] = 0;
    for (int i = 1; i < n; i++) {
      bool diff =  (c[p[i]] != c[p[i - 1]]) ||
        (c[circ(p[i] + len, n)] != c[circ(p[i - 1] + len, n)]);
      c2[p[i]] = c2[p[i - 1]] + diff;
    }
    num_classes = 1 + c2[p[n - 1]];
    memcpy(c, c2, sizeof(int) * n);
  }

  void build() {
    build_single_letters();

    // Prefix doubling step.
    int len = 1;
    while (num_classes < n) {
      build_prefix_len(len);
      len <<= 1;
    }
  }

  void verify() {
    for (int i = 0; i < n; i++) {
      assert(strcmp(s + p[i], s + p[i + 1]) <= 0);
    }
  }
};

sort_suffix_array sort_suf;
radix_suffix_array radix_suf;
char s[N + 1];
int p[N + 1];
long long _time;

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int seed = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(seed);
}

long long get_time() {
  struct timeval time;
  gettimeofday(&time, NULL);
  long long v = 1000ll * time.tv_sec + time.tv_usec / 1000;
  return v;
}

void mark_time() {
  _time = get_time();
}

int report_time(const char* msg) {
  long long delta = get_time() - _time;
  fprintf(stderr, "%s: %lld ms\n", msg, delta);
  return delta;
}

void gen_string() {
  for (int i = 0; i < N; i++) {
    int c = 0;
    while (c < SIGMA - 1 && rand() % DECAY == 0) {
      c++;
    }
    s[i] = 'a' + c;
  }
  s[N] = '\0';
}

void swap(int i, int j) {
  int tmp = p[i];
  p[i] = p[j];
  p[j] = tmp;
}

void tsort(int left, int right, int pos) {
  if (right - left <= 1) {
    return;
  }

  int i = left, lt = left, gt = right;
  char pivot = s[p[left] + pos];

  while (i < gt) {
    char x = s[p[i] + pos];
    if (x < pivot) {
      swap(i++, lt++);
    } else if (x > pivot)  {
      swap(i, --gt);
    } else {
      i++;
    }
  }

  tsort(left, lt, pos);
  if (s[p[lt] + pos] != '\0') {
    tsort(lt, gt, pos + 1);
  }
  tsort(gt, right, pos);
}

void build_with_ternary_qsort() {
  for (int i = 0; i < N; i++) {
    p[i] = i;
  }
  tsort(0, N, 0);
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void benchmark_radix_sort(int& sum_t, int& min_t, int& max_t) {
  mark_time();
  radix_suf.init(s, N + 1);
  radix_suf.build();
  int t = report_time("Construcție cu radix sort");
  radix_suf.verify();
  sum_t += t;
  min_t = min(min_t, t);
  max_t = max(max_t, t);
}

void benchmark_stl_sort(int& sum_t, int& min_t, int& max_t) {
  mark_time();
  sort_suf.init(s, N + 1);
  sort_suf.build();
  int t = report_time("Construcție cu STL sort");
  sort_suf.verify();
  sum_t += t;
  min_t = min(min_t, t);
  max_t = max(max_t, t);
}

void benchmark_ternary_qsort(int& sum_t, int& min_t, int& max_t) {
  mark_time();
  build_with_ternary_qsort();
  int t = report_time("Construcție cu ternary qsort");
  sum_t += t;
  min_t = min(min_t, t);
  max_t = max(max_t, t);

  for (int i = 0; i < N - 1; i++) {
    assert(strcmp(s + p[i], s + p[i + 1]) < 0);
  }
}

int main() {
  init_rng();

  int sum1 = 0, min1 = INFINITY, max1 = 0; // milisecunde
  int sum2 = 0, min2 = INFINITY, max2 = 0;
  int sum3 = 0, min3 = INFINITY, max3 = 0;

  for (int trial = 1; trial <= TRIALS; trial++) {
    printf("Testul %d/%d\n", trial, TRIALS);

    gen_string();
    benchmark_radix_sort(sum1, min1, max1);
    benchmark_stl_sort(sum2, min2, max2);
    benchmark_ternary_qsort(sum3, min3, max3);
  }

  sum1 -= min1 + max1; // Ignoră extremele.
  sum2 -= min2 + max2;
  sum3 -= min3 + max3;
  printf("Timp mediu radix sort: %d ms\n", sum1 / (TRIALS - 2));
  printf("Timp mediu STL sort: %d ms\n", sum2 / (TRIALS - 2));
  printf("Timp mediu ternary qsort: %d ms\n", sum3 / (TRIALS - 2));

  return 0;
}
