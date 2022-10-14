// Autor: Cătălin Frâncu
// Complexitate: O(N^2)
//
// Metodă: Calculăm rangul permutării (adică numărul de ordine în baza
// factorial). Adunăm K și rulăm *unrank* (adică aflăm permutarea cu acel
// rang). Deoarece 20! < 2^64 < 21!, acest algoritm va funcționa doar pînă la
// N ≤ 20. După aceea, rangul poate depăși unsigned long long. Putem crește
// limita folosind numere mari (de exemplu, 100.000! are circa 457.000 de
// cifre).
#include <assert.h>
#include <stdio.h>

#define MAX_N 100'000
#define MAX_CHANGES 21

int v[MAX_N], n;

// Calculează rangul permutării din v cu n elemente.
unsigned long long rank() {
  // Notă: Jumătate din permutările de N elemente (așadar X = 3 * 4 * ... * N)
  // se găsesc în stînga rădăcinii, iar cealaltă jumătate în dreapta. Pentru a
  // decide dacă să adăugăm sau nu X la rang, trebuie să determinăm dacă 2 se
  // află după 1 sau înaintea lui în permutare. Similar, pe nivelul d trebuie
  // să vedem poziția lui d printre elementele 1, 2, ..., d-1 (care va fi un
  // număr între 0 și d - 1) și să înmulțim acea poziție cu produsul (d + 1) *
  // (d + 2) * ... * n.
  unsigned long long product = 1, result = 0;

  for (int d = n; d >= 1; d--) {
    // Află pos = poziția lui d printre elemente mai mici ca d.
    int i = 0, pos = 0;
    while (v[i] != d) {
      if (v[i] < d) {
        pos++;
      }
      i++;
    }
    result += product * pos;
    product *= d;
  }

  return result;
}

// Scrie permutarea de rang k în v.
void unrank(unsigned long long k) {
  for (int i = 0; i < n; i++) {
    v[i] = 0;
  }
  for (int d = n; d >= 1; d--) {
    int pos = k % d, i = 0;
    // Așază d pe a pos-a poziție încă neocupată
    do {
      while (v[i]) {
        i++;
      }
      pos--;
      i++;
    } while (pos >= 0);

    v[i - 1] = d;
    k /= d;
  }

  // Dacă rămîne ceva, înseamnă că testul este greșit: nu existau K permutări
  // la dreapta lui P.
  assert(k == 0);
}

int main() {
  int k;

  FILE* f = fopen("arbperm.in", "r");
  fscanf(f, "%d %d", &n, &k);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &v[i]);
  }
  fclose(f);

  unrank(rank() + k);

  f = fopen("arbperm.out", "w");
  for (int i = 0; i < n; i++) {
    fprintf(f, "%d ", v[i]);
  }
  fprintf(f, "\n");
  fclose(f);

  return 0;
}
