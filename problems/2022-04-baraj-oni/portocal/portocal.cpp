#include <stdio.h>

#define MAX_N 500'000
#define INFINITY 1'000'000
#define MOD 1'000'000'009
#define NIL 0
#define UNBOUND -1

typedef struct {
  int v, next;
} edge;

typedef struct {
  int adj;  // head of adjacency list
  int val;  // node value
  int dnm;  // minimum day if we DON'T match the suffix here
  int dm;   // minimum day if we DO match the suffix here
  int cnm;  // number of assignments if we DON'T match the suffix here
  int cm;   // number of assignments if we DO match the suffix here
  int sts;  // subtree size, INcluding self
  int mu;   // m^(number of unbound nodes in subtree), EXcluding self
} node;

edge e[2 * MAX_N]; // space for linked lists of edges
node h[MAX_N + 1]; // node info
int s[MAX_N];      // the array to match
int n, m, ns;

int min(int x, int y) {
  return (x < y) ? x : y;
}

void add_edge(int u, int v, int pos) {
  e[pos] = { v, h[u].adj };
  h[u].adj = pos;
}

int combine_child_cm(int u, int parent) {
  int min_diff = INFINITY, prod_cnm = 1;
  int neg_result = 1, pos_result = 0;

  for (int pos = h[u].adj; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (v != parent) {
      int choices = (h[v].dnm <  h[v].dm)
        ? h[v].cnm
        : ((h[v].dnm > h[v].dm)
           ? h[v].cm
           : (((long long)h[v].cnm + h[v].cm) % MOD));
      neg_result = (long long)neg_result * choices % MOD;

      if (h[v].dm - h[v].dnm < min_diff) {
        min_diff = h[v].dm - h[v].dnm;
        pos_result = (long long)prod_cnm * h[v].cm % MOD;
      } else if (h[v].dm - h[v].dnm == min_diff) {
        pos_result = ((long long)prod_cnm * h[v].cm +
                      (long long)pos_result * h[v].cnm) % MOD;
      } else {
        pos_result = (long long)pos_result * h[v].cnm % MOD;
      }
      prod_cnm =  (long long)prod_cnm * h[v].cnm % MOD;
    }
  }

  if (min_diff < 0) {
    return neg_result;
  } else if (min_diff == 0) {
    // subtract the case where ALL of the ties are broken in favor of dnm
    return (neg_result + MOD - prod_cnm) % MOD;
  } else {
    // exactly one of the dnm's has to switch to a dm
    return pos_result;
  }

  return 0;
}

void find_min_day(int u, int parent, int level) {
  h[u].sts = 1; // self
  h[u].mu = 1;
  int sum_dnm = 1, prod_cnm = 1, min_cost = INFINITY;
  long long sum_any = 1;

  for (int pos = h[u].adj; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (v != parent) {
      find_min_day(v, u, level + 1);
      h[u].sts += h[v].sts;
      h[u].mu = (long long)h[u].mu * h[v].mu % MOD;
      if (h[v].val == UNBOUND) {
        h[u].mu = (long long)h[u].mu * m % MOD;
      }
      sum_dnm += h[v].dnm;
      int either = min(h[v].dm, h[v].dnm);
      min_cost = min(min_cost, h[v].dm - either);
      sum_any += either;
      prod_cnm =  (long long)prod_cnm * h[v].cnm % MOD;
    }
  }
  sum_any = min(sum_any + min_cost, INFINITY);

  // compute h[u].dnm
  if (level >= ns) {
    h[u].dnm = h[u].sts;
  } else if (h[u].val == UNBOUND && s[level] < m) {
    h[u].dnm = 0;
  } else if (h[u].val == UNBOUND && s[level] == m) {
    h[u].dnm = sum_dnm;
  } else if (h[u].val == s[level]) {
    h[u].dnm = sum_dnm;
  } else if (h[u].val < s[level]) {
    h[u].dnm = h[u].sts;
  } else { // h[u].val > s[level]
    h[u].dnm = 0;
  }

  // compute h[u].dm
  if (h[u].val == UNBOUND || h[u].val == s[level]) {
    h[u].dm = (level < ns - 1) ? sum_any : 0;
  } else {
    h[u].dm = INFINITY;
  }

  // compute h[u].cnm
  h[u].cnm = 0;
  if (level < ns) {
    if (h[u].dnm == h[u].sts) {
      // we don't mind placing a smaller value here, if possible
      int ways = (h[u].val == UNBOUND)
        ? (s[level] - 1)
        : (h[u].val < s[level]);
      h[u].cnm += (long long)ways * h[u].mu % MOD;
    }
    if (h[u].dnm == 0) {
      // we don't mind placing a larger value here, if possible
      int ways = (h[u].val == UNBOUND)
        ? (m - s[level])
        : (h[u].val > s[level]);
      h[u].cnm += (long long)ways * h[u].mu % MOD;
    }
    if (h[u].dnm == sum_dnm  &&
        (((h[u].val == UNBOUND && s[level] == m)) ||
         (h[u].val == s[level]))) {
      h[u].cnm += prod_cnm;
    }
    h[u].cnm %= MOD;
  }

  // compute h[u].cm
  if ((level >= ns) ||
      (h[u].val != UNBOUND && h[u].val != s[level])) {
    h[u].cm = 0;
  } else if (level == ns - 1) {
    h[u].cm = h[u].mu;
  } else {
    h[u].cm = combine_child_cm(u, parent);
  }

  // printf("Node=%2d mu=%6d dnm=%2d dm=%7d cnm=%9d cm=%9d\n",
  //        u, h[u].mu, h[u].dnm, h[u].dm, h[u].cnm, h[u].cm);
}

int main() {
  int task;

  FILE* f = fopen("portocal.in", "r");
  fscanf(f, "%d %d %d %d", &task, &n, &m, &ns);
  for (int u = 1; u <= n; u++) {
    fscanf(f, "%d", &h[u].val);
  }
  for (int i = 0; i < ns; i++) {
    fscanf(f, "%d", &s[i]);
  }
  for (int i = 1; i < n; i++) {
    int u, v;
    fscanf(f, "%d %d", &u, &v);
    add_edge(u, v, 2 * i);
    add_edge(v, u, 2 * i + 1);
  }
  fclose(f);

  find_min_day(1, NIL, 0);

  f = fopen("portocal.out", "w");
  fprintf(f, "%d\n", (task == 1) ? (1 + h[1].dm) : h[1].cm);
  fclose(f);
}
