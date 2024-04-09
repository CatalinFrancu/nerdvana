// Metodă: O(n^2) pornind de la versiunile în O(n^3).
//
// Pornim de la recurența cunoscută, ușor reformulată:
//
// d[i][j] = min_{k} { max(c[k] + d[i][k-1], c[k] + d[k+1][j]) }
//
// Pentru i și j fixate, observăm că d[i][k-1] crește pe măsură ce k crește,
// iar d[k+1][j] scade. Așadar, există un k înainte de care maximul este dat
// de d[k+1][j] și după care maximul este dat de d[i][k-1].
//
// Mai mult, pentru o linie i fixată, pe măsură ce j crește, k nu poate decît
// să crească. Atunci îl putem recalcula pe k în O(1) amortizat. Odată
// calculat k, la dreapta sa dorim minimul expresiei c[l] + d[i][l-1] pentru
// k≤l≤j. Dar această expresie nu mai depinde de k! Deci orice structură de
// date folosim, putem refolosi pentru j+1, j+2 etc. Rezultă că putem folosi o
// coadă de minime: o listă de poziții crescătoare l1, l2, l3 pentru care
// valorile expresiei cresc. Operațiile pe această coadă, pentru un capăt
// stîng i fixat, sînt:
//
// * La începutul liniei i, inițializează i = j = k, iar coada de minime
//   conține c[k]+d[i][k-1] = c[k].
// * La avansarea la j-ul următor:
//   * Recalculează k.
//   * Cît timp prima poziție din coadă este < k, elimin-o (nu mai putem
//     folosi acea poziție, căci k a trecut de ea).
//   * Adaugă expresia c[j] + d[i][j-1] la sfîrșitul cozii; adăugarea ei
//     scoate din coadă valorile mai mici decît ea.
//   * Prima poziție din coadă este cea care oferă minimul la dreapta pentru
//     intervalul [i,j].
//
// Pe coloană procedăm exact la fel, adaptînd logica pentru j fixat și i
// descrescător. Diferența este că, deoarece codul iterează după linii,
// trebuie să menținem cîte o coadă pe fiecare coloană. La intervalul [i,j]
// actualizăm coada pe coloana j, apoi o punem deoparte pînă cînd avem din nou
// nevoie de ea la intervalul [i-1,j].
#include <stdio.h>

const int MAX_N = 5'000;

int n;
int c[MAX_N];
int d[MAX_N][MAX_N];

// Inserții la ambele capete, ștergeri doar în față.
struct quack {
  int *v;
  int head, tail;

  void init(int* buf) {
    v = buf;
    head = tail = 0;
  }

  int front() {
    return v[head];
  }

  int back() {
    return v[tail - 1];
  }

  void pop_front() {
    head++;
  }

  void pop_back() {
    tail--;
  }

  void push_back(int val) {
    v[tail++] = val;
  }

  bool is_empty() {
    return head == tail;
  }
};

struct row_queue {
  quack q;
  int i, k;

  void init(int* buf, int i) {
    this->i = k = i;
    q.init(buf);
    q.push_back(i);
  }

  int cost(int j) {
    return d[i][j - 1] + c[j];
  }

  int first() {
    return cost(q.front());
  }

  void advance(int j) {
    // actualizează k
    while (d[i][k - 1] < d[k + 1][j]) {
      k++;
    }
    // actualizează fața
    while (q.front() < k) {
      q.pop_front();
    }
    // actualizează spatele
    int z = cost(j);
    while (!q.is_empty() && (z < cost(q.back()))) {
      q.pop_back();
    }
    q.push_back(j);
  }
};

struct col_queue {
  quack q;
  int j, k;

  void init(int* buf, int j) {
    this->j = k = j;
    q.init(buf);
    q.push_back(j);
  }

  int cost(int i) {
    return c[i] + d[i + 1][j];
  }

  int first() {
    return cost(q.front());
  }

  void advance(int i) {
    // actualizează k
    while (d[i][k - 1] > d[k + 1][j]) {
      k--;
    }
    // actualizează fața
    while (q.front() > k) {
      q.pop_front();
    }
    // actualizează spatele
    int z = cost(i);
    while (!q.is_empty() && (z < cost(q.back()))) {
      q.pop_back();
    }
    q.push_back(i);
  }
};

int row_buf[MAX_N];
row_queue rq;
col_queue cq[MAX_N];

void read_data() {
  FILE *f = fopen("takara.in", "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &c[i]);
  }
  fclose(f);
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void compute_costs() {
  for (int i = n - 1; i >= 0; i--) {
    rq.init(row_buf, i);
    cq[i].init(d[i], i);

    for (int j = i + 1; j < n; j++) {
      rq.advance(j);
      cq[j].advance(i);
      d[i][j] = min(rq.first(), cq[j].first());
    }
  }
}

void write_result() {
  FILE* f = fopen("takara.out", "w");
  fprintf(f, "%d\n", d[0][n - 1]);
  fclose(f);
}

int main() {
  read_data();
  compute_costs();
  write_result();

  return 0;
}
