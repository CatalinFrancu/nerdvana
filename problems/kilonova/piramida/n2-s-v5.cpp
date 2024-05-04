// Complexitate: O(n^2 |s|).
//
// Pentru fiecare punct (r,c) din matrice ne întrebăm: există vreo cale de
// mărime k*|s| care aunge aici? În practică operăm pe șirul s răsturnat și
// căutăm căi de lungime k*|s| care pornesc din (r,c).
//
// Graful în care testăm existența acestei căi are N*M*|s| noduri. De la
// (r,c,t) există o muchie la (r',c',t+1) dacă (r,c) și (r',c') sînt vecine și
// mat[r][c] = s[t] și mat[r'][c'] = s[t + 1]. Existența muchiilor este
// periodică modulo |s|.
//
// Pentru a afla lungimea căilor care pornesc din fiecare nod, este suficientă
// o parcurgere DFS. Dacă un nod este pe un ciclu sau are o cale către un
// ciclu, atunci există căi de lungime infinită care pornesc din el; în caz
// contrar, lungimea căii maxime care pornește din nod este 1 + lungimea
// căilor care pornesc din descendenți.
//
// Programul folosește stivă proprie, deoarece recursivitatea poate ajunge la
// N^2 * |s| niveluri. Ca optimizare, indexarea grafului în ordinea (rînd,
// coloană, timp) este de 3 ori mai rapidă decît în ordinea (timp, rînd,
// coloană) datorită paginării mai bune în cache.
//
// Autor: Cătălin Frâncu
#include <algorithm>
#include <stdio.h>
#include <string.h>

const int MAX_ROWS = 200;
const int MAX_S = 200;

const int NUM_DIR = 4;
const int DIR[NUM_DIR][2] = { { -1, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 } };

const int WHITE = 0;
const int GRAY = 1;
const int BLACK = 2;

const int INFINITY = 1e9;

typedef unsigned char byte;

struct solution {
  int dist;
  int count;
};

struct level {
  byte p, r, c, dir;
};

char mat[MAX_ROWS + 2][MAX_ROWS + 2];
char s[MAX_S + 1];
byte color[MAX_ROWS + 2][MAX_ROWS + 2][MAX_S];
int dist[MAX_ROWS + 2][MAX_ROWS + 2][MAX_S];
level st[MAX_S * MAX_ROWS * MAX_ROWS];
solution sol[MAX_ROWS * MAX_ROWS];
int nr, nc, lens; // num_rows, num_columns, |s|
int num_sol;

void read_data(FILE* f) {
  fscanf(f, "%d %d ", &nr, &nc);
  for (int r = 1; r <= nr; r++) {
    fscanf(f, "%s ", &mat[r][1]);
  }
  fscanf(f, "%s", s);
}

void prepare_s() {
  lens = strlen(s);

  // răstoarnă s
  for (int i = 0, j = lens - 1; i < j; i++, j--) {
    char c = s[i]; s[i] = s[j]; s[j] = c;
  }
}

void improve(int& x, int y) {
  if (y > x) {
    x = y;
  }
}

void dfs(byte p, byte r, byte c) {
  int ptr = 0;
  st[0] = { p, r, c, 0 };

  while (ptr >= 0) {
    level& l = st[ptr];
    int& my_dist = dist[l.r][l.c][l.p];
    byte& my_color = color[l.r][l.c][l.p];
    if (l.dir == NUM_DIR) {
      if (my_dist != INFINITY) {
        my_dist++;
      }
      my_color = BLACK;
      if (ptr) {
        level& par = st[ptr - 1];
        improve(dist[par.r][par.c][par.p], my_dist);
      }
      ptr--;
    } else {
      my_color = GRAY;
      byte q = (l.p + 1) % lens;
      byte r2 = l.r + DIR[l.dir][0];
      byte c2 = l.c + DIR[l.dir][1];
      if (mat[r2][c2] == s[q]) {
        switch (color[r2][c2][q]) {
          case WHITE:
            st[++ptr] = { q, r2, c2, 0 };
            break;
          case GRAY:
            dist[r2][c2][q] = my_dist = INFINITY;
            break;
          case BLACK:
            improve(my_dist, dist[r2][c2][q]);
            break;
        }
      }
      l.dir++;
    }
  }
}

void compute_distances() {
  for (int r = 1; r <= nr; r++) {
    for (int c = 1; c <= nc; c++) {
      if (mat[r][c] == s[0]) {
        dfs(0, r, c);
      }
    }
  }
}

void count_distances() {
  // colectează
  num_sol = 0;
  for (int r = 1; r <= nr; r++) {
    for (int c = 1; c <= nc; c++) {
      sol[num_sol++] = { .dist = dist[r][c][0], .count = 1 };
    }
  }

  // santinelă
  sol[num_sol++] = { .dist = INFINITY, .count = 0 };

  // sortează
  std::sort(sol, sol + num_sol, [](solution a, solution b) {
    return a.dist < b.dist;
  });

  // comasează distanțele egale
  int i = 1;
  for (int j = 1; j < num_sol; j++) {
    if (sol[i - 1].dist == sol[j].dist) {
      sol[i - 1].count += sol[j].count;
    } else {
      sol[i++] = sol[j];
    }
  }
  num_sol = i;

  // sume parțiale
  for (int i = num_sol - 2; i >= 0; i--) {
    sol[i].count += sol[i + 1].count;
  }
}

int bin_search_sol(long long dist) {
  int lo = -1, hi = num_sol - 1; // (lo, hi]
  while (hi - lo > 1) {
    int mid = (lo + hi) / 2;
    if (sol[mid].dist < dist) {
      lo = mid;
    } else {
      hi = mid;
    }
  }
  return hi;
}

void answer_queries(FILE* fin) {
  FILE* fout = fopen("piramida.out", "w");
  int k, num_k;

  fscanf(fin, "%d", &num_k);
  while (num_k--) {
    fscanf(fin, "%d", &k);
    int pos = bin_search_sol((long long)k * lens);
    fprintf(fout, "%d ", sol[pos].count);
  }

  fprintf(fout, "\n");
  fclose(fout);
}

int main() {
  FILE* fin = fopen("piramida.in", "r");

  read_data(fin);
  prepare_s();
  compute_distances();
  count_distances();
  answer_queries(fin);

  fclose(fin);

  return 0;
}
