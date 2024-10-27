// In addition to v3, calculate the deltas on demand to save (a lot of)
// memory. We no longer need the perm array -- we can swap tuples cheaply.
#include <stdio.h>

const int MAX_N = 100;
const int BITS = 15;
const int MAX_TUPLES = 1 << (BITS + 1);
const char LEFT = 0;
const char RIGHT = 1;

int a[2][MAX_N];
int n;

struct tuple {
  unsigned char half;
  unsigned short mask;

  int diff(int pos) {
    int delta =
      __builtin_popcount(mask ^ a[half][pos]) -
      __builtin_popcount(mask ^ a[half][0]);

    return (1 - 2 * half) * delta;
  }

  bool matches(tuple& other) {
    if (half == other.half) {
      return false;
    }

    int i = 1;
    while ((i < n) && (diff(i) == other.diff(i)))  {
      i++;
    }

    return (i == n);
  }
};

tuple t[MAX_TUPLES];
int num_tuples;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    int x;
    scanf("%d", &x);
    a[0][i] = x >> BITS;
    a[1][i] = x & ((1 << BITS) - 1);
  }
}

void swap(int i, int j) {
  tuple tmp = t[i];
  t[i] = t[j];
  t[j] = tmp;
}

// Sorts the tuples t[first] ... t[last]), which are known to be equal up to
// index pos-1.
void ternary_sort(int first, int last, int pos) {
  if (last - first <= 1) {
    return;
  }

  int i = first, left = first, right = last;
  char pivot = t[first].diff(pos);

  // Loop invariant:
  // * t[first ... left)     the pos-th difference is < pivot
  // * t[left ... i)         the pos-th difference is == pivot
  // * t[i]                  the tuple currently being evaluated
  // * t(i ... right)        tuples yet to be evaluated
  // * t[right ... last)     the pos-th difference is > pivot
  while (i < right) {
    char x = t[i].diff(pos);
    if (x < pivot) {
      swap(i++, left++);
    } else if (x > pivot)  {
      swap(i, --right);
    } else {
      i++;
    }
  }

  ternary_sort(first, left, pos);
  if (pos < n) {
    ternary_sort(left, right, pos + 1);
  }
  ternary_sort(right, last, pos);
}

void create_and_sort_tuples() {
  for (unsigned short mask = 0; mask < (1 << BITS); mask++) {
    t[num_tuples++] = { false, mask };
    t[num_tuples++] = { true, mask };
  }
  ternary_sort(0, num_tuples, 1);
}

void print_solution(int pos) {
  int sol = (t[pos].half == LEFT)
    ? ((t[pos].mask << BITS) | t[pos + 1].mask)
    : ((t[pos + 1].mask << BITS) | t[pos].mask);
  printf("%d\n", sol);
}

void find_match() {
  int i = 1;
  while ((i < num_tuples) && !t[i - 1].matches(t[i])) {
    i++;
  }

  if (i == num_tuples) {
    printf("-1\n");
  } else {
    print_solution(i - 1);
  }
}

int main() {
  read_data();
  create_and_sort_tuples();
  find_match();

  return 0;
}
