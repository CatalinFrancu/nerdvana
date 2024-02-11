const int MAX_LEVELS = 20;

static inline bool coin_toss() {
  static int rnd, uses = 0;

  if (!uses) {
    rnd = rand();
    uses = 30;
  }

  uses--;
  bool result = rnd & 1;
  rnd >>= 1;
  return result;
}
