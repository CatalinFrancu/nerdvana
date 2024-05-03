const int MAX_LEVELS = 18;
const char RAND_METHOD[] = "buffer 0,375";

static inline bool coin_toss() {
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
