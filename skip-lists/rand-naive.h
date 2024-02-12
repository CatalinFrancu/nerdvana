const int MAX_LEVELS = 21;
const char RAND_METHOD[] = "naiv";

static inline bool coin_toss() {
  return rand() & 1;
}
