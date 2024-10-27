#include <stdio.h>

const int MAX_N = 100;
const int BITS = 15;
const int MAX_TUPLES = 2 << (BITS + 1);
const bool LEFT = false;
const bool RIGHT = true;

typedef unsigned short u16;

struct tuple {
  char diff[MAX_N - 1];
  bool half;
  short mask;
};

int a[MAX_N];
tuple t[MAX_TUPLES];
int perm[MAX_TUPLES];
int n, num_tuples;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
  }
}

static inline int popcount(u16 x) {
  x = ((x >> 1) & 0x5555) + (x & 0x5555);
  x = ((x >> 2) & 0x3333) + (x & 0x3333);
  x = ((x >> 4) + x) & 0x0f0f;
  x = ((x >> 8) + x) & 0x00ff;
  return x;
}

static inline int shift_mask_and_popcount(int ind, int shift, int mask) {
  int val = (a[ind] >> shift) & ((1 << BITS) - 1);
  return popcount(mask ^ val);
}

void iterate_masks(int shift, bool half) {
  for (int mask = 0; mask < (1 << BITS); mask++) {
    int pc0 = shift_mask_and_popcount(0, shift, mask);
    tuple& x = t[num_tuples];
    x.mask = mask;
    x.half = half;

    for (int i = 1; i < n; i++) {
      int pc = shift_mask_and_popcount(i, shift, mask);
      x.diff[i - 1] = (half == LEFT) ? (pc - pc0) : (pc0 - pc);
    }

    num_tuples++;
  }
}

void swap(int i, int j) {
  int tmp = perm[i];
  perm[i] = perm[j];
  perm[j] = tmp;
}

// Sorts the tuples t[first] ... t[last]), which are known to be equal up to
// index pos-1. Changes perm[] during the partitioning step, but does not
// change t[], which would be expensive.
void ternary_sort(int first, int last, int pos) {
  if (last - first <= 1) {
    return;
  }

  int i = first, left = first, right = last;
  char pivot = t[perm[first]].diff[pos];

  // Loop invariant:
  // * perm[first ... left)     the pos-th difference is < pivot
  // * perm[left ... i)         the pos-th difference is == pivot
  // * perm[i]                  the tuple currently being evaluated
  // * perm(i ... right)        tuples yet to be evaluated
  // * perm[right ... last)     the pos-th difference is > pivot
  while (i < right) {
    char x = t[perm[i]].diff[pos];
    if (x < pivot) {
      swap(i++, left++);
    } else if (x > pivot)  {
      swap(i, --right);
    } else {
      i++;
    }
  }

  ternary_sort(first, left, pos);
  if (pos < n - 1) {
    ternary_sort(left, right, pos + 1);
  }
  ternary_sort(right, last, pos);
}

void sort_tuples() {
  for (int i = 0; i < num_tuples; i++) {
    perm[i] = i;
  }
  ternary_sort(0, num_tuples, 0);
}

// Compares two arrays of length n - 1.
bool equal_arrays(char* a, char* b) {
  int i = 0;
  while ((i < n - 1) && (a[i] == b[i]))  {
    i++;
  }

  return (i == n - 1);
}

void print_solution(int i, int j) {
  int low = (t[i].half == LEFT) ? t[i].mask : t[j].mask;
  int high = (t[i].half == LEFT) ? t[j].mask : t[i].mask;
  int x = (high << BITS) + low;
  printf("%d\n", x);
}

void find_match() {
  int i = 0;
  bool found = false;

  // Use a flag to avoid an unreasonably long while condition.
  while ((i < num_tuples - 1) && !found) {
    int j = perm[i], k = perm[i + 1];

    if ((t[j].half != t[k].half) &&
        equal_arrays(t[j].diff, t[k].diff)) {
      found = true;
    } else {
      i++;
    }
  }

  if (i == num_tuples - 1) {
    printf("-1\n");
  } else {
    print_solution(perm[i], perm[i + 1]);
  }
}

int main() {
  read_data();
  iterate_masks(0, LEFT);
  iterate_masks(BITS, RIGHT);
  sort_tuples();
  find_match();

  return 0;
}

// We use the "Meet in the middle" technique.
//
// Iterate through all the 15-bit masks. For every mask, xor it with the least
// significant 15 bits of every a_i. Count the set bits in every resulting
// value. This produces an array A of 2^15 tuples, each tuple having N values.
//
// Do the same for the most significant 15 bits. This produces a similar array
// B. For every tuple also store the mask that produced it.
//
// Now we want to find two tuples, one from each array, such that the sum of
// the two tuples contains N identical values. If we find them, then x
// consists of the concatenated masks of the tuples.
//
// How do we find the tuples? We don't care what the values are in the sum of
// tuples, only that they are identical. Therefore, we can only store N-1
// values in each tuple: instead of d_0, d_1, ..., d_{n-1} we store (d_1-d_0),
// (d_2-d_0), ..., (d_{n-1}-d_0). Thus, we need to find two tuples that sum up
// to 0.
//
// Better yet, we can negate the signs of all tuples in B, then search for two
// equal tuples, but from different halves.
//
// We could build a hash map for A and use it to look up tuples from B. But
// it's more efficient to merge the arrays A and B, then sort the merged
// array. Then, if two equal tuples exists, they will necessarily be
// consecutive in the sorted array.
//
// One noteworthy detail is that sorting tuples using std::sort is slow,
// because std::sort is agnostic and always compares tuples from their
// beginning. It is much more efficient to use ternary sort, similar to string
// sorting.
