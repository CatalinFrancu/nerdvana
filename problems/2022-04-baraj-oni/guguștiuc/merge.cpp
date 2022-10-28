// This is BROKEN! I never managed to work out all the kinks.
#include <algorithm>
#include <stdio.h>

#define MAX_MEETINGS 500000
#define MAX_SKIPS 500000
#define MAX_TIME 1000000

#define T_SPLIT 1
#define T_SKIP 2

// events occurring at the same time MUST be processed in this order
#define EVT_MEETING_END    1
#define EVT_SPLIT_END      2
#define EVT_SKIP           3
#define EVT_SPLIT_START    4
#define EVT_MEETING_START  5

typedef struct {
  int x, y;
} range;

typedef struct {
  int l, r, c; // l < c < r; splits at l and r, cut at c
} skip;

typedef struct {
  int type, time, payload;
  // note: to save space, the time could be deduced from the type & payload
} event;

int f1[MAX_TIME + 1], f2[MAX_TIME + 1], f3[MAX_TIME + 1]; // Fenwick trees
int fen_size;

range m[MAX_MEETINGS];                     // meetings
skip s[MAX_SKIPS];                         // skips
event e[2 * MAX_MEETINGS + 3 * MAX_SKIPS]; // events
int st[MAX_SKIPS];                         // skip stack
int nm, ns, ne, ss; // number of meetings, skips, events and stack size

///////////////////////////////////////////////////////////////////////////
//                Fenwick level 1 functions (point update)               //
///////////////////////////////////////////////////////////////////////////

void fenwick_clear(int* v) {
  for (int i = 1; i <= fen_size; i++) {
    v[i] = 0;
  }
}

