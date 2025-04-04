// Sweep line implemented with sets and a custom less-than operator.
#include <algorithm>
#include <set>
#include <stdio.h>

const int MAX_N = 100'000;

// Returns true iff (x1,y1), (x2,y2) and (x3,y3) are in trigonometric order.
long long orientation(int x1, int y1, int x2, int y2, int x3, int y3) {
  return ((long long)(x2 - x1)) * (y3 - y2)
    - ((long long)(x3 - x2)) * (y2 - y1);
}

struct segment {
  int x1, y1, x2, y2;
  int ind;

  // Returns true if this segment is "under" o, false otherwise.
  bool operator<(segment const& o) const {
    if (x2 < o.x2) {
      // This segment is *not* under o if (x2,y2) falls onto o, i.e. if
      // (o.x1,o.y1), (o.x2,o.y2) and (x2,y2) are in clockwise order.
      return orientation(o.x1, o.y1, o.x2, o.y2, x2, y2) < 0;
		} else {
      // This segment *is* under o if (o.x2,o.y2) falls onto this segment,
      // i.e. if (x1,y1), (x2,y2) and (o.x2,o.y2) are in clockwise order.
      return orientation(x1, y1, x2, y2, o.x2, o.y2) > 0;
		}
  }
};

struct event {
  int ind;
  int x, y;
};

segment seg[MAX_N];
event evt[2 * MAX_N];
std::set<segment> set;
int n, bessie, hill_count;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d %d %d %d", &seg[i].x1, &seg[i].y1, &seg[i].x2, &seg[i].y2);
    seg[i].ind = i;
  }
}

void create_and_sort_events() {
  for (int i = 0; i < n; i++) {
    evt[2 * i] = { i, seg[i].x1, seg[i].y1 };
    evt[2 * i + 1] = { i, seg[i].x2, seg[i].y2 };
  }

  std::sort(evt, evt + 2 * n, [](event& a, event& b) {
    return (a.x < b.x) || ((a.x == b.x) && (a.y < b.y));
  });
}

void process_end(event& e) {
  std::set<segment>::iterator it = set.find(seg[e.ind]);

  if (e.ind == bessie) {
    if (it == set.begin()) {
      bessie = n;
    } else {
      std::set<segment>::iterator prev = std::prev(it);
      bessie = prev->ind;
      hill_count++;
    }
  }

  set.erase(it);
}

void sweep_line() {
  bessie = 0;
  hill_count = 1;

  for (int i = 0; i < 2 * n; i++) {
    event& e = evt[i];
    segment& s = seg[e.ind];
    if (e.x == s.x1) {
      set.insert(s);
    } else {
      process_end(e);
    }
  }
}

int main() {
  read_data();
  create_and_sort_events();
  sweep_line();
  printf("%d\n", hill_count);

  return 0;
}
