// Autor: Cătălin Frâncu
// Complexitate: O((Q + N) log N)
//
// Metodă: Arbore de segmente cu actualizare lazy. În fiecare nod u
// (corespunzător unui interval de lungime 2^k) stocăm suma acelui interval și
// un bit suplimentar. Suma este mereu corectă după fiecare actualizare, dar,
// cînd bitul este setat, înseamnă că actualizarea nu a fost propagată și la
// descendenți. Cînd devine necesar (la spargerea în două a unei interogări
// sau a unei actualizări), dacă în nodul curent bitul este setat, îl propagăm
// la fii (vezi funcția st_push()).
#include <stdio.h>

#define MAX_N (1 << 19)

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define T_TOGGLE 1
#define T_COUNT 2

int v[2 * MAX_N];
bool neg[2 * MAX_N];
char buf[MAX_N + 1];
int n;

void st_build() {
  for (int i = n - 1; i >= 1; i--) {
    v[i] = v[2 * i] + v[2 * i + 1];
  }
}

// Propagă informația din bitul neg la fii.
inline void st_push(int node, int child_size) {
  if (neg[node]) {
    neg[2 * node] ^= true;
    v[2 * node] = child_size - v[2 * node];
    neg[2 * node + 1] ^= true;
    v[2 * node + 1] = child_size - v[2 * node + 1];
    neg[node] = 0;
  }
}

// Neagă intervalul [l, r), acoperit complet de nodul node, care corespunde
// intervalului [pl, pr) de lungime putere a lui 2.
void st_update(int node, int pl, int pr, int l, int r) {
  if (l >= r) {
    return;
  } else if ((l == pl) && (r == pr)) {
    neg[node] = !neg[node];
    v[node] = pr - pl - v[node];
  } else {

    st_push(node, (pr - pl) >> 1);
    int mid = (pl + pr) >> 1;
    st_update(2 * node, pl, mid, l, MIN(r, mid));
    st_update(2 * node + 1, mid, pr, MAX(l, mid), r);
    v[node] = v[2 * node] + v[2 * node + 1];

  }
}

// Calculează suma pe intervalul [l, r).
int st_query(int node, int pl, int pr, int l, int r) {
  if (l >= r) {
    return 0;
  } else if ((l == pl) && (r == pr)) {
    return v[node];
  } else {

    st_push(node, (pr - pl) >> 1);
    int mid = (pl + pr) >> 1;
    return
      st_query(2 * node, pl, mid, l, MIN(r, mid)) +
      st_query(2 * node + 1, mid, pr, MAX(l, mid), r);

  }
}

int main() {
  int orig_n, q, t, l, r;

  FILE* fin = fopen("shperl.in", "r");
  FILE* fout = fopen("shperl.out", "w");
  fscanf(fin, "%d %d ", &orig_n, &q);

  // rotunjește n la următoarea putere a lui 2
  n = orig_n;
  while (n & (n - 1)) {
    n = (n | (n - 1)) + 1;
  }

  fread(buf, 1, orig_n, fin);
  for (int i = 0; i < orig_n; i++) {
    v[n + i] = buf[i] - '0';
  }

  st_build();

  while (q--) {
    fscanf(fin, "%d %d %d", &t, &l, &r);
    l--; // vom lucra pe intervalul [l,r), indexat de la 0
    if (t == T_TOGGLE) {
      st_update(1, 0, n, l, r);
    } else {
      fprintf(fout, "%d\n", st_query(1, 0, n, l, r));
    }
  }

  fclose(fin);
  fclose(fout);

  return 0;
}
