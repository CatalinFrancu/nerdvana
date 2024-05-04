// Complexitate O(N * P) cu liste înlănțuite + intrare rapidă.
//
// Autor Cătălin Frâncu
#include <ctype.h>
#include <queue>
#include <stdio.h>

struct fast_input {
  static const int BUF_SIZE = 4098;
  char buf[BUF_SIZE + 1];
  int pos = BUF_SIZE;
  FILE* f;

  fast_input(const char* file_name) {
    f = fopen(file_name, "r");
  }

  char get_char() {
    if (pos == BUF_SIZE) {
      int read = fread(buf, 1, BUF_SIZE, f);
      buf[read] = '\0';
      pos = 0;
    }
    return buf[pos++];
  }

  int read_int() {
    int result = 0;
    char c;
    do {
      c = get_char();
    } while (!isdigit(c));

    do {
      result = 10 * result + c - '0';
      c = get_char();
    } while (isdigit(c));

    return result;
  }
};

const int MAX_NP = 1000000;
const int MAX_VAL = 1000000;
const int NIL = -1;

struct cell {
  int prev, next, list_id;
};

struct list_metainfo {
  int val, disc, head;
};

struct queue_elt {
  int row, col;
};

struct condition {
  int col, val, ret; // if (x[col] == val) return ret;
};

cell buf[MAX_NP];
cell* x[MAX_NP]; // Alocate în cadrul lui buf.
list_metainfo meta[MAX_NP];
int y[MAX_NP];
int last[MAX_VAL + 1];
std::queue<queue_elt> q;
condition cond[MAX_NP];
int n, p, num_removed, num_cond;

void read_data() {
  fast_input f("graba.in");
  n = f.read_int();
  p = f.read_int();
  for (int i = 0; i < p; i++) {
    x[i] = buf + i * n;
    for (int j = 0; j < n; j++) {
      x[i][j].list_id = f.read_int(); // pune valorile aici provizoriu
    }
    y[i] = f.read_int();
  }
}

void reset_last(int col) {
  for (int r = 0; r < p; r++) {
    last[x[r][col].list_id] = NIL;
  }
}

int make_list(int row, int val) {
  static int num_lists = 0;
  meta[num_lists] = { .val = val, .disc = 0, .head = row };
  return num_lists++;
}

void append_to_list(int row, int col) {
  cell& c = x[row][col];
  int& l = last[c.list_id];
  c.prev = l;
  c.next = NIL;
  if (l == NIL) {
    c.list_id = make_list(row, c.list_id);
    // De acum încolo am pus valorile la locul corect, în meta.
  } else {
    c.list_id = x[l][col].list_id;
    x[l][col].next = row;
    meta[c.list_id].disc += (y[row] != y[c.prev]);
  }
  l = row;
}

void make_lists() {
  for (int c = 0; c < n; c++) {
    reset_last(c);
    for (int r = 0; r < p; r++) {
      append_to_list(r, c);
    }
  }
}

void init_queue() {
  for (int r = 0; r < p; r++) {
    for (int c = 0; c < n; c++) {
      list_metainfo& m = meta[x[r][c].list_id];
      if ((m.head == r) && !m.disc) {
        q.push({r, c});
      }
    }
  }
}

void update_discrepancies(int row, int col) {
  cell& c = x[row][col];
  int lost_disc =
    ((c.prev != NIL) && (y[row] != y[c.prev])) +
    ((c.next != NIL) && (y[row] != y[c.next]));
  int gained_disc =
    (c.prev != NIL) && (c.next != NIL) && (y[c.prev] != y[c.next]);

  meta[c.list_id].disc += gained_disc - lost_disc;
}

void reconnect_pointers(int row, int col) {
  cell& c = x[row][col];
  if (c.prev != NIL) {
    x[c.prev][col].next = c.next;
  }
  if (c.next != NIL) {
    x[c.next][col].prev = c.prev;
  }
}

void transfer_head_info(int row, int col) {
  cell& c = x[row][col];
  if (meta[c.list_id].head == row) {
    meta[c.list_id].head = c.next;
  }
}

void delete_cell(int row, int col) {
  list_metainfo& m = meta[x[row][col].list_id];
  int old_disc = m.disc;
  update_discrepancies(row, col);
  reconnect_pointers(row, col);
  transfer_head_info(row, col);

  if (old_disc && !m.disc) {
    q.push({m.head, col});
  }
}

void delete_row(int row, int trigger_col) {
  for (int col = 0; col < n; col++) {
    if (col != trigger_col) {
      delete_cell(row, col);
    }
  }
  num_removed++;
}

void process_list(int row, int col) {
  list_metainfo& m = meta[x[row][col].list_id];
  if (m.head == NIL) {
    // De cînd am pus-o în coadă, i-au dispărut toate elementele.
    return;
  }

  cond[num_cond++] = { col, m.val, y[m.head] };

  for (row = m.head; row != NIL; row = x[row][col].next) {
    delete_row(row, col);
  }
}

void remove_loop() {
  init_queue();
  while (!q.empty() && (num_removed < p)) {
    queue_elt elt = q.front();
    q.pop();
    process_list(elt.row, elt.col);
  }
}

void write_solution() {
  FILE* f = fopen("graba.out", "w");
  if (num_removed < p) {
    fprintf(f, "%d\n", -1);
  } else {
    for (int i = 0; i < num_cond; i++) {
      fprintf(f, "%d %d %d\n", cond[i].col + 1, cond[i].val, cond[i].ret);
    }
  }
  fclose(f);
}

int main() {
  read_data();
  make_lists();
  remove_loop();
  write_solution();

  return 0;
}
