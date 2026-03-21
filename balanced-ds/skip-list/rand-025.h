const int MAX_LEVELS = 17;
const char RAND_METHOD[] = "buffer 0,25";

bool coin_toss() {
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

int get_height() {
  int h = 1;
  while (coin_toss() && (h < MAX_LEVELS)) {
    h++;
  }
  return h;
}
