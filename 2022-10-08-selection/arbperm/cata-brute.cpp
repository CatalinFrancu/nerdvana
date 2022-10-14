// Autor: Cătălin Frâncu
// Complexitate: O(KN)
//
// Metodă: avansează la următoarea permutare de K ori. Fiecare avans este O(N).
#include <stdio.h>

#define MAX_N 100'000

int v[MAX_N + 1], n;

void next_permutation() {
  // urcă în arbore cît timp stăm pe un ultim fiu
  int k = n;
  while (v[k] == k) {
    k--;
  }

  // Treci la fiul următor: găsește-l pe k și mută-l o poziție la dreapta.
  int i = k;
  while (v[i] != k) {
    i--;
  }
  v[i] = v[i + 1];
  v[i + 1] = k;

  // mută pozițiile 1...k (conținînd elementele 1...k) la sfîrșit
  for (int i = n; i > n - k; i--) {
    v[i] = v[i - n + k];
  }

  // mută valorile k+1...n la început
  for (int i = k + 1; i <= n; i++) {
    v[n - i + 1] = i;
  }
}

int main() {
  int k;

  FILE* f = fopen("arbperm.in", "r");
  fscanf(f, "%d %d", &n, &k);
  for (int i = 1; i <= n; i++) {
    fscanf(f, "%d", &v[i]);
  }
  fclose(f);

  while (k--) {
    next_permutation();
  }

  f = fopen("arbperm.out", "w");
  for (int i = 1; i <= n; i++) {
    fprintf(f, "%d ", v[i]);
  }
  fprintf(f, "\n");
  fclose(f);

  return 0;
}
