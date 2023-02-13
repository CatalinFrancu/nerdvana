// Common code for benchmarking of query-update problems.
#include <stdio.h>
#include <sys/time.h>

// some of these are specific to one source; keep them together for consistency
#define MAX_BUCKETS 1024
#define P2_BUCKET_SIZE 512
#define P2_BUCKET_BITS 9
#define MAX_N 0x80000  // 2^19, power of two for segment trees
#define MAX_Q 500'000

#define T_UPDATE 1
#define T_SUM 2

typedef struct {
  int t, x, y;
} query;

long long v[MAX_N + 1];
query q[MAX_Q];
long long answer[MAX_Q];
int n, num_queries, num_answers;
long long time1;

//////////////// input/output

void read_data() {
  scanf("%d %d ", &n, &num_queries);
  for (int i = 1; i <= n; i++) {
    scanf("%lld", &v[i]);
  }

  for (int i = 0; i < num_queries; i++) {
    scanf("%d %d %d", &q[i].t, &q[i].x, &q[i].y);
  }
}

void write_data() {
  for (int i = 0; i < num_answers; i++) {
    printf("%lld\n", answer[i]);
  }
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
  fprintf(stderr, "%s: %lld milliseconds\n", msg, time2 - time1);
}
