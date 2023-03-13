// Solves https://www.nerdarena.ro/problema/infasuratoare
#include <algorithm>
#include <stdio.h>

#define MAX_N 100000

typedef struct {
  int x, y;
} point;

point p[MAX_N];
int n;
int st[MAX_N], ss;  // Graham's scan stack.

long long orientation(point a, point b, point c) {
  return ((long long)(b.y - a.y)) * (c.x - b.x) - ((long long)(b.x - a.x)) * (c.y - b.y);
}

long long dist2(point a, point b) {
  return ((long long)(a.x - b.x)) * (a.x - b.x) + ((long long)(a.y - b.y)) * (a.y - b.y);
}

bool cmp(point a, point b) {
  long long disc = orientation(p[0], a, b);
  if (disc == 0) {
    // a and b are collinear with p[0]. Put the furthest one first.
    return dist2(a, p[0]) > dist2(b, p[0]);
  } else {
    return disc < 0;
  }
}

int main(void) {
  // Read the input data.
  FILE *f = fopen("infasuratoare.in", "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d %d", &p[i].x, &p[i].y);
  }
  fclose(f);

  // Put the point having the minimum y first (minimum x as tiebreaker).
  int min = 0;
  for (int i = 1; i < n; i++) {
    if ((p[i].y < p[min].y) || (p[i].y == p[min].y && p[i].x < p[min].x)) {
      min = i;
    }
  }
  point tmp = p[0];
  p[0] = p[min];
  p[min] = tmp;

  // Sorts the other points in polar order with respect to the first.
  std::sort(p + 1, p + n, cmp);

  // Graham's scan
  st[0] = 0;
  st[1] = 1;
  ss = 2;
  for (int i = 2; i < n; i++) {
    // Ignore points on the same ray as the previous point. Since the previous
    // point was further away, the current one won't help.
    if (orientation(p[0], p[st[ss - 1]], p[i]) != 0) {
      while (orientation(p[st[ss - 2]], p[st[ss - 1]], p[i]) >= 0) {
        ss--;
      }
      st[ss++] = i;
    }
  }

  // Print the convex hull.
  f = fopen("infasuratoare.out", "w");
  fprintf(f, "%d\n", ss);
  for (int i = 0; i < ss; i++) {
    fprintf(f, "%d %d\n", p[st[i]].x, p[st[i]].y);
  }
  fclose(f);
}
