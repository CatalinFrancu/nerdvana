// Complexitate O(N * P). Comentarii la final.
//
// Autor Cătălin Frâncu
#include <queue>
#include <stdio.h>

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
  FILE* f = fopen("graba.in", "r");
  fscanf(f, "%d %d", &n, &p);
  for (int i = 0; i < p; i++) {
    x[i] = buf + i * n;
    for (int j = 0; j < n; j++) {
      fscanf(f, "%d", &x[i][j].list_id); // pune valorile aici provizoriu
    }
    fscanf(f, "%d", &y[i]);
  }
  fclose(f);
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

// Metodă: Dacă există o coloană col și o valoare val astfel încît toate
// valorile de val de pe coloana col corespund la valori y egale, atunci:
//
// 1. Putem emite testul "if (x_col == val) return y".
// 2. Putem elimina pentru totdeauna liniile pentru care x_col == val.
//
// Pe fiecare coloană construim cîte o listă înlănțuită pentru fiecare valoare
// distinctă. Mai exact, fiecare x[r][c] = val pointează la liniile anterioară
// r' și următoare r'' care au x[r'][c] = x[r''][c] = val (sau NIL dacă nu
// există). Fiecare listă stochează și două metainformații: un pointer la
// primul element și o valoare numită „discrepanță”.
//
// Rostul discrepanței este următorul: dorim să aflăm rapid dacă toate liniile
// dintr-o listă corespund la valori y egale. Mai mult, dorim să menținem
// această informație pe măsură ce eliminăm linii, linii care se traduc în
// elemente eliminate din fiecare listă. O metodă ar fi să calculăm tabele
// hash cu valori și frecvențele lor. Sursa prezentă reține, pentru fiecare
// listă, numărul de perechi de poziții consecutive pentru care y-ii diferă.
// Aceasta este discrepanța.
//
// Cînd eliminăm o linie, ea cauzează eliminarea fiecărui element de pe acea
// linie din lista sa respectivă. Pe lîngă logistica normală, putem recalcula
// banal discrepanța listei: lista pierde una sau două unități de discrepanță
// dacă elementul anterior și următor celui eliminat există și au y-i diferiți
// de cel eliminat. Apoi lista cîștigă o unitate de discrepanță dacă elementul
// anterior și următor, acum devenite vecine, au y-i diferiți.
//
// Dacă discrepanța unei liste era pozitivă înainte de eliminare și a devenit
// zero după eliminare, atunci lista conține doar elemente egale și o putem
// folosi pentru a impune o nouă condiție. Menținem o coadă cu listele
// folosibile.
