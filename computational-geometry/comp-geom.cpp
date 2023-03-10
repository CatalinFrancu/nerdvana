#include <stdio.h>

typedef struct {
  int x, y;
} point;

int sgn(long long x) {
  return (x > 0) - (x < 0);
}

int det(point a, point b, point c) {
  return sgn((long long)(b.x - a.x) * (c.y - a.y) -
             (long long)(c.x - a.x) * (b.y - a.y));
}

bool segments_intersect(point a, point b, point c, point d) {
  return
    (det(a, b, c) != det(a, b, d)) &&
    (det(c, d, a) != det(c, d, b));
}

int main() {
  // Test whether AB and CD intersect
  point a, b, c, d;
  scanf("%d %d %d %d %d %d %d %d",
        &a.x, &a.y, &b.x, &b.y, &c.x, &c.y, &d.x, &d.y);
  printf("%d\n", segments_intersect(a, b, c, d));

  return 0;
}
