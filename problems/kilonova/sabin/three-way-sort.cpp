// Metodă: three way radix quicksort.
//
// O primă observație este că putem accelera calcularea similitudinii
// interclasînd șirurile. De exemplu, din abc def ghi obținem adgbehcfi. Cu
// această organizare, dacă pentru două seturi de șiruri dorim similitudinea
// X, atunci dorim ca șirurile interclasate să aibă un prefix comun în
// intervalul [K*X, K*(X+1)) (închis-deschis).
//
// Vom calcula, pentru fiecare interogare, numărul de șiruri cu care ea are un
// prefix comun de cel puțin K*X, respectiv de cel puțin K*(X+1). Răspunsul la
// interogare este diferența acestor două valori.
//
// Putem rezolva problema cu o singură sortare. Construim cele N+Q șiruri
// interclasate pentru raftul de sus și pentru interogări, toate de lungime
// K*P. Mărimea totală a datelor de sortat este 2 * 20.000 * 15 * 30 = 18 MB.
//
// Să considerăm un moment al sortării cînd procesăm șirurile [st, dr), care
// sînt cunoscute ca fiind egale pe primele l poziții, iar acum considerăm
// poziția l pe toate aceste șiruri. Dintre șiruri, unele sînt interogări, iar
// altele nu. Pentru fiecare interogare ne întrebăm: oare prefixul l este
// exact egal cu K*X? Dacă da, atunci notăm pe această interogare numărul de
// șiruri. Similar și pentru testul l == K*(X+1).
//
// Există un mic detaliu care cere atenție. Este posibil ca interogarea i să
// fie evaluată la lungimea l mai mult de o dată (pînă cînd ea pică în tranșa
// din mijloc la partiționare și urcă la lungimea l+1). Este important să nu-i
// calculăm răspunsurile decît prima dată la lungimea l.
#include <stdio.h>

const int MAX_STRINGS = 40'000; // inclusiv interogările
const int MAX_QUERIES = 20'000;
const int MAX_LEN = 15 * 30;
const int MAX_BIG_SHELF = 30'000 * 30;

struct string {
  char* p;
  short qindex;
};

struct query {
  int lo_prefix, hi_prefix;
  short lo_count, hi_count;
};

string s[MAX_STRINGS];
query q[MAX_QUERIES + 1];
char buf[MAX_STRINGS][MAX_LEN + 1];
char big_shelf[MAX_BIG_SHELF + 1];
int n, k, p, num_queries;

// Interclasează cele KxP litere.
void read_string(int ind, FILE* f) {
  s[ind].p = buf[ind];
  for (int i = 0; i < k; i++) {
    for (int j = 0; j < p; j++) {
      s[ind].p[k * j + i] = fgetc(f);
    }
    fgetc(f);
  }
}

// Construiește șirul explicit pentru interogare și pune-l laolaltă cu
// șirurile inițiale.
void read_query(int ind, FILE* f) {
  int score;
  fscanf(f, "%d", &score);
  q[ind].lo_prefix = k * score;
  q[ind].hi_prefix = k * (score + 1);

  int pos = n + ind - 1;
  s[pos].p = buf[pos];
  s[pos].qindex = ind;
  for (int i = 0; i < k; i++) {
    int book;
    fscanf(f, "%d", &book);
    book--;
    for (int j = 0; j < p; j++) {
      s[pos].p[k * j + i] = big_shelf[p * book + j];
    }
  }
}

void read_data() {
  int m;
  FILE* f = fopen("sabin.in", "r");
  fscanf(f, "%d %d %d %d %d ", &n, &k, &m, &p, &num_queries);
  for (int i = 0; i < n; i++) {
    read_string(i, f);
  }
  for (int i = 0; i < m; i++) {
    fscanf(f, "%s ", big_shelf + i * p);
  }
  for (int i = 1; i <= num_queries; i++) {
    read_query(i, f);
  }
  fclose(f);
}

void swap(int i, int j) {
  string tmp = s[i];
  s[i] = s[j];
  s[j] = tmp;
}

// Date fiind șirurile [first...last), avînd prefixul comun prefix_len,
// răspunde la interogările pentru care acest prefix este de interes.
void answer_queries(int first, int last, int prefix_len) {
  int num_strings = 0;
  for (int i = first; i < last; i++) {
    num_strings += (s[i].qindex == 0);
  }

  for (int i = first; i < last; i++) {
    int ind = s[i].qindex;
    if (ind) {
      if (q[ind].lo_prefix == prefix_len) {
        q[ind].lo_count = num_strings;
        q[ind].lo_prefix = -1; // nu mai întreba niciodată despre acest prefix
      } else if (q[ind].hi_prefix == prefix_len) {
        q[ind].hi_count = num_strings;
        q[ind].hi_prefix = -1;
      }
    }
  }
}

void tsort(int first, int last, int pos) {
  if (last - first <= 1) {
    return;
  }

  answer_queries(first, last, pos);

  int i = first, left = first, right = last;
  char pivot = s[first].p[pos];

  while (i < right) {
    char x = s[i].p[pos];
    if (x < pivot) {
      swap(i++, left++);
    } else if (x > pivot)  {
      swap(i, --right);
    } else {
      i++;
    }
  }

  tsort(first, left, pos);
  if (s[left].p[pos] != '\0') {
    tsort(left, right, pos + 1);
  }
  tsort(right, last, pos);
}

void write_answers() {
  FILE* f = fopen("sabin.out", "w");
  for (int i = 1; i <= num_queries; i++) {
    fprintf(f, "%d\n", q[i].lo_count - q[i].hi_count);
  }
  fclose(f);
}

int main() {
  read_data();
  tsort(0, n + num_queries, 0);
  write_answers();

  return 0;
}
