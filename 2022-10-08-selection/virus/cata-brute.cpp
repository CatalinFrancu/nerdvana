// Autor: Cătălin Frâncu
// Complexitate: O(Q N^2)
//
// Metodă: Pentru operații de infectare, marcăm naiv dreptunghiurile într-o
// matrice. Pentru interogări de conectivitate, rulăm algoritmul flood fill
// pentru a vedea dacă (L2,C2) poate fi atins din (L1, C1), apoi același
// algoritm pentru a restaura matricea.
#include <stdio.h>

#define MAX_N 2'000

#define T_INFECT 1
#define T_TEST 2

#define ST_ALIVE 0
#define ST_DEAD 1
#define ST_FILL 2

char s[MAX_N + 2][MAX_N + 2]; // inițializate cu ST_ALIVE

// marchează dreptunghiul (r1,c1)-(r2,c2) ca fiind mort
void infect(int r1, int c1, int r2, int c2) {
  for (int i = r1; i <= r2; i++) {
    s[i][c1] = s[i][c2] = ST_DEAD;
  }
  for (int i = c1 + 1; i < c2; i++) {
    s[r1][i] = s[r2][i] = ST_DEAD;
  }
}

// modifică toată zona accesibilă din (r,c) din starea s1 în starea s2
void flood_fill(int r, int c, int s1, int s2) {
  if (s[r][c] == s1) {
    s[r][c] = s2;
    flood_fill(r - 1, c, s1, s2);
    flood_fill(r, c + 1, s1, s2);
    flood_fill(r + 1, c, s1, s2);
    flood_fill(r, c - 1, s1, s2);
  }
}

// verifică dacă (r1,c1) și (r2,c2) sînt pornite și pot comunica
bool conn_test(int r1, int c1, int r2, int c2) {
  if ((s[r1][c1] == ST_DEAD) || (s[r2][c2] == ST_DEAD)) {
    return false;
  }

  flood_fill(r1, c1, ST_ALIVE, ST_FILL);
  bool result = (s[r2][c2] == ST_FILL);
  flood_fill(r1, c1, ST_FILL, ST_ALIVE);
  return result;
}

int main() {
  int n, q;

  FILE* fin = fopen("virus.in", "r");
  FILE* fout = fopen("virus.out", "w");
  fscanf(fin, "%d %d", &n, &q);

  // santinele în jurul matricei
  infect(0, 0, n + 1, n + 1);

  while (q--) {
    int type, r1, c1, r2, c2;
    fscanf(fin, "%d %d %d %d %d", &type, &r1, &c1, &r2, &c2);
    if (type == T_INFECT) {
      infect(r1, c1, r2, c2);
    } else {
      fprintf(fout, conn_test(r1, c1, r2, c2) ? "DA\n" : "NU\n");
    }
  }

  fclose(fin);
  fclose(fout);

  return 0;
}
