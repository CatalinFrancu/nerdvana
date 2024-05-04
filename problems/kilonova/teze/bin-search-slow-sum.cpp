#include <stdio.h>

const int MAX_Q = 1'000;
const int MOD = 1'000'000'007;

typedef unsigned long long u64;

unsigned n;
int m, k, t1, q;
u64 p;
int d[MAX_Q];
int part_d[MAX_Q + 1]; // suma primelor i valori, deci part_d[2] = d[0] + d[1]
int sum_d;

void read_data() {
  FILE* f = fopen("teze.in", "r");
  fscanf(f, "%d %d %llu %d %d %d", &n, &m, &p, &k, &t1, &q);
  for (int i = 0; i < q; i++) {
    fscanf(f, "%d", &d[(2 * q - 2 + i) % q]); // pune d_2 pe prima poziție
  }
  fclose(f);
}

void preprocess_d() {
  for (int i = 0; i < q; i++) {
    sum_d += d[i];
    part_d[i + 1] = part_d[i] + d[i];
  }
}

u64 t(u64 i) {
  // Adaugă i - 1 termeni din d
  return (u64)t1 + (i - 1) / q * sum_d + part_d[(i - 1) % q];
}

// Cost parțial, fără componentele care țin de m și k.
u64 pcost(unsigned f) {
  u64 result = p;
  for (int i = n / f; i; i--) {
    result += t(i);
  }

  return result * f + (u64)(n % f) * t(n / f + 1);
}

u64 cost(int f) {
  return (pcost(f) + (u64)k * n) % MOD * m % MOD;
}

u64 bin_search_phases() {
  // Contract: căutăm ultima scădere a funcției de cost al unei faze. Mai
  // exact, căutăm ultima poziție x unde pcost(x) < pcost(x - 1). Această
  // valoare este cuprinsă între 1 și n inclusiv (1 înseamnă, implicit, că
  // costul pentru o fază este minimul din toate fazele).
  //
  // lo este o valoare cunoscută a fi bună, iar hi este o valoare cunoscută a
  // fi proastă.
  //
  // Există UN SINGUR MOD de a scrie o căutare binară sau ternară. Orice altă
  // invenție este neclară și posibil incorectă.
  unsigned lo = 1, hi = n + 1; // [lo, hi)
  while (hi - lo > 1) {
    unsigned mid = (lo + hi) / 2;
    if (pcost(mid) < pcost(mid - 1)) {
      lo = mid;
    } else {
      hi = mid;
    }
  }

  return cost(lo);
}

void write_answer(u64 min_cost) {
  FILE* f = fopen("teze.out", "w");
  fprintf(f, "%llu\n", min_cost);
  fclose(f);
}

int main() {
  read_data();
  preprocess_d();
  u64 min_cost = bin_search_phases();
  write_answer(min_cost);

  return 0;
}
