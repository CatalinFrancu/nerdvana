// Time complexity: O((meetings + pauses) * log max_time).
//
// Nomenclature: a pause is a (split at l, skip at c, split at r) triplet.
//
// Method: Notice that a skip will remove a slice between the closest previous
// splits to the left and right of the skip. Therefore, preprocess skips and
// splits and transform them to pauses. Notice that skips are either disjoint
// or contained in one another.
//
// We will consider meetings in decreasing order of their starting time. For a
// meeting starting at time t we wish to compute, in O(log n) time, the number
// of time units removed by all the pauses whose skip occurs after time t.
//
// We can compute the answer for every end time. The answers will look like a
// non-decreasing function from t to max_time. When advancing from t to t-1 we
// will need to update the function. This can be done using a range-range
// Fenwick tree.
//
// Taking all the corner cases into account is rather tedious. :-/ First, what
// does the function look for one pause (l, c, r)?
//
// 0 0 0 0 0 0 (c-l) (c-l+1) ... (r-l) (r-l) (r-l)...
//      ^     ^                       ^
//      l     c                       r
//
// How about for a meeting starting at some moment t between l and r?
//
// 0 0 0 0 0 0 (c-t) (c-t+1) ... (r-t) (r-t) (r-t)...
//      ^   ^ ^                       ^
//      l   t c                       r
//
// If t >= c, then this pause will not contribute anything to the
// meeting. Therefore, we will only add the pause to the Fenwick tree when we
// cross its skip time, c, and we will keep adding 1 at position c until we
// cross its left split, l.
//
// Second, note that if two pauses p1 and p2 have p2.c < p1.c and p2.r > p1.r,
// then we will first add p1 to the Fenwick tree, then p2. When this happens,
// p1 can be removed from the tree and forgotten. It will never contribute
// anything to meetings starting before p2.c. Essentially, p2 „takes over” p1.
//
// Third, when p2.c < p1.c and p2.r < p1.r, p2 does contribute to the range
// (p2.c, p1.c), after which p1 takes over.
//
// Thus, we will store a collection of pauses in decreasing order of their end
// time. New pauses will remove from the collection pauses they contain. This
// is not a stack per se. Some of the pauses are complete (we crossed their
// left split) and some are ongoing. We keep track of the ongoing ones because
// they need to be extended when advancing from time t to t-1.
#include <algorithm>
#include <stdio.h>

#define MAX_MEETINGS 500000
#define MAX_PAUSES 500000
#define MAX_TIME 1000000
#define MAX_INTERVAL (1 << 19) // for fenwick_bin_search

#define NONE -1
#define T_SPLIT 1

typedef struct {
  int x, y;
} meeting;

typedef struct {
  int l, r, c; // l < c < r; splits at l and r, skip at c
  int parent;  // for stack management during the time walk
} pause;

int f1[MAX_TIME + 1], f2[MAX_TIME + 1]; // Fenwick trees

meeting m[MAX_MEETINGS];
pause p[MAX_PAUSES];
int skip[MAX_PAUSES]; // points to an index in p[]
int s[MAX_PAUSES];  // active and completed pauses
int nm, np, ns; // number of meetings, pauses/skips and stack size

// adjustments during the time walk
int num_active, top_active;

///////////////////////////////////////////////////////////////////////////
//                Fenwick level 1 functions (point update)               //
///////////////////////////////////////////////////////////////////////////

void fenwick_clear(int* v) {
  for (int i = 1; i <= MAX_TIME; i++) {
    v[i] = 0;
  }
}

// addition: v[pos] += val
void fenwick_add(int* v, int pos, int val) {
  do {
    v[pos] += val;
    pos += pos & -pos;
  } while (pos <= MAX_TIME);
}

// partial sum: returns the sum of v[1...pos]
int fenwick_sum(int* v, int pos) {
  int s = 0;
  while (pos) {
    s += v[pos];
    pos &= pos - 1;
  }
  return s;
}

// point value: returns v[pos] in amortized O(1) time
int fenwick_at(int* v, int pos) {
  int s = v[pos];
  int parent = pos & (pos - 1);
  pos--;
  while (pos != parent) {
    s -= v[pos];
    pos &= pos - 1;
  }
  return s;
}

// binary search: returns the largest position such that v[pos] < val
int fenwick_bin_search(int* v, int val) {
  int pos = 0;

  for (int interval = MAX_INTERVAL; interval; interval >>= 1) {
    if ((pos + interval <= MAX_TIME) && (v[pos + interval] < val)) {
      val -= v[pos + interval];
      pos += interval;
    }
  }

  return pos;
}

///////////////////////////////////////////////////////////////////////////
//                Fenwick level 2 functions (range update)               //
///////////////////////////////////////////////////////////////////////////

// addition: v[l...r] += val
void fenwick_add2(int l, int r, int val) {
  fenwick_add(f1, l, -val * (l - 1));
  fenwick_add(f1, r + 1, val * r);
  fenwick_add(f2, l, val);
  fenwick_add(f2, r + 1, -val);
}

