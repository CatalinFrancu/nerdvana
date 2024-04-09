// Metodă: O(n^3) cu optimizări. Față de n3-v3.cpp, am clarificat contractele
// variabilelor. Pentru un i și j fixate,
//
// * k este cel mai mic indice (cel mult j) pentru care d[i][k-1] >= d[k+1][j].
// * Coada de minime q ține poziții de tăieturi cuprinse între k și j inclusiv.
#include <stdio.h>

const int MAX_N = 5'000;

struct sorted_queue {
  struct pair {
    int pos, val;
  };

  pair p[MAX_N];
  int head, tail;

  void flush() {
    head = tail = 0;
  }

  void push(int pos, int val) {
    p[tail++] = { pos, val };
  }

  int first() {
    return p[head].val;
  }

  void evict_head(int pos) {
    while (pos > p[head].pos) {
      head++;
    }
  }

  // Noul minim scoate din coadă valori mai mari.
  void evict_and_push_tail(int pos, int val) {
    while ((head != tail) && (val < p[tail - 1].val)) {
      tail--;
    }
    p[tail++] = { pos, val };
  }
};

int n;
int c[MAX_N];
int d[MAX_N][MAX_N];
sorted_queue q;

void read_data() {
  FILE *f = fopen("takara.in", "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &c[i]);
  }
  fclose(f);
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void update_k(int i, int j, int& k) {
  while ((k < j) && (d[i][k - 1] < d[k + 1][j])) {
    k++;
  }
  q.evict_head(k);
}

void look_left(int i, int j, int k) {
  int l = k - 1;
  while (d[l + 1][j] < d[i][j]) {
    d[i][j] = min(d[i][j], c[l] + d[l + 1][j]);
    l--;
  }
}

void look_right(int i, int j, int k) {
  q.evict_and_push_tail(j, c[j] + d[i][j - 1]);
  d[i][j] = q.first();
}

void compute_costs() {
  for (int i = n - 1; i >= 0; i--) {
    int k = i;
    q.flush();
    q.push(k, c[k]); // + d[i][k - 1], care este 0
    for (int j = i + 1; j < n; j++) {
      update_k(i, j, k);
      look_right(i, j, k);
      look_left(i, j, k);
    }
  }
}

void write_result() {
  FILE* f = fopen("takara.out", "w");
  fprintf(f, "%d\n", d[0][n - 1]);
  fclose(f);
}

int main() {
  read_data();
  compute_costs();
  write_result();

  return 0;
}
