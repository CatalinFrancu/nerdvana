// Autor: Cătălin Frâncu
// Complexitate: O((Q + N) sqrt(N))
//
// Metodă: Descompunere în radical. Împărțim vectorul în NB bucăți fiecare de
// mărime BS (în practică, NB = BS = sqrt(N)). Pentru fiecare bucată b reținem
// (1) b[x] = numărul de biți 1 din acea bucată și
// (2) neg[x] = dacă bucata în întregime este negată.
//
// Pentru interogări, adunăm pentru bucățile x incluse complet în interval fie
// b[x], fie BS - b[x], după cum neg[x] = false sau true. Apoi adunăm naiv
// biții din fragmentele rămase la capete, de asemenea negați dacă este cazul
// (poate fi și un singur fragment dacă întreaga interogare este într-o
// singură bucată).
//
// Pentru actualizări, negăm pur și simplu neg[x] pentru toate bucățile x
// incluse complet în interval. La capetele stînga și dreapta negăm naiv biții
// și actualizăm b[stînga] și b[dreapta].
#include <math.h>
#include <stdio.h>

#define MAX_BUCKETS 548 // ceil(sqrt(300'000))
#define MAX_N (MAX_BUCKETS * MAX_BUCKETS)

#define T_TOGGLE 1
#define T_COUNT 2

char v[MAX_N];
int n, nb, bs;         // numărul de bucăți și mărimea lor
int b[MAX_BUCKETS];    // numărul de biți 1 din fiecare bucată
bool neg[MAX_BUCKETS]; // true dacă bucata este negată

// calculează suma intervalului [l, r), complet inclus în bucket
int fragment_count(int l, int r, int bucket) {
  int sum = 0;
  while (l < r) {
    sum += v[l++] ^ neg[bucket]; // neagă dacă este cazul
  }
  return sum;
}

// calculează suma intervalului [l, r)
int range_count(int l, int r) {
  int bl = l / bs, br = r / bs;
  if (bl == br) {
    return fragment_count(l, r, bl);
  } else {
    // capetele
    int sum = fragment_count(l, (bl + 1) * bs, bl);
    sum += fragment_count(br * bs, r, br);

    // bucățile incluse complet
    for (int i = bl + 1; i < br; i++) {
      sum += neg[i] ? (bs - b[i]) : b[i];
    }
    return sum;
  }
}

// neagă valorile pe intervalul [l, r), complet inclus în bucket
void fragment_toggle(int l, int r, int bucket) {
  // notă: b[] se referă la valorile efective din v[], deci neg[] nu intervine
  while (l < r) {
    v[l] = !v[l];
    b[bucket] += v[l] ? +1 : -1;
    l++;
  }
}

// neagă valorile din intervalul [l, r)
void range_toggle(int l, int r) {
  int bl = l / bs, br = r / bs;
  if (bl == br) {
    fragment_toggle(l, r, bl);
  } else {
    // capetele
    fragment_toggle(l, (bl + 1) * bs, bl);
    fragment_toggle(br * bs, r, br);

    // bucățile incluse complet
    for (int i = bl + 1; i < br; i++) {
      neg[i] = !neg[i];
    }
  }
}

int main() {
  int q, t, l, r;

  // citire
  FILE* fin = fopen("shperl.in", "r");
  FILE* fout = fopen("shperl.out", "w");
  fscanf(fin, "%d %d ", &n, &q);
  nb = sqrt(n);
  bs = (n - 1) / nb + 1;

  fread(v, 1, n, fin);
  for (int i = 0; i < n; i++) {
    v[i] -= '0';
  }

  // inițializează totalurile pe fiecare bucată
  for (int i = 0; i < nb; i++) {
    int bucket_start = i * bs;
    for (int j = 0; j < bs; j++) {
      b[i] += v[j + bucket_start];
    }
  }

  while (q--) {
    fscanf(fin, "%d %d %d", &t, &l, &r);
    l--; // vom lucra pe intervalul [l,r), indexat de la 0
    if (t == T_TOGGLE) {
      range_toggle(l, r);
    } else {
      fprintf(fout, "%d\n", range_count(l, r));
    }
  }

  fclose(fin);
  fclose(fout);

  return 0;
}
