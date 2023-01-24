#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define TRIALS 10
#define N 1'000'000
#define DECAY 10
#define SIGMA 6
#define INFINITY 1'000'000

typedef int vector[N + 1];

struct entry {
  int v1, v2, pos;
} l[N];

char s[N + 1];
vector p, c, p2, c2, cnt;
long long _time;

int max(int x, int y) {
  return (x > y) ? x : y;
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

/**
 * Gestiunea timpului
 **/

long long get_time() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return 1000ll * time.tv_sec + time.tv_usec / 1000;
}

void mark_time() {
  _time = get_time();
}

int report_time(const char* msg, int len) {
  long long delta = get_time() - _time;
  fprintf(stderr, "%s: %lld ms; ultima lungime necesară: %d\n",
          msg, delta, len);
  return delta;
}

int circ(int x, int n) {
  return (x >= n) ? (x - n) : x;
}

int build_with_radix_sort() {
  int n = N + 1; // include și terminatorul \0

  /**
   * Pasul inițial: calculăm p[] și c[] pentru caractere invididuale
   **/

  // Calculează frecvențele caracterelor și sumele parțiale.
  memset(cnt, 0, sizeof(int) * 256);
  for (int i = 0; i < n; i++) {
    cnt[(int)s[i]]++;
  }
  for (int i = 0; i < 'a' + SIGMA; i++) {
    cnt[i] += cnt[i - 1];
  }
  // Acum c[x] este numărul de caractere <= x.

  // Fă-l pe p să rețină o ordine a subșirurilor de lungime 1.
  for (int i = 0; i < n; i++) {
    p[--cnt[(int)s[i]]] = i;
  }

  // Calculează clasele caracterelor în ordinea dată de p.
  int num_classes = 1;
  c[p[0]] = 0;
  for (int i = 1; i < n; i++) {
    if (s[p[i]] != s[p[i - 1]]) {
      num_classes++;
    }
    c[p[i]] = num_classes - 1;
  }

  /**
   * Dublarea prefixului cît timp mai există coliziuni.
   */

  int len = 1;
  while (num_classes < n) {
    for (int i = 0; i < n; i++) {
      p2[i] = circ(p[i] - len + n, n);
    }
    // Calculează frecvențele claselor și sumele parțiale.
    memset(cnt, 0, sizeof(int) * num_classes);
    for (int i = 0; i < n; i++) {
      cnt[c[i]]++;
    }
    for (int i = 1; i < num_classes; i++) {
      cnt[i] += cnt[i - 1];
    }
    // Acum cnt[x] este numărul de poziții de clasă <= x.

    // Redistribuie valorile din p[], în clase de mărimea cnt[], în ordinea
    // dată de p2[].
    for (int i = n - 1; i >= 0; i--) {
      p[--cnt[c[p2[i]]]] = p2[i];
    }

    // Recalculează noile clase în c2, apoi mută-le în c.
    num_classes = 1;
    c2[p[0]] = 0;
    for (int i = 1; i < n; i++) {
      if (c[p[i]] != c[p[i - 1]] ||
          c[circ(p[i] + len, n)] != c[circ(p[i - 1] + len, n)]) {
        num_classes++;
      }
      c2[p[i]] = num_classes - 1;
    }
    memcpy(c, c2, sizeof(int) * n);
    len <<= 1;
  }

  return len / 2;
}

