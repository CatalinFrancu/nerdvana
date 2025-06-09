#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

const int N = 1'000'000;
const int DECAY = 1'000;
const int SIGMA = 6;

struct radix_suffix_array {
  const char* s;
  int p[N + 1], c[N + 1], p2[N + 1], c2[N + 1];
  int lcp[N + 1];
  int cnt[256];
  int n, num_classes;

  void init(char* s, int n) {
    this->s = s;
    this->n = n;
  }

  int circ(int x, int n) {
    return (x >= n) ? (x - n) : x;
  }

  void build_single_letters() {
    // Letter frequencies and partial sums.
    memset(cnt, 0, sizeof(int) * 256);
    for (int i = 0; i < n; i++) {
      cnt[(int)s[i]]++;
    }
    for (int i = 1; i <= 'z'; i++) {
      cnt[i] += cnt[i - 1];
    }
    // Now cnt[x] is the number of letters <= x.

    // Make p store the order of length 1 substrings.
    for (int i = 0; i < n; i++) {
      p[--cnt[(int)s[i]]] = i;
    }

    // Compute character classes in the order given by p.
    c[p[0]] = 0;
    for (int i = 1; i < n; i++) {
      c[p[i]] = c[p[i - 1]] + (s[p[i]] != s[p[i - 1]]);
    }
    num_classes = 1 + c[p[n - 1]];
  }

  void build_prefix_len(int len) {
    // Class frequencies and partial sums.
    memset(cnt, 0, sizeof(int) * num_classes);
    for (int i = 0; i < n; i++) {
      cnt[c[i]]++;
    }
    for (int i = 1; i < num_classes; i++) {
      cnt[i] += cnt[i - 1];
    }
    // Now cnt[x] is the frequency of classes <= x.

    // Redistribuite values in p[] in classes of sizes given by cnt, in the
    // order given by p2[].
    for (int i = 0; i < n; i++) {
      p2[i] = circ(p[i] - len + n, n);
    }
    for (int i = n - 1; i >= 0; i--) {
      p[--cnt[c[p2[i]]]] = p2[i];
    }

    // Recompute the new classes in c2, then move them in c.
    c2[p[0]] = 0;
    for (int i = 1; i < n; i++) {
      bool diff =  (c[p[i]] != c[p[i - 1]]) ||
        (c[circ(p[i] + len, n)] != c[circ(p[i - 1] + len, n)]);
      c2[p[i]] = c2[p[i - 1]] + diff;
    }
    num_classes = 1 + c2[p[n - 1]];
    memcpy(c, c2, sizeof(int) * n);
  }

  void build() {
    build_single_letters();

    // Prefix doubling step.
    int len = 1;
    while (num_classes < n) {
      build_prefix_len(len);
      len <<= 1;
    }
  }

  void build_lcp() {
    // Inversează-l pe p în p2
    for (int i = 0; i <= N; i++) {
      p2[p[i]] = i;
    }
    // Acum p2[i] ne spune al cîtelea sufix, în ordine lexicografică, este s[i...N]

    lcp[0] = 0; // Tehnic, nedefinit
    int l = 0;
    for (int i = 0; i <= N; i++) {
      int k = p2[i];
      if (k) {
        // Știm că LCP(p[k - 1], p[k]) >= l. Creștem naiv cît putem.
        int j = p[k - 1];
        while (s[i + l] == s[j + l]) {
          l++;
        }
        lcp[k] = l;

        // Pregătim l-ul pentru următorul k.
        if (l) {
          l--;
        }
      }
    }
  }

  void verify() {
    for (int i = 0; i < n; i++) {
      assert(strcmp(s + p[i], s + p[i + 1]) <= 0);
    }
  }

  void verify_lcp() {
    for (int i = 1; i < n; i++) {
      // Verificăm că exact lcp[i] octeți coincid.
      assert(strncmp(s + p[i - 1], s + p[i], lcp[i]) == 0);
      assert(strncmp(s + p[i - 1], s + p[i], lcp[i] + 1) < 0);
    }
  }
};

radix_suffix_array suf;
char s[N + 1];

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int seed = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(seed);
}

void gen_string() {
  for (int i = 0; i < N; i++) {
    int c = 0;
    while ((c < SIGMA - 1) && (rand() % DECAY == 0)) {
      c++;
    }
    s[i] = 'a' + c;
  }
  s[N] = '\0';
}

int main() {
  init_rng();
  gen_string();

  suf.init(s, N + 1);
  suf.build();
  suf.build_lcp();
  suf.verify();
  suf.verify_lcp();

  return 0;
}
