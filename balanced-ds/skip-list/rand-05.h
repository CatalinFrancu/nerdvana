const int MAX_LEVELS = 22;
const char RAND_METHOD[] = "buffer 0,5";

bool coin_toss() {
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

int get_height() {
  int h = 1;
  while (coin_toss() && (h < MAX_LEVELS)) {
    h++;
  }
  return h;
}