// https://www.infoarena.ro/siruri-de-sufixe
int build_with_sort() {
  int n = N;

  for (int i = 0; i < n; i++) {
    p2[i] = s[i] - 'a';
  }

  bool collision = true;
  int len = 1;
  while (collision) {
    for (int i = 0; i < n; ++i) {
      l[i].v1 = p2[i];
      l[i].v2 = (i + len < n) ? p2[i + len] : -1;
      l[i].pos = i;
    }

    std::sort(l, l + n, [](entry &a, entry &b) {
      return (a.v1 < b.v1) || ((a.v1 == b.v1) && (a.v2 < b.v2));
    });

    p2[l[0].pos] = 0;
    collision = false;
    for (int i = 1; i < n; i++) {
      if (l[i].v1 == l[i - 1].v1 && l[i].v2 == l[i - 1].v2) {
        p2[l[i].pos] = p2[l[i - 1].pos];
        collision = true;
      } else {
        p2[l[i].pos] = i;
      }
    }

    len <<= 1;
  }

  // p2[i]=x înseamnă că prefixul care începe la poziția i este al x-lea
  // lexicografic. Noi vrem inversul: p[x]=i înseamnă că al x-lea prefix este
  // cel care începe la i.
  for (int i = 0; i < n; i++) {
    p[p2[i]] = i;
  }

  return len / 2;
}

void swap(int i, int j) {
  int tmp = p[i];
  p[i] = p[j];
  p[j] = tmp;
}

void tsort(int first, int last, int pos) {
  if (last - first <= 1) {
    return;
  }

  int i = first, left = first, right = last;
  char pivot = s[p[first] + pos];

  while (i < right) {
    char x = s[p[i] + pos];
    if (x < pivot) {
      swap(i++, left++);
    } else if (x > pivot)  {
      swap(i, --right);
    } else {
      i++;
    }
  }

  tsort(first, left, pos);
  if (s[p[left] + pos] != '\0') {
    tsort(left, right, pos + 1);
  }
  tsort(right, last, pos);
}

void build_with_ternary_qsort() {
  for (int i = 0; i < N; i++) {
    p[i] = i;
  }
  tsort(0, N, 0);
}

int main() {

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  int sum1 = 0, min1 = INFINITY, max1 = 0; // milisecunde
  int sum2 = 0, min2 = INFINITY, max2 = 0;
  // int sum3 = 0, min3 = INFINITY, max3 = 0;
  for (int trial = 1; trial <= TRIALS; trial++) {
    printf("Testul %d/%d\n", trial, TRIALS);

    // Generează un șir de lungime N.
    for (int i = 0; i < N; i++) {
      int c = 0;
      while (c < SIGMA - 1 && rand() % 1'000'000 < DECAY) {
        c++;
      }
      s[i] = 'a' + c;
    }
    s[N] = '\0';

    // Metoda 1
    mark_time();
    int len = build_with_radix_sort();
    int t = report_time("Construcție cu radix sort", len);
    sum1 += t;
    min1 = min(min1, t);
    max1 = max(max1, t);

    // Verifică corectitudinea.
    for (int i = 0; i < N; i++) {
      assert(strcmp(s + p[i], s + p[i + 1]) < 0);
    }

    // Metoda 2
    mark_time();
    len = build_with_sort();
    t = report_time("Construcție cu STL sort", len);
    sum2 += t;
    min2 = min(min2, t);
    max2 = max(max2, t);

    // Verifică corectitudinea.
    for (int i = 0; i < N - 1; i++) {
      assert(strcmp(s + p[i], s + p[i + 1]) < 0);
    }

    // // Metoda 3
    // mark_time();
    // build_with_ternary_qsort();
    // t = report_time("Construcție cu ternary qsort", 0);
    // sum3 += t;
    // min3 = min(min3, t);
    // max3 = max(max3, t);

    // // Verifică corectitudinea.
    // for (int i = 0; i < N - 1; i++) {
    //   assert(strcmp(s + p[i], s + p[i + 1]) < 0);
    // }
  }

  sum1 -= min1 + max1; // Ignoră extremele.
  sum2 -= min2 + max2;
  // sum3 -= min3 + max3;
  printf("Timp mediu radix sort: %d ms\n", sum1 / (TRIALS - 2));
  printf("Timp mediu STL sort: %d ms\n", sum2 / (TRIALS - 2));
  // printf("Timp mediu ternary qsort: %d ms\n", sum3 / (TRIALS - 2));

  return 0;
}
