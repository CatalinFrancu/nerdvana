// In addition to v2,
//
// 1. Precompute the modular powers to get rid of modulo operations in
//    hash_code().
//
// 2. Minor speed optimization in iterating the array.
#include <stdio.h>

typedef unsigned long long u64;

const int MAX_BULBS = 1'024;
const int MAX_WORDS = MAX_BULBS / 64;
const int MAX_SWITCHES = 30;
const int FRAC_LEFT = 17;
const int HASH_BASE = 37;
const u64 HASH_MOD = 18'446'744'073'709'551'263ull;
const int HASH_MAP_SIZE = 1 << (FRAC_LEFT + 2); // 4x the number of values it will hold
const char NONE = -1;

u64 powers[MAX_BULBS];

struct bitset {
  u64 v[MAX_WORDS];
  static int size;

  void clear() {
    int last = (size - 1) / 64;
    for (int i = 0; i <= last; i++) {
      v[i] = 0;
    }
  }

  void toggle(int pos) {
    v[pos >> 6] ^= 1ll << (pos & 63);
  }

  void read() {
    int k, pos;
    scanf("%d", &k);
    while (k--) {
      scanf("%d", &pos);
      pos--;
      toggle(pos);
    }
  }

  u64 hash_code() {
    int last = (size - 1) / 64;
    u64 result = 0;
    int p = 0;
    for (int i = 0; i <= last; i++) {
      for (int b = 0; b < 64; b++) {
        result ^= powers[p] * ((v[i] >> b) & 1);
        p++;
      }
    }
    return result;
  }

  bitset& operator^=(const bitset& other) {
    for (int i = 0; i < MAX_WORDS; i++) {
      v[i] ^= other.v[i];
    }
    return *this;
  }

  friend bitset operator^(bitset a, const bitset& b) {
    a ^= b;
    return a;
  }
};

int bitset::size;

struct hash_map {
  u64 key[HASH_MAP_SIZE];
  char val[HASH_MAP_SIZE];

  void init() {
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
      val[i] = NONE;
    }
  }

  int find_pos(u64 k) {
    int pos = k % HASH_MAP_SIZE;
    while ((key[pos] != k) && (val[pos] != NONE)) {
      pos = (pos + 1) % HASH_MAP_SIZE;
    }
    return pos;
  }

  void minimize(u64 k, char v) {
    int pos = find_pos(k);
    if ((val[pos] == NONE) || (v < val[pos])) {
      key[pos] = k;
      val[pos] = v;
    }
  }

  char find(u64 k) {
    int pos = find_pos(k);
    return val[pos];
  }
};

struct state {
  u64 hash_code;
  char touched;
};

bitset sw[MAX_SWITCHES];
hash_map left_map;
state right_table[1 << (MAX_SWITCHES - FRAC_LEFT)];
int n, s, num_days, left, right;

void read_switches() {
  scanf("%d %d %d", &n, &s, &num_days);
  bitset::size = n;
  for (int i = 0; i < s; i++) {
    sw[i].read();
  }
  left = s * FRAC_LEFT / MAX_SWITCHES;
  right = s - left;
}

void compute_powers() {
  powers[0] = 1;
  for (int i = 1; i < n; i++) {
    powers[i] = (__int128)powers[i - 1] * HASH_BASE % HASH_MOD;
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void rec_left(int level, char touched, bitset& bs) {
  if (level == left) {
    left_map.minimize(bs.hash_code(), touched);
  } else {
    rec_left(level + 1, touched, bs);
    bs ^= sw[level];
    rec_left(level + 1, touched + 1, bs);
    bs ^= sw[level];
  }
}

void create_left_hash_map() {
  bitset bs;
  bs.clear();
  left_map.init();
  rec_left(0, 0, bs);
}

void rec_right(int level, char touched, bitset& bs) {
  static int pos = 0;

  if (level == s) {
    right_table[pos++] = { bs.hash_code(), touched };
  } else {
    rec_right(level + 1, touched, bs);
    bs ^= sw[level];
    rec_right(level + 1, touched + 1, bs);
    bs ^= sw[level];
  }
}

void create_right_table() {
  bitset bs;
  bs.clear();
  rec_right(left, 0, bs);
}

void answer_queries() {
  bitset day;

  for (int d = 0; d < num_days; d++) {
    day.clear();
    day.read();
    u64 hash_code = day.hash_code();
    int min_touched = s + 1;

    for (int p = 0; p < (1 << right); p++) {
      if (right_table[p].touched < min_touched) {
        // Only perform a lookup if we have a chance to improve.
        u64 desired = hash_code ^ right_table[p].hash_code;
        char left_touched = left_map.find(desired);
        if (left_touched != NONE) {
          min_touched = min(min_touched, left_touched + right_table[p].touched);
        }
      }
    }
    printf("%d\n", (min_touched == s + 1) ? -1 : min_touched);
  }
}

int main() {
  read_switches();
  compute_powers();
  create_left_hash_map();
  create_right_table();
  answer_queries();

  return 0;
}
