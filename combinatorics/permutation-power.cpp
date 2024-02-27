#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int MAX_N = 1000;
const int MAX_K = 1000;

int p[MAX_N + 1][MAX_K + 1];
bool vis[MAX_N + 1];
int n, k;

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int micros = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(micros);
}

int rand2(int x, int y) {
  return rand() % (y - x + 1) + x;
}

void write_perm(int k)  {
  printf("p^%-2d = [", k);
  for (int i = 1; i <= n; i++) {
    printf(" %2d", p[k][i]);
  }
  printf("] =");

  for (int i = 1; i <= n; i++) {
    vis[i] = false;
  }
  for (int i = 1; i <= n; i++) {
    if (!vis[i]) {
      int j = i;
      printf("(");
      do {
        printf("%d ", j);
        vis[j] = true;
        j = p[k][j];
      } while (j != i);
      printf(")");
    }
  }
  printf("\n");
}

void read_perm() {
  scanf("%d %d", &n, &k);
  for (int i = 1; i <= n; i++) {
    scanf("%d", &p[1][i]);
  }
}

void compute_powers() {
  for (int e = 2; e <= k; e++) {
    for (int i = 1; i <= n; i++) {
      int j = rand2(1, e - 1);
      p[e][i] = p[j][p[e-j][i]];
    }
    write_perm(e);
  }
}

int main() {
  read_perm();
  write_perm(1);
  compute_powers();
}
