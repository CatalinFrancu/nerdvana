// Complexitate: O((n + q) sqrt q).
//
// Metodă: Descompunere în radical. Comentarii la final.
#include <algorithm>
#include <ctype.h>
#include <stdio.h>
#include <unordered_set>
#include <vector>

const int MAX_NODES = 200'000;
const int MAX_QUERIES = 200'000;
const int BLOCK_SIZE = 3'000;

struct fast_input {
  static const int BUF_SIZE = 4096;
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

  char read_non_whitespace() {
    char c;

    do {
      c = get_char();
    } while (isspace(c));

    return c;
  }

  int read_int() {
    int result = 0;
    int sgn = +1;
    char c;

    do {
      c = get_char();
    } while (isspace(c));

    if (c == '-') {
      c = '0';
      sgn = -1;
    }

    do {
      result = 10 * result + c - '0';
      c = get_char();
    } while (isdigit(c));

    return sgn * result;
  }
};

struct query {
  int id;             // ordinea de la intrare
  int outside_delta;  // modificări în afara subarborelui
  int child_delta;    // modificări la fiul curent
  int max_touched;    // maximul dintre fiii anteriori

  query(int id) {
    this->id = id;
    outside_delta = 0;
    child_delta = 0;
    max_touched = 0;
  }
};

struct node {
  std::vector<int> adj;
  std::vector<query> q; // interogări (din blocul curent)
  int parent;
  int tin, tout;        // timpii de intrare/ieșire din DFS
  int pop, spop;        // populația din nod, respectiv din subarbore
};

struct update {
  int id, tin, delta;
};

node nd[MAX_NODES + 1];
update updates[BLOCK_SIZE];
int answer[MAX_QUERIES + 1];
std::unordered_set<int> nodes_with_queries;
int n, num_ops, num_updates;
fast_input fin("rafaela.in");

