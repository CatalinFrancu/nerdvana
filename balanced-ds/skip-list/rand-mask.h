#include "common.h"

std::string get_generator_description() {
  return "generator cu __builtin_ctz";
}

int get_height() {
  int r = rand() | (1 << (MAX_LEVELS - 2));
  return 1 + __builtin_ctz(r);
}
