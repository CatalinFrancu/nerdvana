// Method: Meet in the middle.
//
// From every pair <c_{i}, c_{i+1}> we deduce that we must buy color i or
// color i + 1 (or both). Therefore, build an m×m boolean relations matrix
// where bit (i,j) is 1 if we need to buy color i or j. Include conditions
// <c_1, c_1> and <c_n, c_n> to always buy the first and last platforms. Store
// this matrix as m m-bit masks.
//
// Iterate through the left half of the bits and build all the valid masks. A
// valid left side mask is one that satisfies all relations between left side
// bits. Whenever we do NOT buy a bit x into the left side mask, make a note
// that we will have to buy all bits y from the right side with whom x has
// relations. Thus, whenever we complete a valid mask L we'll also have
// available a "must buy" mask R. All this is accomplished in left_half.rec().
//
// Let ~R be the set of right-side bits not included in R. Then the bits
// bought so far (L∪R) satisfy all relations:
//
// * between bits on the left side
// * between a bit on the left side and a bit on the right side
// * between bits in R
// * between a bit in R and a bit in ~R.
//
// The only relations we still need to satisfy are those between bits in ~R.
// Therefore, we need to prepare a table for the right-hand side to answer the
// following query: given a mask M, what is the cheapest subset of M that we
// need to buy to satisfy all the relations within M? This is accomplished in
// right_half.compute_red_costs().
#include <stdio.h>

const int MAX_COLORS = 40;
const int INFINITY = 1'000'000'000;

typedef unsigned long long u64;

u64 rel[MAX_COLORS];
int cost[MAX_COLORS];
u64 rhs_mask;
int num_colors, mid;

void add_relation(int a, int b) {
  rel[a] |= (1ull << b);
  rel[b] |= (1ull << a);
}

// Force relations <c[1],c[1]> and <c[n],c[n]> so that those colors are always
// chosen.
void read_data() {
  int n, prev, col;
  scanf("%d %d %d", &n, &num_colors, &prev);
  prev--;

  add_relation(prev, prev);
  for (int i = 1; i < n; i++) {
    scanf("%d", &col);
    col--;
    add_relation(prev, col);
    prev = col;
  }
  add_relation(prev, prev);

  for (int i = 0; i < num_colors; i++) {
    scanf("%d", &cost[i]);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

int log2(u64 mask) {
  return 63 - __builtin_clzll(mask);
}

// Returns a mask with 1's from msb to lsb inclusively.
u64 bit_range(int msb, int lsb) {
  return ((1ull << (msb + 1)) - 1) ^ ((1ull << lsb) - 1);
}

void precompute_info() {
  mid = num_colors / 2;
  rhs_mask = bit_range(mid - 1, 0);
}

struct right_half {
  // The reduced cost of a mask is the minimum cost needed to satisfy
  // relations between pairs of bits in the mask.
  int red_cost[1 << (MAX_COLORS / 2)];

  int get_mask_cost(u64 mask) {
    int sum = 0;
    while (mask) {
      int b = log2(mask);
      sum += cost[b];
      mask ^= (1 << b);
    }
    return sum;
  }

  void compute_red_costs() {
    for (u64 mask = 1; mask < (1ull << mid); mask++) {
      int k = log2(mask);

      // Option 1: buy bit k. Reduce the remaining bits.
      u64 remaining = mask ^ (1 << k);
      int c1 = cost[k] + red_cost[remaining];

      // Option 2: do not buy bit k. Necessarily buy all its relations. This
      // also handles the case where k has a relation to itself.
      u64 rel_mask = mask & rel[k];
      remaining = (mask ^ (1 << k)) & ~rel[k];
      int c2 = get_mask_cost(rel_mask) + red_cost[remaining];

      red_cost[mask] = min(c1, c2);
    }
  }

  int cheapest_expansion(u64 bought_mask) {
    u64 not_bought = rhs_mask ^ bought_mask;
    return get_mask_cost(bought_mask) + red_cost[not_bought];
  }
};

right_half right;

struct left_half {
  int min_cost;

  void rec(int k, int c, u64 left_mask, u64 right_mask) {
    if (k == num_colors) {
      int right_cost = right.cheapest_expansion(right_mask);
      min_cost = min(min_cost, c + right_cost);
    } else {
      // We must buy bit k if it has any unsatisfied relations in the LHS
      // where k is the high bit.
      u64 unsat = rel[k] & bit_range(k, mid) & ~left_mask;
      if (!unsat) {
        // Free to not buy bit k. Make note of k's relations in the RHS.
        u64 rhs_relations = rel[k] & rhs_mask;
        rec(k + 1, c, left_mask, right_mask | rhs_relations);
      }
      rec(k + 1, c + cost[k], left_mask | (1ull << k), right_mask);
    }
  }

  void generate_all_valid() {
    min_cost = INFINITY;
    rec(mid, 0, 0, 0);
  }
};

left_half left;

int main() {
  read_data();
  precompute_info();
  right.compute_red_costs();
  left.generate_all_valid();
  printf("%d\n", left.min_cost);

  return 0;
}
