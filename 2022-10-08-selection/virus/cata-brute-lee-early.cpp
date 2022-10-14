// Autor: Cătălin Frâncu
// Complexitate: O(Q N^2)
//
// Metodă: La fel cu cata-brute-lee.cpp, dar oprim algoritmul lui Lee imediat
// ce ajunge la destinație.
#include <stdio.h>

#define MAX_N 2'000

#define T_INFECT 1
#define T_TEST 2

#define ST_ALIVE 0
#define ST_DEAD 1
#define ST_FILL 2

typedef struct {
  short r, c;
} point;

char s[MAX_N + 2][MAX_N + 2]; // inițializate cu ST_ALIVE
point queue[MAX_N * MAX_N];
point dir[4] = { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 1 } };

// marchează dreptunghiul (r1,c1)-(r2,c2) ca fiind mort
void infect(int r1, int c1, int r2, int c2) {
  for (int i = r1; i <= r2; i++) {
    s[i][c1] = s[i][c2] = ST_DEAD;
  }
  for (int i = c1 + 1; i < c2; i++) {
    s[r1][i] = s[r2][i] = ST_DEAD;
  }
}

// modifică toată zona accesibilă din (r,c) din starea s1 în starea s2, ieșind
// prematur imediat ce (r2,c2) este marcat
void lee(short r, short c, int s1, int s2, int r2, int c2) {
  int head = 0, tail = 1;
  queue[0] = { r, c };
  s[r][c] = s2;

  do {
    r = queue[head].r;
    c = queue[head].c;
    for (int d = 0; d < 4; d++) {
      short r2 = r + dir[d].r, c2 = c + dir[d].c;
      if (s[r2][c2] == s1) {
        s[r2][c2] = s2;
        queue[tail++] = { r2, c2 };
      }
    }
    head++;
  } while ((head < tail) && ((r != r2) || (c != c2)));
}

// verifică dacă (r1,c1) și (r2,c2) sînt pornite și pot comunica
bool conn_test(int r1, int c1, int r2, int c2) {
  if ((s[r1][c1] == ST_DEAD) || (s[r2][c2] == ST_DEAD)) {
    return false;
  }

  lee(r1, c1, ST_ALIVE, ST_FILL, r2, c2);
  bool result = (s[r2][c2] == ST_FILL);
  lee(r1, c1, ST_FILL, ST_ALIVE, 0, 0);
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
