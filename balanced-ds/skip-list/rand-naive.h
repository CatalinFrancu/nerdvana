#include "common.h"

std::string get_generator_description() {
  return "generator cu rand() naiv";
}

int get_height() {
  int h = 1;
  while ((rand() & 1) && (h < MAX_LEVELS)) {
    h++;
  }
  return h;
}
