#include <algorithm>
#include <stdio.h>

const int MAX_N = 1 << 18; // 256 k

struct hinge {
  int r, pos;
};

int min(int x, int y) {
  return (x < y) ? x : y;
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

int next_power_of_2(int n) {
  while (n & (n - 1)) {
    n += (n & -n);
  }

  return n;
}

// Arbore de segmente cu operațiile:
//
// 1. update: inc(left, right) -- incrementează pozițiile [left, right]
// 2. query: max(left, right) -- returnează maximul din [left, right]
//
// Invariant:
//
// * lazy_sum este valoarea de adăugat pe fiecare nod din subarbore
// * m este maximul real din subarbore, inclusiv lazy_sum
struct segment_tree {
  int m[2 * MAX_N];
  int lazy_sum[2 * MAX_N];
  int n;

  void init(int n) {
    this->n = next_power_of_2(n);
  }

  void push(int t) {
    lazy_sum[2 * t] += lazy_sum[t];
    m[2 * t] += lazy_sum[t];
    lazy_sum[2 * t + 1] += lazy_sum[t];
    m[2 * t + 1] += lazy_sum[t];
    lazy_sum[t] = 0;
  }

  void pull(int t) {
    m[t] = ::max(m[2 * t], m[2 * t + 1]);
  }

  void inc_helper(int t, int pl, int pr, int l, int r) {
    if (l >= r) {
      return;
    } else if ((l == pl) && (r == pr)) {
      lazy_sum[t]++;
      m[t]++;
    } else {
      push(t);
      int mid = (pl + pr) >> 1;
      inc_helper(2 * t, pl, mid, l, min(r, mid));
      inc_helper(2 * t + 1, mid, pr, ::max(l, mid), r);
      pull(t);
    }
  }

  void inc(int left, int right) {
    inc_helper(1, 0, n, left, right + 1);
  }

  int max_helper(int t, int pl, int pr, int l, int r) {
    if (l >= r) {
      return 0;
    } else if ((l == pl) && (r == pr)) {
      return m[t];
    } else {
      push(t);
      int mid = (pl + pr) >> 1;
      return ::max(max_helper(2 * t, pl, mid, l, min(r, mid)),
                   max_helper(2 * t + 1, mid, pr, ::max(l, mid), r));
    }
  }

  int max(int left, int right) {
    return max_helper(1, 0, n, left, right + 1 );
  }
};

hinge h[MAX_N];
int sol[MAX_N];
segment_tree st;
int n, k;

void read_data() {
  FILE* f = fopen("balama.in", "r");
  fscanf(f, "%d %d", &n, &k);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &h[i].r);
    h[i].pos = i;
  }
  fclose(f);
}

void sort_by_r() {
  std::sort(h, h + n, [](hinge a, hinge b) {
    return a.r > b.r;
  });
}

void scan_hinges() {
  int next_to_fill = 0;
  int i = 0;

  st.init(n);

  while (next_to_fill < k) {
    int left = max(h[i].pos - k + 1, 0);
    int right = min(h[i].pos, n - k);
    int m = st.max(left, right);
    st.inc(left, right);
    if (m == next_to_fill) {
      sol[next_to_fill++] = h[i].r;
    }
    i++;
  }
}

void write_answers() {
  FILE* f = fopen("balama.out", "w");
  for (int i = k - 1; i >= 0; i--) {
    fprintf(f, "%d ", sol[i]);
  }
  fprintf(f, "\n");
  fclose(f);
}

int main() {
  read_data();
  sort_by_r();
  scan_hinges();
  write_answers();

  return 0;
}

// Metodă: Considerăm elementele în ordine descrescătoare. După ce procesăm un
// element, îl bifăm ca văzut în cele (cel mult) K ferestre care îl conțin. De
// exemplu, pentru N=100, K=10, pos=30, vom incrementa pozițiile 21...30
// pentru a nota că ferestrele care încep pe acele poziții au încă un element
// procesat.
//
// Primul element (maximul, fie el X) este singurul element din toate
// ferestrele care îl conțin. Deci X nu poate fi altceva decît primul
// maxim. Îl notăm în soluție la poziția sol[0] (la afișare vom răsurna
// vectorul soluție).
//
// Dacă al doilea maxim, fie el Y, este departe de X (mai departe de K
// elemente), atunci Y nu poate fi decît primul maxim din toate ferestrele
// care îl conțin. Iar asta nu ne mai interesează, căci avem deja o valoare
// mai mare la sol[0]. În schimb, dacă Y este aproape de X, unele dintre
// ferestrele care îl conțin pe Y vor avea populație 1, deci notăm sol[1] = Y.
//
// În general, cînd procesăm un element Z vedem care este populația maximă din
// toate ferestrele care îl conțin pe Z, fie această populație P. Dacă la
// sol[P] nu avem încă o valoare notată, notăm sol[P]=Z. Remarcăm că valorile
// din vectorul de populație cresc cu cîte 1, deci vom completa valorile din
// sol[] în ordine. Ne putem opri cînd am completat sol[K-1].
