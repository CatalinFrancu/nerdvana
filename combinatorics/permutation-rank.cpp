// Permutation ranking and unranking using several methods. None of them are
// exceedingly fancy; we want them to be reasonably easy to code.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// We'll rank and unrank TRIALS random permutations of size N.
#define TRIALS 10'000'000
#define N 20
#define MAX_P2 16 // for fenwick_bin_search()

typedef unsigned long long u64;

typedef struct {
  u64 rank;
  char v[N];
} permutation;

permutation p[TRIALS];
u64 fact[N];
char kth[1 << N][N]; // kth[x][k] = the k-th UNSET bit in x (0-based)

int min(int x, int y) {
  return (x < y) ? x : y;
}

/****************************************************************
 *                       Time management                        *
 ****************************************************************/
long long __time1;

long long get_time() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return 1000ll * time.tv_sec + time.tv_usec / 1000;
}

void mark_time() {
  __time1 = get_time();
}

void report_time(const char* msg) {
  long long time2 = get_time();
  fprintf(stderr, "%s: %lld milliseconds\n", msg, time2 - __time1);
}

/****************************************************************
 *                       Naive method                           *
 ****************************************************************/

bool used[N];

u64 rank_naive(char* p) {
  u64 result = 0;
  for (int i = 0; i < N; i++) {
    // count the elements smaller than p[i]
    int cnt = 0;
    for (int j = 0; j < i; j++) {
      cnt += (p[j] < p[i]);
    }
    result += fact[N - 1 - i] * (p[i] - cnt);
  }
  return result;
}

void unrank_naive(u64 rank, char* witness) {
  for (int i = 0; i < N; i++) {
    used[i] = false;
  }
  for (int i = 0; i < N; i++) {
    int q = rank / fact[N - 1 - i];
    rank %= fact[N - 1 - i];
    // get the q-th unused value
    int j = 0;
    while (q || used[j]) {
      q -= !used[j++];
    }
    assert(j == witness[i]);
    used[j] = true;
  }
}

/****************************************************************
 *                       Fenwick trees                          *
 ****************************************************************/

// Reminder: Fenwick trees are indexed from 1.
int fen[N + 1];

void fenwick_clear() {
  for (int i = 1; i <= N; i++) {
    fen[i] = 0;
  }
}

// Sets all the values to 1, then transforms the array into a Fenwick tree.
// https://stackoverflow.com/a/31070683/6022817
void fenwick_set_1() {
  for (int i = 1; i <= N; i++) {
    fen[i] = 1;
  }
  for (int i = 1; i <= N; i++) {
    int j = i + (i & -i);
    if (j <= N) {
      fen[j] += fen[i];
    }
  }
}

void fenwick_add(int pos, int val) {
  do {
    fen[pos] += val;
    pos += pos & -pos;
  } while (pos <= N);
}

int fenwick_sum(int pos) {
  int s = 0;
  while (pos) {
    s += fen[pos];
    pos &= pos - 1;
  }
  return s;
}

// returns the largest position such that fen[pos] < val
int fenwick_bin_search(int val) {
  int pos = 0;

  for (int interval = MAX_P2; interval; interval >>= 1) {
    if ((pos + interval <= N) && (fen[pos + interval] < val)) {
      val -= fen[pos + interval];
      pos += interval;
    }
  }

  return pos;
}

u64 rank_fenwick(char* p) {
  fenwick_clear();
  u64 result = 0;
  for (int i = 0; i < N; i++) {
    // count the elements smaller than p[i] -- same as naive version
    int cnt = fenwick_sum(p[i] + 1);
    result += fact[N - 1 - i] * (p[i] - cnt);
    fenwick_add(p[i] + 1, 1);
  }
  return result;
}

void unrank_fenwick(u64 rank, char* witness) {
  // Fenwick values of 1 indicate still usable elements
  fenwick_set_1();
  for (int i = 0; i < N; i++) {
    int q = rank / fact[N - 1 - i];
    rank %= fact[N - 1 - i];
    // get the q-th usable value
    int j = fenwick_bin_search(q + 1);
    assert(j == witness[i]);
    fenwick_add(j + 1, -1);
  }
}

/****************************************************************
 *                           Popcount                           *
 ****************************************************************/

u64 rank_popcount(char* p) {
  u64 result = 0;
  int mask = 0;
  for (int i = 0; i < N; i++) {
    // count the number of set bits among bits [0...p[i])
    int cnt = __builtin_popcount(mask & ((1 << p[i]) - 1));
    result += fact[N - 1 - i] * (p[i] - cnt);
    mask |= (1 << p[i]);
  }
  return result;
}

