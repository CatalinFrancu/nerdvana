// 1. Rank permutations to map them to the contiguous range [0...10!).
// 2. Run BFS to compute distances.
// 3. For a given query, first remap it so that it starts from abcdefghij.
//
// v2: Generate moves by swapping a single pair at a time.
// v3: Use our own popcount.
// v4: Replace leters with digits.
#include <stdio.h>

const int LENGTH = 10;
const int NUM_PERMS = 3'628'800;
const int INFINITY = 0xff;
const char START[LENGTH] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
const int FACT[LENGTH] = { 1, 1, 2, 6, 24, 120, 720, 5'040, 40'320, 362'880 };

struct string {
  char s[LENGTH];
  int rank;

  void set(const char* s, int rank) {
    for (int i = 0; i < LENGTH; i++) {
      this->s[i] = s[i];
    }
    this->rank = rank;
  }

  void flip(int l, int r) {
    while (l < r) {
      swap(l++, r--);
    }
  }

  void swap(int i, int j) {
    int tmp = s[i]; s[i] = s[j]; s[j] = tmp;
  }
};

string q[NUM_PERMS];
unsigned char dist[NUM_PERMS];
int pop[1 << LENGTH];

void precompute() {
  for (int i = 1; i < (1 << LENGTH); i++) {
    pop[i] = pop[i >> 1] + (i & 1);
  }
}

int rank(const char* s) {
  int result = 0, mask = 0;

  for (int i = 0; i < LENGTH; i++) {
    int bit = 1 << s[i];
    int cnt = pop[mask & (bit - 1)];
    mask |= bit;
    result += cnt * FACT[i];
  }

  return result;
}

void bfs() {
  for (int i = 0; i < NUM_PERMS; i++) {
    dist[i] = INFINITY;
  }

  int head = 0, tail = 0;
  int rank_start = rank(START);
  q[tail++].set(START, rank_start);
  dist[rank_start] = 0;

  while (head != tail) {
    string str = q[head++];

    for (int l = 1; l <= 2; l++) {
      for (int i = 0; i + l < LENGTH; i++) {
        int x = i, y = i + l;
        do {
          str.swap(x--, y++);
          int r2 = rank(str.s);
          if (dist[r2] == INFINITY) {
            dist[r2] = 1 + dist[str.rank];
            q[tail++].set(str.s, r2);
          }

        } while (x >= 0 && y < LENGTH);
        str.flip(x + 1, y - 1);
      }
    }
  }
}

// Apply the same transform to dest that would transform src into start.
void remap(char* src, char* dest) {
  for (int i = 0; i < LENGTH; i++) {
    src[i] -= 'a';
    dest[i] -= 'a';
  }  
  for (int i = 0; i < LENGTH; i++) {
    int j = 0;
    while (src[j] != dest[i]) {
      j++;
    }
    dest[i] = START[j];
  }
}

void process_queries() {
  char src[LENGTH + 1], dest[LENGTH + 1];
  while ((scanf("%s %s", src, dest) == 2) && (src[0] != '*')) {
    remap(src, dest);
    printf("%d\n", dist[rank(dest)]);
  }
}

int main() {
  precompute();
  bfs();
  process_queries();

  return 0;
}
