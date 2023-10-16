// Common code for benchmarking of query-update problems.
#include <stdio.h>
#include <sys/time.h>

// some of these are specific to one source; keep them together for consistency
const int MAX_N = 0x80000;  // 2^19, power of two for segment trees
const int MAX_Q = 500'000;

const int OP_UPDATE = 1;
const int OP_SEARCH = 2;

typedef struct {
  int type, pos, val;
} query;

int v[MAX_N + 1];
query q[MAX_Q];
int answer[MAX_Q];
int n, num_queries, num_answers;
long long time1;

//////////////// input/output

void read_data() {
  scanf("%d %d ", &n, &num_queries);
  for (int i = 1; i <= n; i++) {
    scanf("%d", &v[i]);
  }

  for (int i = 0; i < num_queries; i++) {
    scanf("%d", &q[i].type);
    if (q[i].type == OP_UPDATE) {
      scanf("%d %d", &q[i].pos, &q[i].val);
    } else {
      scanf("%d", &q[i].val);
    }
  }
}

void write_data() {
  for (int i = 0; i < num_answers; i++) {
    printf("%d\n", answer[i]);
  }
}

//////////////// various tools

void copy_array(int* dest, int* src, int count) {
  for (int i = 0; i < count; i++) {
    dest[i] = src[i];
  }
}

int next_power_of_2(int n) {
  while (n & (n - 1)) {
    n += n & -n;
  }

  return n;
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