void unrank_popcount(u64 rank, char* witness) {
  int mask = 0;
  for (int i = 0; i < N; i++) {
    int q = rank / fact[N - 1 - i];
    rank %= fact[N - 1 - i];
    // get the q-th unused value
    int j = kth[mask][q];
    assert(j == witness[i]);
    mask |= (1 << j);
  }
}

void unrank_popcount2(u64 rank, char* witness) {
  int mask = 0;
  for (int i = 0; i < N; i++) {
    int q = rank / fact[N - 1 - i];
    rank %= fact[N - 1 - i];

    // Get the q-th unused value. Same as above, but we check the lower half
    // of the mask, then the higher half. The point is that we only need 2^10
    // entries instead of 2^20, which runs much faster because of better
    // caching.
    int lo = mask & ((1 << (N >> 1)) - 1);
    int pop = (N >> 1) - __builtin_popcount(lo);
    int j = (q < pop)
      ? kth[lo][q]
      : ((N >> 1) + kth[mask >> (N >> 1)][q - pop]);

    assert(j == witness[i]);
    mask |= (1 << j);
  }
}

// Same as unrank_popcount2, but replaces divisions with repeated subtractions.
void unrank_popcount3(u64 rank, char* witness) {
  int mask = 0;
  for (int i = 0; i < N; i++) {
    int q = 0;
    while (fact[N - 1 - i] <= rank) {
      rank -= fact[N - 1 - i];
      q++;
    }

    int lo = mask & ((1 << (N >> 1)) - 1);
    int pop = (N >> 1) - __builtin_popcount(lo);
    int j = (q < pop)
      ? kth[lo][q]
      : ((N >> 1) + kth[mask >> (N >> 1)][q - pop]);

    assert(j == witness[i]);
    mask |= (1 << j);
  }
}

int main() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  // precompute factorials
  fact[0] = 1;
  for (int i = 1; i < N; i++) {
    fact[i] = fact[i - 1] * i;
  }

  // precompute k-th bit values
  mark_time();
  for (int i = 0; i < (1 << N); i++) {
    int cnt = 0;
    for (int k = 0; k < N; k++) {
      if (!(i & (1 << k))) {
        kth[i][cnt++] = k;
      }
    }
    // Values from cnt to N are meaning less because there aren't enough unset
    // bits in i. Either set them to a special value or make sure the code
    // doesn't read those values.
  }
  report_time("Precomputed k-th set bit values");

  // generate the trials
  mark_time();
  for (int i = 0; i < TRIALS; i++) {
    for (int j = 0; j < N; j++) {
      int k = rand() % (j + 1);
      p[i].v[j] = p[i].v[k];
      p[i].v[k] = j;
    }
    p[i].rank = rank_popcount(p[i].v);

    // for (int j = 0; j < N; j++) {
    //   printf("%d ", p[i].v[j]);
    // }
    // printf("rank %llu\n", p[i].rank);
  }
  report_time("Generated trials");
  printf("--\n");

  mark_time();
  for (int i = 0; i < TRIALS; i++) {
    assert(rank_naive(p[i].v) == p[i].rank);
  }
  report_time("Naive rank");

  mark_time();
  for (int i = 0; i < TRIALS; i++) {
    assert(rank_fenwick(p[i].v) == p[i].rank);
  }
  report_time("Fenwick rank");

  mark_time();
  for (int i = 0; i < TRIALS; i++) {
    assert(rank_popcount(p[i].v) == p[i].rank);
  }
  report_time("Popcount rank");
  printf("--\n");

  mark_time();
  for (int i = 0; i < TRIALS; i++) {
    unrank_naive(p[i].rank, p[i].v);
  }
  report_time("Naive unrank");

  mark_time();
  for (int i = 0; i < TRIALS; i++) {
    unrank_fenwick(p[i].rank, p[i].v);
  }
  report_time("Fenwick unrank");

  mark_time();
  for (int i = 0; i < TRIALS; i++) {
    unrank_popcount(p[i].rank, p[i].v);
  }
  report_time("Popcount unrank");

  mark_time();
  for (int i = 0; i < TRIALS; i++) {
    unrank_popcount2(p[i].rank, p[i].v);
  }
  report_time("Popcount2 unrank");

  mark_time();
  for (int i = 0; i < TRIALS; i++) {
    unrank_popcount3(p[i].rank, p[i].v);
  }
  report_time("Popcount3 unrank");

  return 0;
}
