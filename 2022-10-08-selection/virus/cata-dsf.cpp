// Autor: Cătălin Frâncu
// Complexitate: O((Q + N) log* N)
//
// Metodă:
//
// 1. Numerotăm toate dreptunghiurile (infecțiile) într-o matrice, în ordine
// cronologică.
//
// 2. Rulăm algoritmul lui Lee, numerotînd zonele care rămîn pornite pînă la
// final, cu numere mai mari decît dreptunghiurile. Acum fiecare calculator
// face parte dintr-o componentă numerotată.
//
// 3. Procesăm operațiile în ordine inversă. Pentru o operație de infectare,
// „dezinfectăm” (haha) dreptunghiul, unificînd componenta acestuia cu toate
// componentele cu număr mai mare adiacente pe interior sau pe
// exterior. Pentru o operație de interogare, verificăm dacă cele două
// calculatoare sînt pornite și fac parte din aceeași componentă. Notăm
// răspunsurile la aceste verificări.
//
// 4. La final, tipărim răspunsurile în ordinea corectă.
//
// Putem gestiona unificarea și testarea componentelor cu o structură de
// mulțimi disjuncte.
#include <stdio.h>

#define MAX_N 2'000
#define MAX_Q 100'000

#define COMP_NEW 0
#define COMP_WALL 1
#define COMP_INFECT 2 // de aici începînd numerotăm componentele

#define T_INFECT 1
#define T_TEST 2

typedef struct {
  short r1, c1, r2, c2;
  char type;
  bool answer;
} query;

int s[MAX_N + 2][MAX_N + 2]; // inițializate cu COMP_NEW
query q[MAX_Q];
int n;

///////////////////////////////////////////////////////////////////////////
//                     structura de mulțimi disjuncte                    //
///////////////////////////////////////////////////////////////////////////

int parent[MAX_N * MAX_N];

void set_make(int n) {
  for (int i = 0; i <= n; i++) {
    parent[i] = i;
  }
}

int set_find(int x) {
  int r = x;
  while (parent[r] != r) {
    r = parent[r];
  }

  // comprimarea căii
  while (parent[x] != r) {
    int p = parent[x];
    parent[x] = r;
    x = p;
  }

  return r;
}

void set_union(int x, int y) {
  x = set_find(x);
  y = set_find(y);

  if (x != y) {
    parent[y] = x;
  }
}

///////////////////////////////////////////////////////////////////////////
//                           algoritmul lui Lee                          //
///////////////////////////////////////////////////////////////////////////

typedef struct {
  short r, c;
} point;

point queue[MAX_N * MAX_N];
point dir[4] = { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 1 } };

void lee(short r, short c, int comp) {
  int head = 0, tail = 1;
  queue[0] = { r, c };
  s[r][c] = comp;

  while (head < tail) {
    r = queue[head].r;
    c = queue[head].c;
    for (int d = 0; d < 4; d++) {
      short r2 = r + dir[d].r, c2 = c + dir[d].c;
      if (s[r2][c2] == COMP_NEW) {
        s[r2][c2] = comp;
        queue[tail++] = { r2, c2 };
      }
    }
    head++;
  }
}

///////////////////////////////////////////////////////////////////////////
//                        problema propriu-zisă                          //
///////////////////////////////////////////////////////////////////////////

void infect(query q, int comp) {
  for (int i = q.r1; i <= q.r2; i++) {
    s[i][q.c1] = s[i][q.c2] = comp;
  }
  for (int i = q.c1 + 1; i < q.c2; i++) {
    s[q.r1][i] = s[q.r2][i] = comp;
  }
}

// unifică cu comp vecinii lui (r,c) mai mari decît comp
void disinfect_element(int r, int c, int comp) {
  for (int d = 0; d < 4; d++) {
    int r2 = r + dir[d].r, c2 = c + dir[d].c;
    if (set_find(s[r2][c2]) > comp) {
      set_union(comp, s[r2][c2]);
    }
  }
}

// opusul lui infect() este disinfect(), nu?
void disinfect(query q) {
  int me = s[q.r1][q.c1];

  for (int i = q.r1; i <= q.r2; i++) {
    disinfect_element(i, q.c1, me);
    disinfect_element(i, q.c2, me);
  }

  for (int i = q.c1 + 1; i < q.c2; i++) {
    disinfect_element(q.r1, i, me);
    disinfect_element(q.r2, i, me);
  }
}

// verifică dacă (r1,c1) pot comunica
bool conn_test(query q, int last_infect_comp) {
  // punctele trebuie să fie în aceeași componentă, care să nu fie printre
  // dreptunghiurile infectate
  int comp1 = set_find(s[q.r1][q.c1]);
  int comp2 = set_find(s[q.r2][q.c2]);
  return ((comp1 == comp2) && (comp1 > last_infect_comp));
}

int main() {
  int num_q;

  FILE* f = fopen("virus.in", "r");
  fscanf(f, "%d %d", &n, &num_q);

  // santinele în jurul matricei
  infect({ 0, 0, (short)(n + 1), (short)(n + 1) }, COMP_WALL);

  // citește operațiile și numerotează infecțiile
  int comp = COMP_INFECT - 1;
  for (int i = 0; i < num_q; i++) {
    fscanf(f, "%hhd %hd %hd %hd %hd",
          &q[i].type, &q[i].r1, &q[i].c1, &q[i].r2, &q[i].c2);
    if (q[i].type == T_INFECT) {
      infect(q[i], ++comp);
    }
  }
  fclose(f);
  int last_infect_comp = comp;

  // rulează algoritmul lui lee pentru a numerota zonele încă pornite
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= n; j++) {
      if (s[i][j] == COMP_NEW) {
        lee(i, j, ++comp);
      }
    }
  }

  set_make(comp);

  // unifică/interoghează componentele în ordine inversă
  for (int i = num_q - 1; i >= 0; i--) {
    if (q[i].type == T_TEST) {
      q[i].answer = conn_test(q[i], last_infect_comp);
    } else {
      disinfect(q[i]);
      last_infect_comp--;
    }
  }

  // tipărește răspunsurile
  f = fopen("virus.out", "w");
  for (int i = 0; i < num_q; i++) {
    if (q[i].type == T_TEST) {
      fprintf(f, q[i].answer ? "DA\n" : "NU\n");
    }
  }
  fclose(f);

  return 0;
}
