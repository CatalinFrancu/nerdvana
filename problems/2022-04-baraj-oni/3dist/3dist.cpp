#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#define MAX_N 250'000
#define INFINITY 2'100'000'000

typedef struct {
  int x, y, d;
  int rx; // we'll reindex x's from 1 to n when computing d values
} point;

int f[MAX_N + 1]; // Fenwick tree
point p[MAX_N];
int diff[MAX_N], neighbor[MAX_N];
int n, num_neighbors;

int max(int x, int y) {
  return (x > y) ? x : y;
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void fenwick_reset() {
  for (int i = 1; i <= n; i++) {
    f[i] = -INFINITY;
  }
}

// max update: v[pos] increases to val
void fenwick_update(int pos, int val) {
  do {
    f[pos] = max(f[pos], val);
    pos += pos & -pos;
  } while (pos <= n);
}

// partial max: returns max(val[1...pos])
int fenwick_max(int pos) {
  int m = -INFINITY;
  while (pos) {
    m = max(m, f[pos]);
    pos &= pos - 1;
  }
  return m;
}

int dist(int a, int b) {
  return abs(p[a].x - p[b].x) + abs(p[a].y - p[b].y);
}

// updates every point's d against points to its south-west
void scan_quadrant() {
  // reindex x coordinates from 1 to n
  std::sort(p, p + n, [](point a, point b) {
    return (a.x < b.x) || (a.x == b.x && a.y < b.y);
  });
  for (int i = 0; i < n; i++) {
    p[i].rx = i + 1;
  }

  // process points in increasing y order
  std::sort(p, p + n, [](point a, point b) {
    return (a.y < b.y) || (a.y == b.y && a.x < b.x);
  });

  fenwick_reset();

  for (int i = 0; i < n; i++) {
    // query the highest x+y (the point closest to i) among lower x's
    int m = fenwick_max(p[i].rx);
    if (m != -INFINITY) {
      p[i].d = min(p[i].d, p[i].x + p[i].y - m);
    }
    // now add ourselves to the tree
    fenwick_update(p[i].rx, p[i].x + p[i].y);
  }

  // rotate 90° to prepare for the next quadrant
  for (int i = 0; i < n; i++) {
    p[i] = { p[i].y, -p[i].x, p[i].d };
  }
}

// returns true iff p is to the south-west of q
bool is_south_west(point p, point q) {
  return (p.y + p.x < q.y + q.x) ||
    (p.y + p.x == q.y + q.x && p.x < q.x);
}

// returns true iff p is to the south-east of q
bool is_south_east(point p, point q) {
  return (p.y - p.x < q.y - q.x) ||
    (p.y - p.x == q.y - q.x && p.x < q.x);
}

// finds q or the first point to the right of q on the SW-NE diagonal
int bin_search_west(int l, int r, point q) {
  l--; r--; // transform [l, r) to (l, r]
  while (l < r - 1) {
    int mid = (l + r) >> 1;
    if (is_south_east(p[diff[mid]], q)) {
      l = mid;
    } else {
      r = mid;
    }
  }

  // fix for when q is larger than every point
  return is_south_east(p[diff[r]], q) ? (r + 1) : r;
}

// Adds a neighbor j of point i if the distances match. Returns the number of
// triplets it forms with previous neighbors k.
int consider_neighbor(int i, int j) {
  int result = 0;

  if (dist(i, j) == p[i].d) {
    for (int k = 0; k < num_neighbors; k++) {
      result += (dist(j, neighbor[k]) == p[i].d);
    }
    neighbor[num_neighbors++] = j;
  }

  return result;
}

// processes a batch of points having the same d value
long long process_batch(int l, int r) {
  // Within each batch, index points by y-x. Then p[diff[l...r]] will traverse
  // points in south-east to north-west order.
  for (int i = l; i < r; i++) {
    diff[i] = i;
  }
  std::sort(diff + l, diff + r, [](int a, int b) {
    return is_south_east(p[a], p[b]);
  });

  long long result = 0;
  int j = l;

  for (int i = l; i < r; i++) {
    // noteworthy corners of the diamond centered at p[i] of radius d
    point north = { p[i].x, p[i].y +  p[i].d };
    point east = { p[i].x + p[i].d, p[i].y };
    point west = { p[i].x - p[i].d, p[i].y };
    num_neighbors = 0;

    // scan the north-west edge from west to north
    int k = bin_search_west(l, r, west);
    while (k < r && is_south_east(p[diff[k]], north)) {
      result += consider_neighbor(i, diff[k++]);
    }

    // scan the north-east edge from north to east; note that j (the north
    // corner) increases as i increases, so no binary search is necessary here
    while (j < r && is_south_west(p[j], north)) {
      j++;
    }
    k = j;
    while (k < r && is_south_west(p[k], east)) {
      result += consider_neighbor(i, k++);
    }
  }

  return result;
}

long long process_batches() {
  // sort points by their d value, then south-west to north-east
  std::sort(p, p + n, [](point a, point b) {
    return (a.d < b.d) ||
      (a.d == b.d && a.x + a.y < b.x + b.y) ||
      (a.d == b.d && a.x + a.y == b.x + b.y && a.x < b.x);
  });

  long long result = 0;
  int batch_start = 0;

  // identify batches of points having the same d value
  for (int i = 1; i < n; i++) {
    if (p[i].d != p[i - 1].d) {
      result += process_batch(batch_start, i);
      batch_start = i;
    }
  }
  result += process_batch(batch_start, n);

  return result;
}

int main() {

  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d %d", &p[i].x, &p[i].y);
    p[i].d = INFINITY;
  }

  // compute d values
  for (int i = 0; i < 4; i++) {
    scan_quadrant();
  }

  printf("%lld\n", process_batches());

  return 0;
}
