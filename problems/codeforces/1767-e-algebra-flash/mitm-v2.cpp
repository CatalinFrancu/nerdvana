// In addition to v1, we precompute mask costs on the right-hand side to
// reduce the recurrence to O(1) per call. This turns out to run equally fast.
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
  int mask_cost[1 << (MAX_COLORS / 2)];
  int red_cost[1 << (MAX_COLORS / 2)];

  void compute_mask_costs() {
    for (u64 mask = 1; mask < (1ull << mid); mask++) {
      int k = log2(mask);
      mask_cost[mask] = cost[k] + mask_cost[mask ^ (1 << k)];
    }
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
      int c2 = mask_cost[rel_mask] + red_cost[remaining];

      red_cost[mask] = min(c1, c2);
    }
  }

  int cheapest_expansion(u64 bought_mask) {
    u64 not_bought = rhs_mask ^ bought_mask;
    return mask_cost[bought_mask] + red_cost[not_bought];
  }
};

right_half right;

struct left_half {
  int min_cost;

  void rec(int k, int c, u64 left_mask, u64 right_mask) {
    if (k == num_colors) {
      int right_cost = right.cheapest_expansion(right_mask);
      min_cost = min(min_cost, c + right_cost);
    } else if (c < min_cost) {
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
  right.compute_mask_costs();
  right.compute_red_costs();
  left.generate_all_valid();
  printf("%d\n", left.min_cost);

  return 0;
}
