// input data validator
#include <assert.h>
#include <stdio.h>
#include <unordered_set>

#define MAX_N 35'000

#define T_APPLE 1
#define T_PEAR 2

typedef struct p {
  int r, c;

  bool operator==(const p& other) const {
    return (this->r == other.r) && (this->c == other.c);
  }
} point;

struct hashFunction {
  size_t operator()(const point &p) const {
    return p.r ^ p.c;
  }
};

int main() {
  int n;

  scanf("%d", &n);

  assert(n >= 1 && n <= 35000);
  
  std::unordered_set<point, hashFunction> s;
  for (int i = 0; i < n; i++) {
    point p;
    int t;
    assert(scanf("%d %d %d", &p.r, &p.c, &t) == 3);
    assert(p.r >= 1 && p.r <= n);
    assert(p.c >= 1 && p.c <= n);
    assert(t == 1 || t == 2);
    assert(s.find(p) == s.end());
    s.insert(p);
  }

  while (true) {
  }

  return 0;
}
