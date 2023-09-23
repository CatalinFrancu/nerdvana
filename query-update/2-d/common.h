// Common code for benchmarking of query-update problems.
#include <stdio.h>
#include <sys/time.h>

const int MAX_N = 1'024;
const int MAX_Q = 500'000;

const int OP_UPDATE = 1;
const int OP_SUM = 2;

typedef struct {
  int t, row1, col1, row2, col2, val;
} query;

typedef long long matrix[MAX_N + 1][MAX_N + 1];

matrix mat;
query q[MAX_Q];
long long answer[MAX_Q];
int n, num_queries, num_answers;
long long time1;

//////////////// input/output

void read_data() {
  scanf("%d %d ", &n, &num_queries);
  for (int row = 1; row <= n; row++) {
    for (int col = 1; col <= n; col++) {
      scanf("%lld", &mat[row][col]);
    }
  }

  for (int i = 0; i < num_queries; i++) {
    scanf("%d", &q[i].t);
    if (q[i].t == OP_UPDATE) {
      scanf("%d %d %d", &q[i].row1, &q[i].col1, &q[i].val);
    } else {
      scanf("%d %d %d %d", &q[i].row1, &q[i].col1, &q[i].row2, &q[i].col2);
    }
  }
}

void write_data() {
  for (int i = 0; i < num_answers; i++) {
    printf("%lld\n", answer[i]);
  }
}

//////////////// various tools

long long matrix_sum(matrix a, int row1, int col1, int row2, int col2) {
  long long sum = 0;
  for (int r = row1; r < row2; r++) {
    for (int c = col1; c < col2; c++) {
      sum += a[r][c];
    }
  }
  return sum;
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
