// In addition to v1, reduce memory usage to 64 kB: 25,000 bytes for the
// string and 90 blocks of 444 bytes each.
#include <stdio.h>
#include <string.h>

const int MAX_LEN = 100'000;
const int BLOCK_SIZE = 1'112;
const int MAX_BLOCKS = (MAX_LEN - 1) / BLOCK_SIZE + 1;
const int LATIN_SIGMA = 26;
const int SIGMA = 4;
const int MAX_Q_LEN = 10;
const int OP_UPDATE = 1;
const int OP_QUERY = 2;

// An array using two bits per element
struct packed_array {
  unsigned v[MAX_LEN / 16];

  void set(int pos, int val) {
    int word = pos / 16;
    int offset = pos % 16 * 2;
    v[word] = (v[word] & ~(3 << offset)) | (val << offset);
  }

  int get(int pos) {
    int word = pos / 16;
    int offset = pos % 16 * 2;
    return (v[word] >> offset) & 3;
  }
};

packed_array s;

struct block_info {
  int start;
  short cnt[SIGMA][MAX_Q_LEN * (MAX_Q_LEN + 1) / 2];

  void init(int start, int end) {
    this->start = start;
    for (int pos = start; pos < end; pos++) {
      update(s.get(pos), pos, +1);
    }
  }

  void update(int c, int pos, int val) {
    for (int mod = 1; mod <= MAX_Q_LEN; mod++) {
      int gauss = mod * (mod - 1) / 2;
      cnt[c][gauss + (pos - start) % mod] += val;
    }
  }

  int query(char* q, int qlen, int rem) {
    int result = 0;
    for (int i = 0; i < qlen; i++) {
      // Count the occurrences of q[rem] at positions equal to i modulo qlen.
      int pos = qlen * (qlen - 1) / 2 + i;
      result += cnt[(int)q[rem]][pos];
      rem = (rem + 1) % qlen;
    }
    return result;
  }
};

char letter_map[LATIN_SIGMA];
block_info block[MAX_BLOCKS];
int n, num_blocks;

void precompute_letter_map() {
  letter_map['A' - 'A'] = 0;
  letter_map['C' - 'A'] = 1;
  letter_map['G' - 'A'] = 2;
  letter_map['T' - 'A'] = 3;
}

int translate_char(char c) {
  return letter_map[c - 'A'];
}

void translate_string(char* s) {
  for (char* c = s; *c; c++) {
    *c = translate_char(*c);
  }
}

void read_string() {
  char c;
  while ((c = getchar()) != '\n') {
    s.set(n++, translate_char(c));
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void init_blocks() {
  num_blocks = (n - 1) / BLOCK_SIZE + 1;
  for (int b = 0; b < num_blocks; b++) {
    int start = b * BLOCK_SIZE;
    int end = min(start + BLOCK_SIZE, n);
    block[b].init(start, end);
  }
}

void update(int pos, int c) {
  int b = pos / BLOCK_SIZE;
  block[b].update(s.get(pos), pos, -1);
  s.set(pos, c);
  block[b].update(c, pos, +1);
}

int naive_count(int l, int r, char* q, int qlen, int rem) {
  int result = 0;
  while (l < r) {
    result += (s.get(l) == q[rem]);
    rem = (rem + 1) % qlen;
    l++;
  }
  return result;
}

int block_sum(int bl, int br, char* q, int qlen, int rem) {
  int result = 0;
  while (bl < br) {
    result += block[bl].query(q, qlen, rem);
    rem = (rem + BLOCK_SIZE) % qlen;
    bl++;
  }
  return result;
}

int query(int l, int r, char* q, int qlen) { // [l inclusive, r exclusive)
  int bl = l / BLOCK_SIZE;
  int br = r / BLOCK_SIZE;

  if (bl == br) {
    return naive_count(l, r, q, qlen, 0);
  } else {
    int rem_l = ((bl + 1) * BLOCK_SIZE - l) % qlen;
    int rem_r = (br * BLOCK_SIZE - l) % qlen;
    return naive_count(l, (bl + 1) * BLOCK_SIZE, q, qlen, 0)
      + naive_count(br * BLOCK_SIZE, r, q, qlen, rem_r)
      + block_sum(bl + 1, br, q, qlen, rem_l);
  }
}

void process_ops() {
  int num_ops, type;
  scanf("%d", &num_ops);
  while (num_ops--) {
    scanf("%d", &type);
    if (type == OP_UPDATE) {
      int x;
      char c;
      scanf("%d %c", &x, &c);
      update(x - 1, translate_char(c));
    } else {
      int l, r, qlen;
      char q[MAX_Q_LEN + 1];
      scanf("%d %d %s", &l, &r, q);
      qlen = strlen(q);
      translate_string(q);
      int answer = query(l - 1, r, q, qlen);
      printf("%d\n", answer);
    }
  }
}

int main() {
  precompute_letter_map();
  read_string();
  init_blocks();
  process_ops();

  return 0;
}
