#include <stdio.h>
#include <stdlib.h>

#define MAX_N 1000

int a[MAX_N][MAX_N];
int n;
int max_fruit, min_diff;

void bkt(int r, int c, int fruit, int apples) {
  if (r == n || c == n) {
    int diff = abs(fruit - 2 * apples);
    if ((fruit > max_fruit) ||
        ((fruit == max_fruit) && (diff < min_diff))) {
      max_fruit = fruit;
      min_diff = diff;
    }
    return;
  }

  if (a[r][c]) {
    fruit++;
    apples += (a[r][c] == 1);
  }

  if (r < n) {
    bkt(r + 1, c, fruit, apples);
  }
  if (c < n) {
    bkt(r, c + 1, fruit, apples);
  }
}

int main() {
  scanf("%d", &n);
  n++;
  for (int i = 0; i < n; i++) {
    int r, c, t;
    scanf("%d %d %d", &r, &c, &t);
    a[r][c] = t;
  }

  bkt(0, 0, 0, 0);

  printf("%d %d\n", max_fruit, min_diff);
  //  printf("%d\n", max_fruit);
  return 0;
}
