// Complexitate: O((n + q) sqrt(n)) cu algoritmul lui Mo.
#include <algorithm>
#include <math.h>
#include <stdio.h>

const int MAX_N = 300000;
const int MAX_Q = 300000;

struct query {
  int x, y;
  int orig;
};

int p[MAX_N + 1], inv[MAX_N + 1];
int other_end[MAX_N + 1]; // extremele fiecărui ciclu pointează una la alta
bool seen[MAX_N + 1];
query q[MAX_Q];
int answer[MAX_Q];
int n, root, num_queries;

void read_data() {
  FILE* f = fopen("perm.in", "r");
  fscanf(f, "%d %d", &n, &num_queries);
  root = sqrt(n);

  for (int i = 1; i <= n; i++) {
    fscanf(f, "%d", &p[i]);
  }

  for (int i = 0; i < num_queries; i++) {
    fscanf(f, "%d %d", &q[i].x, &q[i].y);
    q[i].orig = i;
  }

  fclose(f);
}

void compute_inv() {
  for (int i = 1; i <= n; i++) {
    inv[p[i]] = i;
  }
}

void sort_queries() {
  std::sort(q, q + num_queries, [](query a, query b) {
    int bucket_a = a.x / sqrt(n);
    int bucket_b = b.x / sqrt(n);
    if (bucket_a != bucket_b) {
      return bucket_a < bucket_b;
    }
    return (bucket_a & 1)
      ? (a.y > b.y)
      : (a.y < b.y);
  });
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

void traverse_cycles() {
  for (int i = 1; i <= n; i++) {
    if (!seen[i]) {
      int first = i, last = i;
      int j = i;

      while (!seen[j]) {
        seen[j] = true;
        first = min(first, j);
        last = max(last, j);
        j = p[j];
      }

      other_end[first] = last;
      other_end[last] = first;
    }
  }
}

void include(int x, int y, int& inner, int& cycles, int new_pos) {
  if ((inv[new_pos] >= x) && (inv[new_pos] <= y)) {
    inner++;
  }
  if (((p[new_pos] >= x) && (p[new_pos] <= y)) ||
      (p[new_pos] == new_pos)) {
    inner++;
  }
  if (((other_end[new_pos] >= x) && (other_end[new_pos] <= y)) ||
      (other_end[new_pos] == new_pos)) {
    cycles++;
  }
}

void exclude(int x, int y, int& inner, int& cycles, int old_pos) {
  if ((inv[old_pos] >= x) && (inv[old_pos] <= y)) {
    inner--;
  }
  if (((p[old_pos] >= x) && (p[old_pos] <= y)) ||
      (p[old_pos] == old_pos)) {
    inner--;
  }
  if (((other_end[old_pos] >= x) && (other_end[old_pos] <= y)) ||
      (other_end[old_pos] == old_pos)) {
    cycles--;
  }
}

void mo() {
  int x = q[0].x, y = q[0].x - 1;
  int inner = 0; // pointeri din interiorul în interiorul interogării
  int cycles = 0; // cicluri conținute complet
  for (int i = 0; i < num_queries; i++) {
    while (x > q[i].x) {
      include(x, y, inner, cycles, x - 1);
      x--;
    }
    while (y < q[i].y) {
      include(x, y, inner, cycles, y + 1);
      y++;
    }
    while (x < q[i].x) {
      x++;
      exclude(x, y, inner, cycles, x - 1);
    }
    while (y > q[i].y) {
      y--;
      exclude(x, y, inner, cycles, y + 1);
    }

    // Toate valorile care nu pointează intern sînt sfîrșituri de lanțuri.
    int tails = (y - x + 1) - inner;
    answer[q[i].orig] = cycles + tails - (tails == 0);
  }
}

void write_answers() {
  FILE* f = fopen("perm.out", "w");
  for (int i = 0; i < num_queries; i++) {
    fprintf(f, "%d\n", answer[i]);
  }
  fclose(f);
}

int main() {
  read_data();
  compute_inv();
  sort_queries();
  traverse_cycles();
  mo();
  write_answers();
  return 0;
}
