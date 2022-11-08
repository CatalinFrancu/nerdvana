// Complexity: O(n log n) due to sorting, otherwise O(n).
//
// Method: First, some nomenclature. A plateau is a run of consecutive equal
// heights. An inner dip is a plateau that is lower than its plateaus to the
// left and right. An outer dip is a plateau at either end of the array that
// is lower than its neighbor. Dips can be nested, so we can have a lower,
// narrower dip inside a taller, wider dip.
//
// As we read the input data, we keep track of plateaus in p[] and dips in
// d[]. This can be done with a sorted stack of decreasing plateau heights.
// When the latest plateau A is taller than the previous one, B, we remove B
// from the stack and create a dip for it. We take B's height to be the height
// differential between B and its neighboring plateaus. For example, the array
// 5 5 3 3 3 3 3 8 will make an inner dip of length 5 and height 2 (raising
// all the 3's to height 5), then an outer dip of length 7 and height 3
// (raising all the 5's to height 8).
//
// Let's call the objective function "ugliness". Note that filling one unit of
// height in a dip reduces the ugliness by 2 for inner dips or 1 for outer
// dips. We call 1 or 2 "the profit". Therefore, we sort the dips by adjusted
// length: the length for outer dips is multiplied by 2. The point is that an
// inner dip of size 8 will yield the same profit as two outer dips of size 4.
//
// After sorting, scan the dips and fill them up as much as possible. Note
// that this works. It will never attempt to fill a dip without completely
// filling any nested dips. The nested dips are narrower, so they will receive
// priority.
//
// There is one trick at the end, when we have some M remaining credits that
// are insufficient to fill any remaining dips. Let X be the length of the
// widest outer dip filled and Y be the length of the narrowest inner dip not
// filled. If X + M > Y, it is worth emptying the X dip and filling the Y dip
// instead, which reduces the ugliness by 1. This only works once.
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#define MAX_N 1'000'000

typedef struct {
  int len, h;
} plateau;

typedef struct {
  int profit; // 1 for outer, 2 for inner
  int len;
  int h;      // height differential to the next plateau
} dip;

dip d[MAX_N];
plateau p[MAX_N];

long long min(long long x, long long y) {
  return (x < y) ? x : y;
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);

  while (num_tests--) {
    int n, np = 0, nd = 0;  // numbers of plateaus and dips
    long long k, ugliness = 0;

    scanf("%d %lld", &n, &k);

    // Invariant: p stores a max-stack of decreasing plateaus.
    // Whenever we remove a plateau, we create a dip.
    while (n--) {
      int h;
      scanf("%d", &h);
      if (np) {
        ugliness += abs(h - p[np - 1].h);
      }

      if (np && h == p[np - 1].h) {
        p[np - 1].len++;    // existing plateau
      } else {
        p[np++] = { 1, h }; // new plateau
        while (np >= 2 && h > p[np - 2].h) {
          // evict the plateau at position np - 2 from the stack
          int closest = (np > 2 && p[np - 3].h < h)
            ? (np - 3)
            : (np - 1);

          d[nd++] = {
            (np == 2) ? 1 : 2,          // profit
            p[np - 2].len,              // length
            p[closest].h - p[np - 2].h  // height
          };

          p[closest].len += p[np - 2].len;
          p[np - 2] = p[np - 1];
          np--;
        }
      }
    }

    // All that remains is a downward slope from the tallest hill eastwards
    while (np > 1) {
      d[nd++] = { 1, p[np - 1].len, p[np - 2].h - p[np - 1].h };
      p[np - 2].len += p[np - 1].len;
      np--;
    }

    // sort dips by adjusted length
    std::sort(d, d + nd, [](dip a, dip b) {
      int la = (a.profit == 1) ? 2 * a.len : a.len;
      int lb = (b.profit == 1) ? 2 * b.len : b.len;
      return (la < lb) || ((la == lb) && (a.profit > b.profit));
    });

    // widest outer dip filled, narrowest inner dip not filled
    int wof = -1, ninf = -1;

    for (int i = 0; i < nd; i++) {
      // fill the dip either to its brink or until we run out of credits
      long long count = min(d[i].h, k / d[i].len);
      ugliness -= count * d[i].profit;
      k -= count * d[i].len;

      if ((count < d[i].h) && (d[i].profit == 2) && (ninf == -1)) {
        ninf = d[i].len;
      } else if (count && (d[i].profit == 1)) {
        wof = d[i].len;
      }
    }

    // the trick at the end
    if ((wof != -1) && (ninf != -1) && (wof + k >= ninf)) {
      ugliness--;
    }

    printf("%lld\n", ugliness);
  }

  return 0;
}
