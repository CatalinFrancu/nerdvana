// Complexity: O(n log^2 n). Comments at the end.
#include <set>
#include <stdio.h>

const int MAX_N = 300'000;

int a[MAX_N], p[MAX_N];
int freq[MAX_N + 1];
int choices[MAX_N + 1];
int n, k;

// Keeps track of a set of numbers and their product, capped at k.
struct factor_set {
  std::multiset<int> set;

  void init() {
  }

  void insert(int f) {
    if (f > 1) {
      set.insert(f);
    }
  }

  void erase(int f) {
    if (f > 1) {
      set.erase(set.find(f));
    }
  }

  int slow_product() {
    long long p = 1;
    for (int f: set) {
      p *= f;
      if (p >= k) {
        return k;
      }
    }
    return p;
  }

  int product() {
    if (set.size() >= 31) {
      // Can't hurt and may help occasionally.
      return k;
    } else {
      return slow_product();
    }
  }
};

factor_set fs;
std::set<int> hand; // values still in hand, to be placed in p

void read_data() {
  scanf("%d %d", &n, &k);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
    freq[a[i]]++;
  }
}

void init_choices() {
  fs.init();

  int constr = 0; // number of constraints <= val
  for (int val = 1; val <= n; val++) {
    constr += freq[val];
    choices[val] = constr - (val - 1); // val - 1 elements already made their choices
    fs.insert(choices[val]);
  }
}

int find_value_to_satisfy(int constr) {
  auto it = hand.lower_bound(constr);

  fs.erase(choices[*it]);

  // Now try to advance from *it while we we are not leaving behind more than
  // k permutations.
  int p;
  while ((p = fs.product()) < k) {
    // I am taking away constraint constr, which is ≤ *it, therefore *it loses
    // one choice even if we later settle on a value > *it.
    k -= p;
    choices[*it]--;
    fs.insert(choices[*it]);
    it++;
    fs.erase(choices[*it]);
  }

  return *it;
}

void fill_permutation() {
  for (int val = 1; val <= n; val++) {
    hand.insert(val);
  }
  for (int i = 0; i < n; i++) {
    p[i] = find_value_to_satisfy(a[i]);
    hand.erase(p[i]);
  }
}

void write_answer() {
  for (int i = 0; i < n; i++) {
    printf("%d ", p[i]);
  }
  printf("\n");
}

int main() {
  read_data();
  init_choices();
  fill_permutation();
  write_answer();

  return 0;
}

// Regular permutation unranking, with trickier math.
//
// First, how do we compute the number of permutations given a set of values
// (initially 1..n) and a set of constraints (initially a[])? Process values
// in increasing order. For each value x, the number of possible placements is
// C[x] = A[x] - B[x], where A[x] is the number of constraints ≤ x and B[x] is
// the number of values < x. This is because x can satisfy any of those A[x]
// constraints, but B[x] of them were taken by values before x. The number
// of valid permutations is prod(C), the product of all the values of C.
//
// Now assume that we have placed some values p[0...i-1] while satisfying the
// constraints a[0...i-1]. We want to find p[i]. Consider all remaining values
// in increasing order, beginning at or above a[i] (which we must satisfy).
// For each value x, ask the question: If we remove x and a[i] from C, how
// many permutations are left? If the new prod(C) is k or more, then assign
// p[i]=x. Otherwise, subtract prod(C) from k and try the next greater value
// after x. This is the regular unranking algorithm, except prod(C) isn't
// simply some factorial value.
//
// We can compute prod(C) in O(n) as shown above. However, we need to
// recompute it incrementally. Let us consider the last example from the
// statement. Assume we have already determined the first 6 values. We still
// need to satisfy the last 14 constraints (sorted)
//
//   1  1  1  4  5  6  6  6  7  7  9 11 13 18
//
// using the last 14 values (sorted)
//
//   3  5  7  8  9 10 11 14 15 16 17 18 19 20
//
// Assume we know C from the previous step:
//
//   3  4  8  7  7  6  6  6  5  4  3  3  2  1     = C
//
// We begin by using value 14, the smallest remaining value that satisfies
// constraint a[6]=13. Remove C[14]. If we recompute C naively, we get:
//
//   3  4  8  7  7  6  6  -  5  4  3  3  2  1     = C
//
// Note that other values did not change. Indeed, values smaller than 13/14
// are unaffected by the change. Values larger than 13 lose a choice because
// constraint 13 is gone, but also gain a choice because value 14, which was
// competing for those choices, is also gone.
//
// The new prod(C) is about 61M, less than the rank we still need. Subtract
// prod(C) from k and replace 14 with 15. The new C is:
//
//   3  4  8  7  7  6  6  5  -  4  3  3  2  1     = C
//
// Note that C[14] came back as 5, not 6. This makes sense, because constraint
// 13 is gone, so there is one less choice for value 14. Other values are not
// affected, for the same reasoning as above.
//
// All in all, we can update prod(C) in O(1), with one caveat. Sometimes the
// product will be too large to represent. Fortunately, we don't care about
// the exact value. We just need to know whether it is above or below k. So we
// store the factors in a set that returns their product when possible or
// INFINITY when it's too large. This is currently implemented in O(log n)
// using a multiset.
