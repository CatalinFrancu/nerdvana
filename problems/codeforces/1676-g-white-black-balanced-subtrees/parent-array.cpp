#include <stdio.h>

const int MAX_NODES = 4'000;
const int WHITE = 0;
const int BLACK = 1;

struct node {
  short parent;
  short num_children; // children who have not yet reported to us
  short cnt[2];
  unsigned char color;
};

node nd[MAX_NODES + 1];
int n, answer;

void read_data() {
  scanf("%d", &n);
  for (int i = 2; i <= n; i++) {
    scanf("%hd ", &nd[i].parent);
    nd[nd[i].parent].num_children++;
  }

  for (int i = 1; i <= n; i++) {
    nd[i].color = (getchar() == 'B') ? BLACK : WHITE;
  }
}

// All of u's children have reported to u. Time for u to count its own
// contribution, then report to its parent.
void process(node& u) {
  u.cnt[u.color]++; // count self
  answer += (u.cnt[WHITE] == u.cnt[BLACK]);

  nd[u.parent].cnt[WHITE] += u.cnt[WHITE];
  nd[u.parent].cnt[BLACK] += u.cnt[BLACK];
}

void traverse_tree() {
  for (int u = 1; u <= n; u++) {
    int s = u;
    while (s && !nd[s].num_children) {
      process(nd[s]);
      s = nd[s].parent;
      nd[s].num_children--;
    }
  }
}

void reset() {
  for (int i = 1; i <= n; i++) {
    nd[i] = { 0, 0, 0, 0, 0 };
  }
  answer = 0;
}

void solve_test() {
  read_data();
  traverse_tree();
  printf("%d\n", answer);
  reset();
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test();
  }

  return 0;
}
