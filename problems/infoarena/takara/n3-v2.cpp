// Metodă: O(n^3) cu o optimizare. Față de n3.cpp, adăugăm observația că
// d[i][k] crește pe măsură ce k crește, iar d[k][j] descrește pe măsură ce k
// crește. Atunci cele două funcții se vor intersecta undeva. Echivalent,
// funcția
//
// max(d[i][k-1], d[k+1][j])
//
// este bitonă. Mai mult, pentru un i fixat, k crește pe măsură ce j
// crește. De aceea, îl putem recalcula pe k în O(1) amortizat.
//
// Acum pentru l < k, maximul este dat de jumătatea dreaptă, adică costul
// testării la l este c[l] + d[l+1][j]. Pentru l > k, maximul este dat de
// jumătatea stîngă, adică costul testării la l este dat de c[l] + d[i][l-1].
//
// Din păcate, adunarea valorilor din c[] denaturează funcția, care nu mai
// este bitonă. Este nevoie să mergem naiv în stînga și în dreapta lui k cît
// timp funcția nu depășește minimul deja obținut.
#include <stdio.h>

const int MAX_N = 5'000;

int n;
int c[MAX_N];
int d[MAX_N][MAX_N];

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
  d[i][j] = c[k] + d[i][k - 1];
}

void look_left(int i, int j, int k) {
  int l = k - 1;
  while (d[l + 1][j] < d[i][j]) {
    d[i][j] = min(d[i][j], c[l] + d[l + 1][j]);
    l--;
  }
}

void look_right(int i, int j, int k) {
  int l = k + 1;
  while (d[i][l - 1] < d[i][j]) {
    d[i][j] = min(d[i][j], c[l] + d[i][l - 1]);
    l++;
  }
}

void compute_costs() {
  for (int i = n - 1; i >= 0; i--) {
    int k = i;
    for (int j = i + 1; j < n; j++) {
      update_k(i, j, k);
      look_left(i, j, k);
      look_right(i, j, k);
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
