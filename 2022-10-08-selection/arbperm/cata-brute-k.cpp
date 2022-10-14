// Autor: Cătălin Frâncu
// Complexitate: O(K)
//
// Metodă: avansează la următoarea permutare de K ori. Fiecare avans este O(1).
#include <stdio.h>

#define MAX_N 100'000

// pos[x] = k astfel încît v[k] = x
int v[MAX_N + 1], pos[MAX_N + 1], n;

// atribuie v[k] = val și actualizează pos[] în concordanță
void set(int k, int val) {
  v[k] = val;
  pos[val] = k;
}

void next_permutation() {
  // urcă în arbore cît timp stăm pe un ultim fiu
  int k = n;
  while (v[k] == k) {
    k--;
  }

  // treci la fiul următor: găsește-l pe k și mută-l o poziție la dreapta
  int i = pos[k];
  set(i, v[i + 1]);
  set(i + 1, k);

  if (k < n) {
    // Mută pozițiile 1...k (conținînd elementele 1...k) la sfîrșit. Notă:
    // pentru fiecare mutare (deci O(n)) au existat n-1 pași în care k ==
    // n. Deci aceste bucle costă O(1) amortizat.
    for (int i = n; i > n - k; i--) {
      set(i, v[i - n + k]);
    }

    // mută valorile k+1...n la început
    for (int i = k + 1; i <= n; i++) {
      set(n - i + 1, i);
    }
  }
}

int main() {
  int k;

  FILE* f = fopen("arbperm.in", "r");
  fscanf(f, "%d %d", &n, &k);
  for (int i = 1; i <= n; i++) {
    fscanf(f, "%d", &v[i]);
    pos[v[i]] = i;
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
