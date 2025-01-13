// Complexitate: O(q log^2 n).
//
// Metodă: descompunere heavy-light. Comentarii la final.
#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_SEGTREE_NODES = 1 << 19;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int parent;
  int heavy;      // fiul cu cel mai mare subarbore
  int head;       // începutul lanțului nostru
  int pos;        // momentul descoperirii în dfs
  int init_pop;   // populația inițială

  // pozițiile în BFS ale nodului și ale fiilor
  int bfs_pos;
  int bfs_first, bfs_last;

  bool has_light_children() {
    return bfs_last - bfs_first >= 1;
  }
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

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

struct segment_tree {
  int v[MAX_SEGTREE_NODES];
  int n;

  void init(int size) {
    this->n = next_power_of_2(size);
  }

  void raw_set(int pos, int val) {
    v[pos + n] = val;
  }

  void build() {
    for (int pos = n - 1; pos; pos--) {
      v[pos] = max(v[2 * pos], v[2 * pos + 1]);
    }
  }

  void add(int pos, int val) {
    pos += n;
    v[pos] += val;
    for (pos >>= 1; pos; pos >>= 1) {
      v[pos] = max(v[2 * pos], v[2 * pos + 1]);      
    }
  }

  int range_max(int l, int r) {
    int result = 0;

    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        result = max(result, v[l++]);
      }
      l >>= 1;

      if (!(r & 1)) {
        result = max(result, v[r--]);
      }
      r >>= 1;
    }

    return result;
  }
};

struct queue {
  int v[MAX_NODES];
  int head, tail;

  void init() {
    head = tail = 0;
  }

  void enqueue(int u) {
    v[tail++] = u;
  }

  int dequeue() {
    return v[head++];
  }

  bool is_empty() {
    return head == tail;
  }
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
fenwick_tree fen;
segment_tree set;
queue q;
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
    fscanf(fin, "%d", &nd[u].init_pop);
    total_pop += nd[u].init_pop;
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
      nd[u].init_pop += nd[v].init_pop;
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
  fen.point_add(nd[u].pos, nd[u].init_pop);

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

void bfs() {
  int pos = 1;

  q.init();
  q.enqueue(1);

  while (!q.is_empty()) {
    int u = q.dequeue();
    int p = nd[u].parent;

    if (!nd[p].bfs_first) {
      nd[p].bfs_first = pos;
    }
    nd[p].bfs_last = pos;
    set.add(pos, nd[u].init_pop);
    nd[u].bfs_pos = pos++;

    if (nd[u].heavy) {
      q.enqueue(nd[u].heavy);
    }

    for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
      int v = list[ptr].v;
      if (v != nd[u].parent && v != nd[u].heavy) {
        q.enqueue(v);
      }
    }
  }
}

void update(int u, int delta) {
  total_pop += delta;
  while (u) {
    int h = nd[u].head;
    fen.range_add(nd[h].pos, nd[u].pos, delta);
    set.add(nd[h].bfs_pos, delta);
    u = nd[h].parent;
  }
}

int query(int u) {
  int size_of_u = fen.get(nd[u].pos);
  int max_flow = total_pop - size_of_u;

  if (nd[u].heavy) {
    int from_heavy_child = fen.get(nd[nd[u].heavy].pos);
    max_flow = max(max_flow, from_heavy_child);
  }

  if (nd[u].has_light_children()) {
    int from_light_c = set.range_max(nd[u].bfs_first + 1, nd[u].bfs_last);
    max_flow = max(max_flow, from_light_c);
  }

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
  set.init(n);
  heavy_dfs(1);
  decompose_dfs(1, 1);
  bfs();
  set.build();
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
// v3: Pentru fiii light, în locul tuturor seturilor STL putem folosi un
// singur arbore de segmente cu actualizare punctuală și maxim pe
// interval. Toți fiii unui nod sînt vecini într-o parcurgere în lățime.