// addition: v[pos] += val
void fenwick_add(int* v, int pos, int val) {
  do {
    v[pos] += val;
    pos += pos & -pos;
  } while (pos <= fen_size);
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

// range sum: returns the sum of v[l...r]
int fenwick_range_sum(int* v, int l, int r) {
  return fenwick_sum(v, r) - fenwick_sum(v, l - 1);
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
  int pos = 0, interval = 1;
  while ((interval << 1) < fen_size) {
    interval <<= 1;
  }
  while (interval) {
    if ((pos + interval <= fen_size) && (v[pos + interval] < val)) {
      val -= v[pos + interval];
      pos += interval;
    }
    interval >>= 1;
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

// range sum: returns the sum of v[l...r]
long long fenwick_range_sum2(int l, int r) {
  return fenwick_sum2(r) - fenwick_sum2(l - 1);
}

///////////////////////////////////////////////////////////////////////////
//                               main code                               //
///////////////////////////////////////////////////////////////////////////

bool evt_cmp(event a, event b) {
  if (a.time != b.time) {
    return a.time < b.time;
  }
  if (a.type != b.type) {
    return a.type < b.type;
  }
  // Same time, same type. We don't care about meetings and there can only be
  // one skip. That leaves splits to handle.
  if (a.type != EVT_SPLIT_START && a.type != EVT_SPLIT_END) {
    return true;
  }

  // make sure they open and close in proper parentheses order
  skip sa = s[a.payload], sb = s[b.payload];
  if (a.type == EVT_SPLIT_START) {
    return (sa.r > sb.r) ||
      ((sa.r == sb.r) && (sa.c < sb.c));
  } else {
    return (sa.l > sb.l) ||
      ((sa.r == sb.r) && (sa.c > sb.c));
  }
}

// create events for key moments and sort them in chronological order
void make_events() {
  for (int i = 0; i < nm; i++) {
    e[ne++] = { EVT_MEETING_START, m[i].x, i };
    e[ne++] = { EVT_MEETING_END, m[i].y, i };
  }

  for (int i = 0; i < ns; i++) {
    e[ne++] = { EVT_SPLIT_START, s[i].l, i };
    e[ne++] = { EVT_SKIP, s[i].c, i };
    e[ne++] = { EVT_SPLIT_END, s[i].r, i };
  }

  std::sort(e, e + ne, [](event a, event b) {
    return (a.time < b.time) || ((a.time == b.time) && (a.type < b.type));
  });
}

// returns true iff skip a contains skip b
bool contains(int a, int b) {
  return (s[a].l <= s[b].l) && (s[a].r >= s[b].r);
}

// Returns the number of time units lost due to the skip at position k on the
// stack.
long long skip_cost(int k) {
  skip me = s[st[k]];
  long long result = fenwick_range_sum2(me.l, me.c - 1);

  if (k) {
    // don't count meetings that were ongoing during the previous skip
    skip before = s[st[k - 1]];
    result -= (long long)fenwick_sum(f3, before.c) * (me.c - me.l);
  }

  printf("Skip cost of stack pos %d (skip %d) is %lld\n", k, st[k], result);

  return result;
}

// Ensures that skip i is no longer on the stack. If it is, it must
// necessarily be the top element. This will have a negative cost.
long long ensure_gone(int i) {
  if (ss && st[ss - 1] == i) {
    printf("split %d found on top of stack\n", i);
    ss--;
  }
  return 0;
}

// Processees a skip. Returns the number of time units removed by this skip.
long long process_skip(int k) {
  int result = 0;

  // This skip takes over skips it contains.
  while (ss && contains(k, st[ss - 1])) {
    long long payback = skip_cost(--ss);
    printf("Removed skip %d from stack, paid back %lld\n", st[ss], payback);
    result -= payback;
  }

  st[ss++] = k;
  long long contrib = skip_cost(ss - 1);
  result += contrib;
  printf("Pushing skip %d on stack, contribution %lld\n", k, contrib);

  return result;
}

// Scans the event list and returns the number of time units skipped.
// f1 and f2 store a range-range Fenwick tree (+1 per meeting per time unit spanned)
// f3[t] stores the number of ongoing meetings at time t
long long scan_events() {
  long long result = 0;
  range r;

  for (int i = 0; i < ne; i++) {
    printf("*** Event %d: type %d time %d payload %d\n", i, e[i].type, e[i].time, e[i].payload);
    switch (e[i].type) {
      case EVT_MEETING_START:
        printf("  meeting start, adding to Fenwick tree\n");
        r = m[e[i].payload];
        fenwick_add2(r.x, r.y - 1, +1);
        fenwick_add(f3, r.x, +1);
        break;

      case EVT_MEETING_END:
        printf("  meeting end, subtracting from Fenwick tree\n");
        r = m[e[i].payload];
        fenwick_add2(r.x, r.y - 1, -1);
        fenwick_add(f3, r.x, -1);
        break;

      case EVT_SPLIT_START:
        printf("  split start, nothing to do\n");
        break;

      case EVT_SPLIT_END:
        printf("  split end\n");
        result += ensure_gone(e[i].payload);
        break;

      case EVT_SKIP:
        printf("  SKIP!\n");
        result += process_skip(e[i].payload);
        break;
    }
    printf("  result after event: %lld\n", result);
  }

  return result;
}

inline int flip(int x) {
  return (1 + fen_size) - x;
}

// mirror all the data; clear the stack
void mirror() {
  for (int i = 0; i < nm; i++) {
    int tmp = m[i].x;
    m[i].x = flip(m[i].y);
    m[i].y = flip(tmp);
  }

  for (int i = 0; i < ns; i++) {
    int tmp = s[i].l;
    s[i].l = flip(s[i].r);
    s[i].r = flip(tmp);
    s[i].c = flip(s[i].c);
  }

  // reverse events and fix types
  for (int i = 0, j = ne - 1; i < j; i++, j--) {
    event tmp = e[i];
    e[i] = e[j];
    e[j] = tmp;
  }
  for (int i = 0; i < ne; i++) {
    switch (e[i].type) {
      case EVT_MEETING_END:   e[i].type = EVT_MEETING_START; break;
      case EVT_SPLIT_END:     e[i].type = EVT_SPLIT_START; break;
      case EVT_SKIP:          e[i].type = EVT_SKIP; break;
      case EVT_SPLIT_START:   e[i].type = EVT_SPLIT_END; break;
      case EVT_MEETING_START: e[i].type = EVT_MEETING_END; break;
    }
  }
  ss = 0;
}

int main() {

  int num_queries;
  long long result = 0;

  // read meetings
  scanf("%d %d", &nm, &num_queries);
  for (int i = 0; i < nm; i++) {
    scanf("%d %d", &m[i].x, &m[i].y);
    result += m[i].y - m[i].x;
  }

  printf("Initial length: %lld\n", result);

  // read splits and skips and convert to (l, r, c) format
  // f1 stores splits; f2 stores skips
  fen_size = MAX_TIME;
  fenwick_add(f1, MAX_TIME, 1);
  while (num_queries--) {
    int op, t;
    scanf("%d %d\n", &op, &t);
    if (op == T_SPLIT) {
      if (!fenwick_at(f1, t)) {
        // don't allow values to increase past 1
        fenwick_add(f1, t, 1);
      }
    } else { // T_SKIP
      if (!fenwick_at(f1, t) && !fenwick_at(f2, t)) {
         // no skips nor splits here yet
        fenwick_add(f2, t, 1);
        int sum = fenwick_sum(f1, t);
        s[ns].l = 1 + fenwick_bin_search(f1, sum);
        s[ns].r = 1 + fenwick_bin_search(f1, sum + 1);
        s[ns].c = t;
        printf("Skip %d: l=%d r=%d c=%d\n", ns, s[ns].l, s[ns].r, s[ns].c);
        ns++;
      }
    }
  }

  fenwick_clear(f1);
  fenwick_clear(f2);

  make_events();
  result -= scan_events();
  mirror();
  printf("FLIPPED!\n");
  result -= scan_events();

  printf("%lld\n", result);

  return 0;
}

// TODOs / ideas
// * limit fen_size to the actual max coord OR replace it in the code
// * subtract the result from the sum of all meetings
// * no need for the third Fenwick tree
// * use some split/skip nomenclature consistently
