// Use the meet in the middle technique. Take the first L switches (L=18 works
// best in practice) and compute the 2^L subsets. Flipping a subset S results
// some lightbulbs being on (a bitset B). Store these results in a hash map
// mapping B to |S|.
//
// For the remaining R=s-L switches, compute the power set again. Store the
// pairs <B,|S|> in an array for every subset S and resulting bitset B.
//
// Now for a given day whose bitset is A, we wish to find all the combinations
// of one bitset B from the left hash map and one bitset C from the right
// array such that A^C=B. We do this by iterating the right array. From all
// the possible answers we choose the one that flips the fewest switches.
//
// This approach is slow. It uses STL hash maps whose keys are bitsets.
// Fiddling with the value of L may give some improvements.
#include <bitset>
#include <stdio.h>
#include <unordered_map>

const int MAX_BULBS = 1'000;
const int MAX_S = 30;
const int MAX_DAYS = 1'000;
const int FRAC_LEFT = 18;

typedef std::bitset<MAX_BULBS> bulbset;

struct ext_bulbset {
  bulbset bs;
  char touched;
};

bulbset ctrl[MAX_S];
bulbset day[MAX_DAYS];
ext_bulbset rpowerset[1 << (MAX_S - FRAC_LEFT)];
std::unordered_map<bulbset, char> lmap;
int n, s, num_days, left, right;

int calc_left() {
  return s * FRAC_LEFT / MAX_S;
}

void read_bitset(bulbset& bs) {
  int k, bulb;
  scanf("%d", &k);
  while (k--) {
    scanf("%d", &bulb);
    bulb--;
    bs.set(bulb);
  }
}

void read_data() {
  scanf("%d %d %d", &n, &s, &num_days);
  for (int i = 0; i < s; i++) {
    read_bitset(ctrl[i]);
  }
  for (int i = 0; i < num_days; i++) {
    read_bitset(day[i]);
  }
  left = calc_left();
  right = 1 << (s - left);
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void rec_left(int level, char touched, bulbset& bs) {
  if (level == left) {
    if (lmap.contains(bs)) {
      lmap[bs] = min(lmap[bs], touched);
    } else {
      lmap[bs] = touched;
    }
  } else {
    rec_left(level + 1, touched, bs);
    bs ^= ctrl[level];
    rec_left(level + 1, touched + 1, bs);
    bs ^= ctrl[level];
  }
}

void create_left_hash_map() {
  bulbset bs;
  rec_left(0, 0, bs);
}

void rec_right(int level, char touched, bulbset& bs) {
  static int pos = 0;

  if (level == s) {
    rpowerset[pos++] = { bs, touched };
  } else {
    rec_right(level + 1, touched, bs);
    bs ^= ctrl[level];
    rec_right(level + 1, touched + 1, bs);
    bs ^= ctrl[level];
  }
}

void create_right_hash_map() {
  bulbset bs;
  rec_right(left, 0, bs);
}

void answer_queries() {
  bulbset bs;

  for (int d = 0; d < num_days; d++) {
    int min_touched = s + 1;
    for (int p = 0; p < right; p++) {
      bs = day[d] ^ rpowerset[p].bs;
      auto it = lmap.find(bs);
      if (it != lmap.end()) {
        min_touched = min(min_touched, it->second + rpowerset[p].touched);
      }
    }
    printf("%d\n", (min_touched == s + 1) ? -1 : min_touched);
  }
}

int main() {
  read_data();
  create_left_hash_map();
  create_right_hash_map();
  answer_queries();

  return 0;
}
