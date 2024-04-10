// V3, greoaie, dar comentată.
//
// În primul rînd, dacă tipărim cu un program șirul FFT și îl aliniem frumos,
// observăm că șirul arată identic dacă îl citim din 1 în 1 sau din s în s sau
// din s^2 în s^2 etc.
//
// Apoi, esența este că încercăm să aliniem șirul dat la grupe de mărime s,
// apoi de mărime s^2 etc. Oriunde două cifre consecutive NU cresc cu 1 modulo
// s, trebuie să existe o aliniere la o grupă de mărime s.
//
// Pentru exemplul din enunț (s=6), am reprezentat cu bare verticală
// alinierile.
//
// 2 | 4 5 0 1 2 3 | 5 0 1 2 3 4 | 0 1 2 3 4 5 | 1
//
// Rezultă că prefixul dinaintea lui 2 este 3 4 5 0 1, deci poziția șirului
// este deplasată cu 5 relativ la alinierea în grupe de 6. Echivalent,
// reprezentarea în baza 6 a poziției se termină cu 5. Conform observației
// inițiale, putem acum colecta doar prima cifră din fiecare grupă ca să
// încercăm alinierea la grupe de 36:
//
// 3 4 5 0 1
//
// Procedăm astfel și compunem poziția cifră cu cifră în baza s, cît timp
// alinierea este evidentă. În exemplul de mai sus, toate cifrele cresc cu 1
// modulo 6, deci nu mai putem deduce grupele. De aici, programul denegerează
// într-o mlaștină de cazuri particulare, obținute mecanic printr-o combinație
// de:
//
// * generator de teste mici + program forță brută
// * enumerarea exhaustivă pe hîrtie a unor cazuri (de exemplu 7, 8 și 9
//   valori modulo 5)
// * deprecații și imprecații la ființe imaginare
#include <stdio.h>

const int MAX_N = 500'000;
const int FAIL = -1;
const int UNSET = MAX_N;
const int MOD = 1'000'000'007;

int v[MAX_N];
int n, s;

void read_data(FILE* f) {
  fscanf(f, "%d %d", &n, &s);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &v[i]);
  }
}

int mod_pow(long long a, int n) {
  long long result = 1;

  while (n) {
    if (n & 1) {
      result = result * a % MOD;
    }
    a = a * a % MOD;
    n >>= 1;
  }

  return result;
}

// Cînd vectorul are lungime 2, algoritmul continuă să funcționeze, dar poate
// dura O(n^2). De exemplu, pentru s = 100 și vectorul (35 39), alinierile
// funcționează astfel:
//
// * (36 37 ... 99 0 1 ... 34) 35 | 39 --> cifra s - 1 în baza s
// * (37 38 ... 99 0 1 ... 35) 36 | 39 --> cifra s - 1 în baza s
// * (38 39 ... 99 0 1 ... 36) 37 | 39 --> cifra s - 1 în baza s
// * (39 40 ... 99 0 1 ... 37) 38 | 39 --> cifra s - 1 în baza s
// * În sfîrșit, 38 39 crește cu 1 --> cifra 38 în baza s.
//
// Rezultă o sumă de puteri, care duc la formula de jos.
int phase_length_2() {
  int exp = (s + v[1] - v[0] - 1) % s;
  long long p = mod_pow(s, exp);

  int result = (v[1] == 0)
    ? (p * (2 * s - 1) - 1) % MOD
    : (p * v[1] - 1) % MOD;

  n = 1;
  v[0] = 0;
  return result;
}

// Caută pozițiile unde știm sigur că există o aliniere. Dacă există mai
// multe, ele trebuie să fie congruente modulo s, altfel testul nu are
// soluție.
int get_alignment() {
  int al = UNSET;
  for (int i = 1; i < n; i++) {
    if ((v[i - 1] + 1) % s != v[i]) {
      if ((al != UNSET) && (al != i % s)) {
        return FAIL;
      } else {
        al = i % s;
      }
    }
  }
  return al;
}

// Verifică dacă o secvență, presupusă a crește cu 1, are două (sau mai multe)
// zerouri.
bool has_two_zeroes() {
  if (v[0] == 0) {
    return n >= s + 1;
  } else {
    return n >= 2 * s - v[0] + 1;
  }
}

// Verifică dacă o secvență, presupune a crește cu 1, trece peste 0. Dacă
// începe cu 0, acela nu se consideră.
bool passes_zero() {
  return (n >= s + 1 - v[0]);
}

// Procesează secvențele care cresc cu 1 (a căror aliniere este necunoscută).
int phase_single_run() {
  if (!passes_zero()) {
    int result = v[0];
    n = 1;
    v[0] = 0;
    return result;
  }

  if (n <= s) {
    int result = s - n;
    v[0] = (v[n - 1] + 1) % s;
    n = 1;
    return result;
  }

  if (has_two_zeroes()) {
    int result = 2 * s - n;
    v[0] = v[1] = v[n - s];
    n = 2;
    return result;
  }

  int pos_0 = s - v[0];
  v[0] = v[1] = 1;
  n = 2;
  return s - pos_0 - 1;
}

// Procesează o secvență. Îi găsește alinierea și o reduce la secvența
// obținută luînd doar primul caracter din fiecare grupă.
//
// Modifică n și vectorul v. Returnează cifra în baza s (decalajul secvenței
// față de alinierea perfectă).
int phase() {
  if (n == 2) {
    return phase_length_2();
  }

  int al = get_alignment();
  if (al == FAIL) {
    return FAIL;
  } else if (al == UNSET) {
    return phase_single_run();
  }

  int new_n = 0;
  if (al) {
    // Copiază începutul grupei incomplete.
    v[new_n++] = (v[al - 1] + 1) % s;
  }
  for (int i = al; i < n; i += s) {
    v[new_n++] = v[i];
  }
  n = new_n;

  return (s - al) % s;
}

int find_start_position() {
  if (s == 1) {
    return 0;
  }

  long long result = 0;
  long long phase_result = 0;
  long long pow = 1;

  while ((n > 1) && ((phase_result = phase()) != FAIL)) {
    // Adaugă următoarea cifră în baza z.
    result = (result + pow * phase_result) % MOD;
    pow = pow * s % MOD;
  }

  // Cînd rămîne o singură cifră, ea apare pe poziția egală cu cifra.
  return (phase_result == FAIL)
    ? FAIL
    : (result + pow * v[0]) % MOD;
}

void process_test(FILE* fin, FILE* fout) {
  read_data(fin);
  int answer = find_start_position();
  fprintf(fout, "%d\n", answer);
}

int main() {
  FILE* fin = fopen("pscfft.in", "r");
  FILE* fout = fopen("pscfft.out", "w");
  int num_tests;
  fscanf(fin, "%d", &num_tests);
  while (num_tests--) {
    process_test(fin, fout);
  }
  fclose(fin);
  fclose(fout);

  return 0;
}