int min(int x, int y) {
  return (x < y) ? x : y;
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

void read_data() {
  n = fin.read_int();
  num_ops = fin.read_int();

  for (int i = 0; i < n - 1; i++) {
    int u = fin.read_int();
    int v = fin.read_int();
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }

  for (int u = 1; u <= n; u++) {
    nd[u].pop = fin.read_int();
  }
}

// Calculează timpii DFS și părinții
void initial_dfs(int u) {
  static int time = 0;
  nd[u].tin = time++;

  for (int v: nd[u].adj) {
    if (v != nd[u].parent) {
      nd[v].parent = u;
      initial_dfs(v);
    }
  }

  nd[u].tout = time - 1;
}

void read_ops(int start, int end) {
  num_updates = 0;
  for (int id = start; id < end; id++) {
    char type = fin.read_non_whitespace();

    if (type == 'U') {
      int delta = fin.read_int();
      int u = fin.read_int();
      updates[num_updates++] = { id, nd[u].tin, delta };
      // .pop va intra în vigoare în următorul bloc
      nd[u].pop += delta;
    } else {
      int u = fin.read_int();
      nd[u].q.push_back(query(id));
      nodes_with_queries.insert(u);
    }
  }
}

void sort_updates_by_dfs_time() {
  std::sort(updates, updates + num_updates, [](update& a, update& b) {
    return a.tin < b.tin;
  });
}

void dfs(int u) {
  nd[u].spop = nd[u].pop;

  for (int v: nd[u].adj) {
    if (v != nd[u].parent) {
      dfs(v);
      nd[u].spop += nd[v].spop;
    }
  }
}

// Notifică-l pe u că există actualizarea upd în afara subarborelui său.
void process_outside_update(int u, update& upd) {
  for (query& q: nd[u].q) {
    if (upd.id < q.id) {
      q.outside_delta += upd.delta;
    }
  }
}

// Notifică-l pe u că există actualizarea upd în fiul său curent.
void process_child_update(int u, update& upd) {
  for (query& q: nd[u].q) {
    if (upd.id < q.id) {
      q.child_delta += upd.delta;
    }
  }
}

int merge_init(int u) {
  int i = 0;

  while ((i < num_updates) && (updates[i].tin < nd[u].tin)) {
    process_outside_update(u, updates[i++]);
  }

  while ((i < num_updates) && (updates[i].tin == nd[u].tin)) {
    // Doar le sărim. Populația din u nu influențează interogările din u.
    i++;
  }

  return i;
}

int merge_advance(int u, int v, int i) {
  while ((i < num_updates) && (updates[i].tin <= nd[v].tout)) {
    process_child_update(u, updates[i++]);
  }

  for (query& q: nd[u].q) {
    q.max_touched = max(q.max_touched, nd[v].spop + q.child_delta);
    q.child_delta = 0;
  }

  return i;
}

void merge_finish(int u, int i) {
  while (i < num_updates) {
    process_outside_update(u, updates[i++]);
  }
}

void answer_queries(int u, int max_untouched) {
  for (query& q: nd[u].q) {
    // fluxul pe muchia dinspre părinte
    answer[q.id] = nd[1].spop + q.outside_delta - nd[u].spop;

    // maximul fiilor afectați de actualizări
    answer[q.id] = max(answer[q.id], q.max_touched);

    // maximul fiilor neafectați de actualizări
    answer[q.id] = max(answer[q.id], max_untouched);
  }

  nd[u].q.clear();
}

void process_queries(int u) {
  int max_untouched = 0;
  int i = merge_init(u);

  for (int v: nd[u].adj) {
    if (v != nd[u].parent) {
      int old_i = i;
      i = merge_advance(u, v, i);

      if (i == old_i) {
        max_untouched = max(max_untouched, nd[v].spop);
      }
    }
  }

  merge_finish(u, i);
  answer_queries(u, max_untouched);
}

void process_nodes_with_queries() {
  for (int u: nodes_with_queries) {
    process_queries(u);
  }
  nodes_with_queries.clear();
}

void process_ops() {
  for (int start = 0; start < num_ops; start += BLOCK_SIZE) {
    int end = min(start + BLOCK_SIZE, num_ops);
    dfs(1);
    read_ops(start, end);
    sort_updates_by_dfs_time();
    process_nodes_with_queries();
  }
}

void write_answers() {
  FILE* f = fopen("rafaela.out", "w");

  for (int i = 0; i < num_ops; i++) {
    if (answer[i]) {
      fprintf(f, "%d\n", answer[i]);
    }
  }

  fclose(f);
}

int main() {
  read_data();
  initial_dfs(1);
  process_ops();
  write_answers();

  return 0;
}

// Procesăm interogările în blocuri de circa sqrt(q).
//
// La începutul blocului, facem un DFS ca să aflăm populația subarborelui
// fiecărui nod, notată cu spop(u). Atunci, pentru o interogare în nodul u,
// răspunsul poate proveni din:
//
// 1. Părintele p. Pe acea muchie traficul este spop(p) - spop(u).
// 2. Maximul dintre fiii în subarborii căruia nu există actualizări.
// 3. Maximul dintre fiii în subarborii căruia există actualizări.
//
// Pentru (3) observația-cheie este că există cel mult sqrt(q) fii cu
// actualizări. Ne permitem să consultăm toți fiii dintr-un nod, dar o singură
// dată (asta face și DFS-ul). Dar nu ne permitem să-i consultăm o dată pentru
// fiecare interogare din acel nod, căci am obține O(q·n). Restul sînt detalii
// de implementare ca să obținem efort O(sqrt(q)) per interogare + O(n) per
// bloc.
//
// Sortăm actualizările din bloc în ordinea timpilor de intrare în DFS. Fiii
// nodului u sînt natural sortați în aceeași ordine. Acum interclasăm fiii cu
// actualizările. Astfel putem afla, pentru fiecare actualizare într-un nod z,
// fiul lui u din al cărui subarbore face parte z.
//
// La interclasare, dacă observăm că un fiu v al lui u nu are actualizări în
// subarbore, îl colectăm separat și luăm maximul acestor fii (categoria 2 de
// mai sus). Pentru fiii v care au actualizări, trebuie să remarcăm că aceste
// actualizări nu se aplică neapărat tuturor interogărilor din u (depinde de
// ordinea cronologică). Deci confruntăm fiecare actualizare cu fiecare
// interogare.
//
// La epuizarea interogărilor din subarborele lui v, fiecare interogare din u
// își poate calcula contribuția fiului v (categoria 3 de mai sus).
//
// v2: Procesarea interogărilor dintr-un nod (inclusiv partea de interclasare
// a fiilor cu actualizările) nu trebuie neapărat să stea în DFS. Pentru
// claritate, am extras-o separat.
//
// v3: Renunță la cîmpul touched. Iterează doar prin nodurile care au
// interogări.
//
// v4: Renunță la total_pop. Încorporează mai devreme actualizările.
//
// v5: Stochează timpul DFS pe actualizări, în locul nodului.
//
// v6: Citire rapidă.
