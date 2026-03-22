#include "common.h"

std::string get_generator_description() {
  return "generator cu buffer și p = 0,375";
}

bool coin_toss() {
  static int rnd, uses = 0;

  if (!uses) {
    rnd = rand();
    uses = 10;
  }

  uses--;
  bool result = (rnd & 7) < 3;
  rnd >>= 3;
  return result;
}

int get_height() {
  int h = 1;
  while (coin_toss() && (h < MAX_LEVELS)) {
    h++;
  }
  return h;
}
