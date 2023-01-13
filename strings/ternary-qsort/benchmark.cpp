// Benchmarking code for three string-sorting algorithms:
// 1. STL sort of STL string objects;
// 2. Quicksort of C strings;
// 3. Ternary quicksort.
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/time.h>

#define TRIALS 10
#define MIN_LEN 20
#define MAX_LEN 50
#define N 1'000'000
#define SIGMA 26

std::string s[N];
char c[N][MAX_LEN + 1];
char* ord[N];

long long time1;

/***************************** RNG functions *****************************/

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

char random_char() {
  return rand() % SIGMA + 'a';
}

int rand2(int min, int max) {
  return rand() % (max - min + 1) + min;
}

/**************************** time management ****************************/

// Returns the current timestamp in milliseconds
long long get_time() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return 1000ll * time.tv_sec + time.tv_usec / 1000;
}

void mark_time() {
  time1 = get_time();
}

void report_time(const char* msg) {
  long long time2 = get_time();
  fprintf(stderr, "%s: %lld milliseconds\n", msg, time2 - time1);
}

/******************** data generation and verification ********************/

void gen_stl_strings() {
  for (int i = 0; i < N; i++) {
    s[i] = "";
    int len = rand2(MIN_LEN, MAX_LEN);
    for (int j = 0; j < len; j++) {
      s[i] += random_char();
    }
  }
}

void validate_stl_strings() {
  for (int i = 0; i < N - 1; i++) {
    assert(s[i] <= s[i + 1]);
  }
}

void gen_c_strings() {
  for (int i = 0; i < N; i++) {
    int len = rand2(MIN_LEN, MAX_LEN);
    for (int j = 0; j < len; j++) {
      c[i][j] = random_char();
    }
    c[i][len] = '\0';
    ord[i] = (char*)c[i];
  }
}

void validate_ord() {
  for (int i = 0; i < N - 1; i++) {
    assert(strcmp(ord[i], ord[i + 1]) <= 0);
  }
}

void swap(int i, int j) {
  char* tmp = ord[i];
  ord[i] = ord[j];
  ord[j] = tmp;
}

// Sorts the strings ord[first] ... ord[last]), which are known to be equal
// up to position pos-1. Changes ord[] during the partitioning step, but does
// not change c[], which would be expensive.
void tsort(int first, int last, int pos) {
  if (last - first <= 1) {
    return;
  }

  int i = first, left = first, right = last;
  char pivot = ord[first][pos];

  // Loop invariant:
  // * ord[first ... left)     the pos-th character is < pivot
  // * ord[left ... i)         the pos-th character is == pivot
  // * ord[i]                  the string currently being evaluated
  // * ord(i ... right)        strings yet to be evaluated
  // * ord[right ... last)     the pos-th character is > pivot
  while (i < right) {
    char x = ord[i][pos];
    if (x < pivot) {
      swap(i++, left++);
    } else if (x > pivot)  {
      swap(i, --right);
    } else {
      i++;
    }
  }

  tsort(first, left, pos);
  if (ord[left][pos] != '\0') {
    tsort(left, right, pos + 1);
  }
  tsort(right, last, pos);
}

int main(void) {
  init_rng();

  for (int i = 0; i < TRIALS; i++) {
    gen_stl_strings();
    mark_time();
    std::sort(s, s + N);
    report_time("STL sort on STL strings");
    validate_stl_strings();

    gen_c_strings();
    mark_time();
    std::sort(ord, ord + N, [](char* a, char* b) {
      return strcmp(a, b) <= 0;
    });
    report_time("STL sort on C strings");
    validate_ord();

    gen_c_strings();
    mark_time();
    tsort(0, N, 0);
    report_time("ternary qsort on C strings");
    validate_ord();

    printf("\n");
  }
}
