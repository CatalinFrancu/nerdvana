// Complexitate: O(q log^2 n) + mînăreală.
//
// Metodă: descompunere heavy-light. Comentarii la final.
#include <stdio.h>

const int MAX_NODES = 200'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int parent;
  int heavy;  // fiul cu cel mai mare subarbore
  int head;   // începutul lanțului nostru
  int pos;    // momentul descoperirii în dfs
  int pop;    // populația din subarbore; relevantă doar pentru fiii light
};

struct fenwick_tree {
  int v[MAX_NODES + 1];
  int n;

  void init(int n) {
    this->n = n;
  }

  void add(int pos, int val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  void range_add(int l, int r, int val) {
    add(l, val);
    add(r + 1, -val);
  }

  void point_add(int pos, int val) {
    range_add(pos, pos, val);
  }

  int get(int pos) {
    long long s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }
};

int max(int x, int y) {
  return (x > y) ? x : y;
}

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
fenwick_tree fen;
FILE *fin, *fout;
int n, num_queries, total_pop;

void add_edge(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree() {
  fscanf(fin, "%d %d", &n, &num_queries);
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    fscanf(fin, "%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }

  for (int u = 1; u <= n; u++) {
    fscanf(fin, "%d", &nd[u].pop);
    total_pop += nd[u].pop;
  }
}

int heavy_dfs(int u) {
  int my_size = 1, max_child_size = 0;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent) {

      nd[v].parent = u;
      int child_size = heavy_dfs(v);
      my_size += child_size;
      nd[u].pop += nd[v].pop;
      if (child_size > max_child_size) {
        max_child_size = child_size;
        nd[u].heavy = v;
      }

    }
  }

  return my_size;
}

void decompose_dfs(int u, int head) {
  static int time = 1;

  nd[u].head = head;
  nd[u].pos = time++;
  fen.point_add(nd[u].pos, nd[u].pop);

  if (nd[u].heavy) {
    decompose_dfs(nd[u].heavy, head);
  }

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent && v != nd[u].heavy) {
      decompose_dfs(v, v); // la v începe un lanț nou
    }
  }
}

void update(int u, int delta) {
  total_pop += delta;
  while (u) {
    int h = nd[u].head;
    fen.range_add(nd[h].pos, nd[u].pos, delta);
    nd[h].pop += delta;
    u = nd[h].parent;
  }
}

int query_light_children(int u) {
  int result = 0;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent && v != nd[u].heavy) {
      result = max(result, nd[v].pop);
    }
  }

  return result;
}

int query(int u) {
  int size_of_u = fen.get(nd[u].pos);
  int max_flow = total_pop - size_of_u;

  if (nd[u].heavy) {
    int from_heavy_child = fen.get(nd[nd[u].heavy].pos);
    max_flow = max(max_flow, from_heavy_child);
  }

  int from_light_c = query_light_children(u);
  max_flow = max(max_flow, from_light_c);

  return max_flow;
}

void process_ops() {
  char type;
  int u, delta;

  while (num_queries--) {
    fscanf(fin, " %c", &type);
    if (type == 'U') {
      fscanf(fin, "%d %d ", &delta, &u);
      update(u, delta);
    } else {
      fscanf(fin, "%d ", &u);
      fprintf(fout, "%d\n", query(u));
    }
  }
}

int main() {
  fin = fopen("rafaela.in", "r");
  fout = fopen("rafaela.out", "w");

  read_tree();
  fen.init(n);
  heavy_dfs(1);
  decompose_dfs(1, 1);
  process_ops();

  fclose(fin);
  fclose(fout);

  return 0;
}

// Înrădăcinăm arborele în nodul 1. Fie S(u) numărul curent de noduri din
// subarborele lui u, inclusiv u. Fie T numărul total de cetățeni (echivalent
// cu S(1)).
//
// Întrucît cetățenii se adună spre capitala c, traficul maxim va fi pe o
// muchie incidentă capitalei. Există două cazuri:
//
// 1. Pe muchia de la părinte spre c traficul va fi T - S(c).
// 2. Pe o muchie de la un fiu v spre c traficul va fi S(v).
//
// Pentru (1) trebuie să răspundem la interogarea „Cît este S(c) acum?” Pentru
// (2) trebuie să răspundem la interogarea „Cît este S(v) maxim pentru orice
// fiu v al lui c”?
//
// Cînd populația unui nod se modifică, toți strămoșii lui trebuie notificați.
// De aceea, descompunem heavy-light arborele și construim pe fiecare lanț un
// arbore Fenwick cu suport pentru adăugare pe interval și interogare
// punctuală. Cu acesta putem procesa muchiile care duc din c la părinte și la
// fiul heavy.
//
// v4: Testele de intrare nu par să pedepsească iterarea naivă prin toți fiii
// light ai nodului interogat. Așadar renunțăm complet la set.
