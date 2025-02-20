// Autor: Cătălin Frâncu
// Complexitate: O(N * min(N, log_N(K)))
//
// Metodă: Socotim rangurile de la dreapta la stînga (deci noile cifre sînt
// inserate pe poziții de la dreapta spre stînga). Atunci vrem să calculăm
// permutarea de rang cu k mai mic.
//
// Scoatem elemente din permutare, începînd cu n, și calculăm rangul.
// Elementul n va contribui cu [0...n-1], elementul n va contribui cu
// [0...n-2]×(n-1) etc. Cînd ajungem la un rang ≥ k, ne putem opri. Apoi
// scădem k și reinserăm elementele conform rangului obținut.
//
// Nu avem nevoie de structuri de date. Pentru n mic, O(n²) este suficient,
// iar pentru n mare vom avea de scos doar cîteva elemente.
//
// Later edit: acest algoritm eșuează din două motive pe cazul de mai jos: (1)
// Chiar există n elemente care migrează și (2) produsul elementelor eliminate
// depășește reprezentarea.
//
// 100000 1
// 2 1 3 4 5 6 7 ... 100000
#include <stdio.h>

const int MAX_N = 100'000;

int v[MAX_N], n;
long long k;
long long prod; // produsul elementelor eliminate

void read_data() {
  FILE* f = fopen("arbperm.in", "r");
  fscanf(f, "%d %lld", &n, &k);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &v[i]);
  }
  fclose(f);
}

// Elimină-l pe n din vector. Returnează poziția lui n de la dreapta.
int remove() {
  int pos = 0;
  while (v[pos] != n) {
    pos++;
  }

  for (int i = pos; i < n - 1; i++) {
    v[i] = v[i + 1];
  }

  return n - pos - 1;
}

// Elimină elemente pînă obține un rang ≥ k. Recalculează n, k și prod.
void remove_elements() {
  k = -k;
  prod = 1;

  while (k < 0) {
    k += prod * remove();
    prod *= n--;
  }
}

// Inserează-l pe n într-o permutare de n-1 elemente.
void insert(int pos_from_right) {
  int i;
  for (i = n - 1; i >= n - pos_from_right; i--) {
    v[i] = v[i - 1];
  }
  v[i] = n;
}

// Inserează elementele eliminate anterior, conform cu rangul k.
void insert_elements() {
  while (prod > 1) {
    n++;
    prod /= n;
    insert(k / prod);
    k %= prod;
  }
}

void write_answer() {
  FILE* f = fopen("arbperm.out", "w");
  for (int i = 0; i < n; i++) {
    fprintf(f, "%d ", v[i]);
  }
  fprintf(f, "\n");
  fclose(f);
}

int main() {
  read_data();
  remove_elements();
  insert_elements();
  write_answer();

  return 0;
}
