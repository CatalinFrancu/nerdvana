// Interactor for GCD Guess.
// Reads the list of numbers to hide from data.in.
// Communicates with the guesser via stdin/stdout.
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

const int MAX_QUESTIONS = 30;
const int MAX_VALUE = 2'000'000'000;

void print_both(const char* msg, va_list args) {
  va_list args2;
  va_copy(args2, args);
  fprintf(stderr, "To client: ");
  vfprintf(stderr, msg, args);
  vprintf(msg, args2);
  fflush(stdout);
}

void die(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
  exit(1);
}

void say(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  print_both(msg, args);
  va_end(args);
}

unsigned gcd(unsigned a, unsigned b) {
  while (b) {
    int tmp = a; a = b; b = tmp % b;
  }
  return a;
}

void play_the_game(unsigned x) {
  unsigned q = 0, a, b;

  while (scanf(" ? %u %u", &a, &b) == 2) {
    if (++q > MAX_QUESTIONS) {
      die("Question limit exceeded.\n");
    }
    if ((a < 0) || (a > MAX_VALUE) ||
        (b < 0) || (b > MAX_VALUE)) {
      die("Parameters out of bounds: %d %d.\n", a, b);
    }
    if (a == b) {
      die("Naughty naughty! You passed the value %d twice.\n", a);
    }
    fprintf(stderr, "From client: (%u, %u)\n", a, b);
    say("%d\n", gcd(a + x, b + x));
  }

  if (scanf(" ! %u", &a) != 1) {
    die("Syntax error.\n");
  } else if (a != x) {
    die("BZZZZT! You guessed %u, x was %u.\n", a, x);
  } else {
    fprintf(stderr, "Correct! x = %u with %d questions\n", x, q);
  }
}

int main() {
  FILE* f = fopen("data.in", "r");
  int num_tests;

  fscanf(f, "%d", &num_tests);
  say("%d\n", num_tests);
  while (num_tests--) {
    int x;
    fscanf(f, "%d", &x);
    play_the_game(x);
  }

  fclose(f);

  return 0;
}
