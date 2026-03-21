const int MAX_LEVELS = 21;
const char RAND_METHOD[] = "naiv";

int get_height() {
  return 1 + __builtin_ctz(rand());
}
