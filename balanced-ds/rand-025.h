const int MAX_LEVELS = 17;
const char RAND_METHOD[] = "buffer 0,25";

static inline bool coin_toss() {
  static int rnd, uses = 0;

  if (!uses) {
    rnd = rand();
    uses = 15;
  }

  uses--;
  bool result = (rnd & 3) == 0;
  rnd >>= 2;
  return result;
}
