// Autor: Cătălin Frâncu
// Complexitate: O(N * min(N, log_N(K)))
//
// Metodă: Începem să calculăm rangul permutării (adică numărul de ordine în
// baza factorial). De la bun început adăugăm K la număr. Ne oprim cînd
// transportul devine 0, căci ordinea elementelor mai mici nu se mai
// schimbă. Scoatem elementele mai mari din permutare și le reinserăm conform
// noului număr în baza factorial.
//
// Fiecare cifră căutată, eliminată și reinserată costă O(N). Acești pași pot
// fi reduși la O(log N) cu arbori indexați binar sau la O(1) prin
// precalcularea permutării inverse, dar efortul este nenecesar. De cîte ori
// se execută acest pas? De cel mult N ori dacă urcăm pînă în rădăcina
// arborelui, ceea ce ar indica o complexitate O(N^2). Pe de altă parte, la
// calculul rangului, K se împarte la N pe ultimul nivel, la N-1 pe penultimul
// nivel etc. Deci vom urca în arbore cel mult O(log_N(K)) niveluri.
//
// Cu alte cuvinte: pentru permutările mici O(N^2) este ieftin, iar pentru
// permutările mari majoritatea elementelor în afară de 2-3 nu trebuie
// reordonate. Din acest motiv nici nu avem nevoie să stocăm mai mult de 21 de
// cifre din codul generat (21! depășește 64 de biți).
#include <assert.h>
#include <stdio.h>

#define MAX_N 100'000
#define MAX_CHANGES 21

int v[MAX_N], fb[MAX_CHANGES], n;

int main() {
  int k;

  FILE* f = fopen("arbperm.in", "r");
  fscanf(f, "%d %d", &n, &k);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &v[i]);
  }
  fclose(f);

  // Construiește numărul în baza factorial. Inițializează transportul cu k și
  // oprește-te cînd acesta devine 0.
  int d = n;
  while (k) {
    // Află poziția lui d printre elemente mai mici ca d. Aceasta ne indică
    // cifra în baza d pe nivelul d din arbore.
    int i = 0;
    while (v[i] != d) {
      if (v[i] < d) {
        k++;
      }
      i++;
    }

    // Notează cifra și urcă un nivel.
    fb[n - d] = k %  d;
    k /= d;
    d--;

    // Dacă am depășit nivelul 1 și încă avem rest, înseamnă că testul este
    // greșit: nu există K permutări la dreapta lui P.
    assert(d > 0);
  }

  // Cifrele de la 1 la d nu-și schimbă ordinea. Scoate cifrele d+1...n din
  // vector.
  int j = 0;
  for (int i = 0; i < n; i++) {
    if (v[i] <= d) {
      v[j++] = v[i];
    }
  }

  // Reinserează cifrele d+1...n în vector conform cifrelor în baza factorial.
  while (++d <= n) {
    // Vectorul are d-1 cifre. Inserează-l pe d la poziția fb[n - d].
    for (int i = d - 2; i >= fb[n - d]; i--) {
      v[i + 1] = v[i];
    }
    v[fb[n - d]] = d;
  }

  f = fopen("arbperm.out", "w");
  for (int i = 0; i < n; i++) {
    fprintf(f, "%d ", v[i]);
  }
  fprintf(f, "\n");
  fclose(f);

  return 0;
}
