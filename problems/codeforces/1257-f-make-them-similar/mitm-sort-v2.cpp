// In addition to v1, use the builtin popcount and encapsulate a method.
#include <stdio.h>

const int MAX_N = 100;
const int BITS = 15;
const int MAX_TUPLES = 1 << (BITS + 1);
const bool LEFT = false;
const bool RIGHT = true;

int n;

struct tuple {
  char diff[MAX_N - 1];
  bool half;
  short mask;

  bool matches(const tuple& other) {
    if (half == other.half) {
      return false;
    }

    int i = 0;
    while ((i < n - 1) && (diff[i] == other.diff[i]))  {
      i++;
    }

    return (i == n - 1);
  }
};

int a[MAX_N];
tuple t[MAX_TUPLES];
int perm[MAX_TUPLES];
int num_tuples;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
  }
}

int shift_mask_and_popcount(int ind, int shift, int mask) {
  int val = (a[ind] >> shift) & ((1 << BITS) - 1);
  return __builtin_popcount(mask ^ val);
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

void print_solution(int i, int j) {
  int low = (t[i].half == LEFT) ? t[i].mask : t[j].mask;
  int high = (t[i].half == LEFT) ? t[j].mask : t[i].mask;
  int x = (high << BITS) + low;
  printf("%d\n", x);
}

void find_match() {
  int i = 0;

  while ((i < num_tuples - 1) &&
         !t[perm[i]].matches(t[perm[i + 1]])) {
    i++;
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
