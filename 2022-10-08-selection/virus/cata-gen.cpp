// Test generator for Virus
// Author: Cătălin Frâncu
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define DEBUG false

#define MIN_N 3
#define MAX_N 2'000
#define MIN_Q 1
#define MAX_Q 100'000

#define MAX_BORDER 3
#define ALIVE_CHANCE 98 // out of 100

#define T_SPLIT 1
#define T_CONN 2

typedef struct {
  int type, r1, c1, r2, c2;
} query; // also used for rectangles

query r[MAX_Q]; // rectangles available for splitting
query q[MAX_Q];
bool dead[MAX_N + 1][MAX_N + 1];
int n, num_r, num_q, num_dead;

void debug(const char* format, ...) {
  if (DEBUG) {
    va_list args;
    va_start(args, format);

    vprintf(format, args);

    va_end(args);
  }
}

void usage() {
  fprintf(stderr, "Usage: cata-gen <n> <i> <c>\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Generates an array of i infections and c connectivity queries.\n");
  _exit(1);
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

int rand2(int x, int y) {
  return rand() % (y - x + 1) + x;
}

void mirror_rows(query* q, query outer) {
  q->r1 = outer.r1 + outer.r2 - q->r1;
  q->r2 = outer.r1 + outer.r2 - q->r2;
  int tmp = q->r1; q->r1 = q->r2; q->r2 = tmp;
}

void mirror_columns(query* q, query outer) {
  q->c1 = outer.c1 + outer.c2 - q->c1;
  q->c2 = outer.c1 + outer.c2 - q->c2;
  int tmp = q->c1; q->c1 = q->c2; q->c2 = tmp;
}

// add a rectangle to the list of split targets, if it is large enough
void maybe_push(query q) {
  debug("Considering (%d,%d)-(%d,%d)... ", q.r1, q.c1, q.r2, q.c2);
  if ((q.r2 > q.r1) && (q.c2 > q.c1)) {
    r[num_r++] = q;
    debug("ok\n");
  } else {
    debug("not ok\n");
  }
}

void kill(int r, int c) {
  assert(!dead[r][c]);
  dead[r][c] = true;
  num_dead++;
}

// paint the rectangle in the matrix and test for intersections
void paint(query q) {
  for (int i = q.r1; i <= q.r2; i++) {
    kill(i, q.c1);
    kill(i, q.c2);
  }
  for (int i = q.c1 + 1; i < q.c2; i++) {
    kill(q.r1, i);
    kill(q.r2, i);
  }
  assert((1 <= q.r1) && (q.r1 < q.r2) && (q.r2 <= n));
  assert((1 <= q.c1) && (q.c1 < q.c2) && (q.c2 <= n));
}

void infect() {
  // choose a rectangle to split
  int pos = rand() % num_r;
  query f = r[pos];
  r[pos] = r[--num_r];
  debug("**** Splitting rectangle (%d,%d)-(%d,%d)\n", f.r1, f.c1, f.r2, f.c2);

  int outer_h = f.r2 - f.r1 + 1, outer_w = f.c2 - f.c1 + 1;

  // choose border sizes
  int max_border_h = min(outer_h - 2, MAX_BORDER);
  int max_border_w = min(outer_w - 2, MAX_BORDER);
  int border_h = rand2(0, max_border_h);
  int border_w = rand2(0, max_border_w);
  debug("Chose borders %dx%d\n", border_h, border_w);

  // choose inner rectangle sizes
  int inner_h = rand2(2, outer_h - border_h);
  int inner_w = rand2(2, outer_w - border_w);
  debug("Chose size %dx%d\n", inner_h, inner_w);

  // rectangle coordinates
  query inner = {
    T_SPLIT,
    f.r1 + border_h,
    f.c1 + border_w,
    f.r1 + border_h + inner_h - 1,
    f.c1 + border_w + inner_w - 1,
  };
  debug("Chose inner (%d,%d)-(%d,%d)\n", inner.r1, inner.c1, inner.r2, inner.c2);

  query right, bottom;
  if (rand() % 2 == 0) {
    // extend line horizontally
    right = { 0, f.r1, inner.c2 + 1, inner.r2, f.c2 };
    bottom = { 0, inner.r2 + 1, f.c1, f.r2, f.c2 };
  } else {
    // extend line vertically
    bottom = { 0, inner.r2 + 1, f.c1, f.r2, inner.c2 };
    right = { 0, f.r1, inner.c2 + 1, f.r2, f.c2 };
  }
  debug("Chose right (%d,%d)-(%d,%d)\n", right.r1, right.c1, right.r2, right.c2);
  debug("Chose bottom (%d,%d)-(%d,%d)\n", bottom.r1, bottom.c1, bottom.r2, bottom.c2);

  if (rand() % 2 == 0) {
    mirror_rows(&inner, f);
    mirror_rows(&bottom, f);
    mirror_rows(&right, f);
  }
  if (rand() % 2 == 0) {
    mirror_columns(&inner, f);
    mirror_columns(&bottom, f);
    mirror_columns(&right, f);
  }
  debug("Transformed inner (%d,%d)-(%d,%d)\n", inner.r1, inner.c1, inner.r2, inner.c2);
  debug("Transformed right (%d,%d)-(%d,%d)\n", right.r1, right.c1, right.r2, right.c2);
  debug("Transformed bottom (%d,%d)-(%d,%d)\n", bottom.r1, bottom.c1, bottom.r2, bottom.c2);

  // write down the split operation and paint it
  q[num_q++] = inner;
  paint(inner);

  // shrink the inner rectangle
  inner.r1++; inner.r2--; inner.c1++; inner.c2--;

  maybe_push(inner);
  maybe_push(right);
  maybe_push(bottom);
}

void pick_point(bool alive, int* r, int* c) {
  do {
    *r = rand2(1, n);
    *c = rand2(1, n);
  } while (dead[*r][*c] == alive);
}

query connectivity_query() {
  // pick two distinct points, each with an ALIVE_CHANCE
  query q;
  q.type = T_CONN;
  pick_point(rand() % 100 < ALIVE_CHANCE, &q.r1, &q.c1);
  do {
    pick_point(rand() % 100 < ALIVE_CHANCE, &q.r2, &q.c2);
  } while ((q.r1 == q.r2) && (q.c1 == q.c2));
  debug("**** Connectivity query: (%d,%d,alive=%d)-(%d,%d,alive=%d)\n",
        q.r1, q.c1, !dead[q.r1][q.c1],
        q.r2, q.c2, !dead[q.r2][q.c2]);
  return q;
}

int main(int argc, char** argv) {
  if (argc != 4) {
    usage();
  }

  n = atoi(argv[1]);
  int i = atoi(argv[2]);
  int c = atoi(argv[3]);

  // Sanity checks.
  assert((MIN_N <= n) && (n <= MAX_N));
  assert((MIN_Q <= i + c) && (i + c <= MAX_Q));

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  num_r = 1;
  r[0] = { 0, 1, 1, n, n };
  while (num_r && (num_q < i)) {
    infect();
  }

  debug("%d dead computers, %d alive\n", num_dead, n * n - num_dead);

  if (num_q < i) {
    fprintf(stderr, "Only found %d infections, test params %d %d %d\n",
            num_q, n, i, c);
  }

  while (num_q < i + c) {
    q[num_q++] = connectivity_query();
  }

  // shuffle
  for (int i = 1; i < num_q; i++) {
    int j = rand() % (i + 1);
    query tmp = q[j]; q[j] = q[i]; q[i] = tmp;
  }

  printf("%d %d\n", n, num_q);
  for (int i = 0; i < num_q; i++) {
    printf("%d %d %d %d %d\n", q[i].type, q[i].r1, q[i].c1, q[i].r2, q[i].c2);
  }

  return 0;
}
