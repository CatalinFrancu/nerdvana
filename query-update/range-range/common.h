// Common code for benchmarking of query-update problems.
#include <stdio.h>
#include <sys/time.h>

// some of these are specific to one source; keep them together for consistency
const int MAX_BUCKETS = 1024;
const int P2_BUCKET_SIZE = 512;
const int P2_BUCKET_BITS = 9;
const int MAX_N = 500'000;
const int MAX_SEGTREE_NODES = 1 << 20;
const int MAX_Q = 500'000;

const int OP_UPDATE = 1;
const int OP_QUERY = 2;

typedef struct {
  int t, l, r, val;
} query;

long long v[MAX_N];
query q[MAX_Q];
long long answer[MAX_Q];
int n, num_queries, num_answers;
long long time1;

//////////////// input/output

void read_data() {
  scanf("%d %d ", &n, &num_queries);
  for (int i = 0; i < n; i++) {
    scanf("%lld", &v[i]);
  }

  for (int i = 0; i < num_queries; i++) {
    scanf("%d %d %d", &q[i].t, &q[i].l, &q[i].r);
    if (q[i].t == OP_UPDATE) {
      scanf("%d", &q[i].val);
    }
  }
}

void write_data() {
  for (int i = 0; i < num_answers; i++) {
    printf("%lld\n", answer[i]);
  }
}

//////////////// various tools

long long array_sum(long long* v, int l, int r) {
  long long sum = 0;
  while (l < r) {
    sum += v[l++];
  }
  return sum;
}

void array_add(long long* v, int l, int r, int val) {
  while (l < r) {
    v[l++] += val;
  }
}

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

//////////////// time management

// Returns the current timestamp in milliseconds
long long get_time() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return 1000ll * time.tv_sec + time.tv_usec / 1000;
}

void mark_time() {
  time1 = get_time();
}

void report_time(const char* msg) {
  long long time2 = get_time();
  fprintf(stderr, "%-35s %4lld milliseconds\n", msg, time2 - time1);
}
