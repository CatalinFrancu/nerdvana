#include <stdio.h>

#define MAX_MEETINGS 50000000

#define T_SPLIT 1
#define T_SKIP 2

typedef struct {
  int x, y;
} meeting;

meeting m[MAX_MEETINGS];

int main() {

  int n, num_ops;
  long long result = 0;

  // read meetings
  scanf("%d %d", &n, &num_ops);
  for (int i = 0; i < n; i++) {
    scanf("%d %d", &m[i].x, &m[i].y);
    result += m[i].y - m[i].x;
  }

  while (num_ops--) {
    int op, t;
    scanf("%d %d\n", &op, &t);

    if (op == T_SPLIT) {
      for (int i = 0; i < n; i++) {
        if (m[i].x < t && m[i].y > t) {
          m[n++] = { t, m[i].y };
          m[i].y = t;
        }
      }
    } else { // T_SKIP
      int i = 0;
      while (i < n) {
        if (m[i].x < t && m[i].y > t) {
          result -= (m[i].y - m[i].x);
          m[i] = m[--n];
        } else {
          i++;
        }
      }
    }
  }

  printf("%lld\n", result);

  return 0;
}