// partial sum: returns the sum of v[1...pos]
long long fenwick_sum2(int pos) {
  return (long long)fenwick_sum(f2, pos) * pos + fenwick_sum(f1, pos);
}

///////////////////////////////////////////////////////////////////////////
//                               main code                               //
///////////////////////////////////////////////////////////////////////////

int max(int x, int y) {
  return (x > y) ? x : y;
}

// sorts meeting starts, pause starts and skips in reverse chronological order
void sort_data() {
  std::sort(m, m + nm, [](meeting a, meeting b) {
    return a.x > b.x;
  });

  std::sort(p, p + np, [](pause a, pause b) {
    return (a.l > b.l) || (a.l == b.l && a.r < b.r);
  });

  std::sort(skip, skip + np, [](int a, int b) {
    return p[a].c > p[b].c;
  });
}

// returns true iff pause a contains pause b
bool contains(int a, int b) {
  return (p[a].l <= p[b].l) && (p[a].r >= p[b].r);
}

// extends the top active pause by one unit
void extend_top_active() {
  if (top_active != NONE) {
    fenwick_add2(p[top_active].c, p[top_active].c, 1);
    fenwick_add2(MAX_TIME, MAX_TIME, -1);
  }
}

// adds/removes a pause from the Fenwick tree, given its initial value (at the
// skip point) and subsequent unit values
void plot_pause(int k, int initial, int unit) {
  // position where we nullify the sum because our parent takes over
  int end = (p[k].parent == NONE) ? MAX_TIME : p[p[k].parent].c;

  // position where we add the last 1 value: the end of the pause, unless our
  // parent takes over
  int last = ((p[k].r < end) ? p[k].r : end) - 1;

  fenwick_add2(p[k].c, p[k].c, initial);
  fenwick_add2(p[k].c, last, unit);
  fenwick_add2(end, end, -initial - (last - p[k].c + 1) * unit);
}

// when we have just crossed the skip point, pause #k contributes 1 unit at
// every position
void add_pause(int k) {
  plot_pause(k, 0, +1);
}

void remove_pause(int k, int time) {
  // the pause was extended for a while since it was added, hence the -initial
  // value we subtract
  int initial = p[k].c - max(time, p[k].l);
  plot_pause(k, -initial, -1);
}

// Pause #k may already have been evicted from the stack. If not, then
// it is the top active pause.
void deactivate_pause(int k) {
  if (top_active == k) {
    top_active = p[k].parent;
    num_active--;
  }
}

// pause #k pops nested pauses from the stack, then becomes the top active
// pause
void stack_trim(int k) {
  while (ns && contains(k, s[ns - 1])) {
    int top = s[--ns];
    remove_pause(top, p[k].c);
    deactivate_pause(top);
  }

  p[k].parent = top_active;
  add_pause(k);

  s[ns++] = k;
  top_active = k;
  num_active++;
}

// Goes backwards in time. At each moment t processes meetings and pauses
// starting at t and skips. Returns the number of time units remaining.
long long time_walk() {
  long long result = 0;
  int i = 0, j = 0, k = 0; // meeting, pause and skip pointers
  top_active = NONE;

  for (int t = MAX_TIME; t >= 1; t--) {
    extend_top_active();

    // Process meetings. It is safe to take the sum from 1 to y because the
    // Fenwick tree is empty before time t.
    while ((i < nm) && (m[i].x == t)) {
      result += (m[i].y - m[i].x) - fenwick_sum2(m[i].y - 1);
      i++;
    }

    // process pause starts
    while ((j < np) && (p[j].l == t)) {
      deactivate_pause(j++);
    }

    // process the skip, if there is one
    if ((k < np) && (p[skip[k]].c == t)) {
      stack_trim(skip[k++]);
    }
  }

  return result;
}

int main() {

  int num_ops;

  // read meetings
  scanf("%d %d", &nm, &num_ops);
  for (int i = 0; i < nm; i++) {
    scanf("%d %d", &m[i].x, &m[i].y);
  }

  // read splits and skips and convert them to pauses
  // f1 marks splits; f2 marks skips
  while (num_ops--) {
    int op, t;
    scanf("%d %d\n", &op, &t);
    if (op == T_SPLIT) {
      fenwick_add(f1, t, 1);
    } else { // T_SKIP
      if (!fenwick_at(f1, t) && !fenwick_at(f2, t)) {
         // no skips nor splits here yet
        fenwick_add(f2, t, 1);
        int sum = fenwick_sum(f1, t);
        p[np].l = 1 + fenwick_bin_search(f1, sum);
        p[np].r = 1 + fenwick_bin_search(f1, sum + 1);
        p[np].c = t;
        skip[np] = np;
        np++;
      }
    }
  }

  fenwick_clear(f1);
  fenwick_clear(f2);

  sort_data();
  printf("%lld\n", time_walk());

  return 0;
}
