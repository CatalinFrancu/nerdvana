// Complexitate: O(n).
//
// Metodă: Operăm pe muchii, deci dorim ca adîncimea arborelui să fie K=K'-1,
// unde K' este valoarea citită. Dacă un subarbore cu rădăcina u are diametrul
// mai mic decît 2K, nu este nevoie să tăiem muchii din interiorul lui. Putem
// obține o soluție cel puțin la fel de bună dacă tăiem muchia dintre u și
// părinte și atîrnăm arborele detașat de centrul său.
//
// Dacă depistăm că diametrul subarborelui depășește 2K, este nevoie să tăiem:
//
// 1. Orice fiu care combinat cu alt fiu dă un diametru prea mare. În esență,
//    asta înseamnă să eliminăm toți fii mai înalți decît K, cu niște if-uri.
// 2. Orice fiu care combinat cu părintele (înălțime++) ne dă un diametru prea
//    mare.
//
// Pentru rădăcină criteriul este diferit -- tăiem orice nod care are
// înălțimea mai mare decît K-1.
//
// Menținem recursiv diametrele și centrele.
//
// Autor: Cătălin Frâncu
//
// TODO: Curățenie majoră.
#include <stdio.h>

const int MAX_NODES = 300000;

struct cell {
  int v, next;
};

struct vertex {
  int adj;
  int parent;
  int depth; // de la rădăcină
  int height; // numărul de muchii pînă la cea mai depărtată frunză
  int diam;
  int center;
};

struct cut {
  int u, v; // taie muchia (u->v) și atîrnă subarborele de centrul lui v
};

cell list[2 * MAX_NODES];
vertex node[MAX_NODES + 1];
cut c[MAX_NODES];
int n, desired, num_cuts;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, node[u].adj };
  node[u].adj = pos++;
}

void read_data() {
  FILE* f = fopen("redpanda.in", "r");

  fscanf(f, "%d %d", &n, &desired);
  desired--; // muchii
  for (int i = 1; i < n; i++) {
    int u, v;
    fscanf(f, "%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
  fclose(f);
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

// Găsește centrul subarborelui u știind că el se află în fiul v.
int find_center(int u, int v) {
  int center_depth = node[v].depth + node[v].height - node[u].diam / 2;
  int c = node[v].center;
  while (node[c].depth > center_depth) {
    c = node[c].parent;
  }
  return c;
}

void compute_diameter(int u) {
  // Dacă u este frunză,
  node[u].diam = 0;
  node[u].center = u;

  int c1 = 0, c2 = 0, h1 = -1, h2 = -1;

  for (int ptr = node[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v) {
      if (node[v].height > h1) {
        h2 = h1;
        c2 = c1;
        h1 = node[v].height;
        c1 = v;
      } else if (node[v].height > h2) {
        h2 = node[v].height;
        c2 = v;
      }
      if (node[v].diam > node[u].diam) {
        node[u].diam = node[v].diam;
        node[u].center = node[v].center;
      }
    }
  }

  if (c1 && c2) {
    if (h1 + h2 + 2 > node[u].diam) {
      node[u].diam = h1 + h2 + 2;
      node[u].center = find_center(u, c1);
    }
  } else if (c1) {
    if (h1 + 1 > node[u].diam) {
      node[u].diam = h1 + 1;
      node[u].center = find_center(u, c1);
    }
  }
}

void compute_height(int u) {
  node[u].height = 0;

  for (int ptr = node[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v) {
      node[u].height = max(node[u].height, 1 + node[v].height);
    }
  }
}

int find_children_to_keep(int u) {
  int sm_above = 0, lg_below = 0;
  for (int ptr = node[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v) {
      if (node[v].height >= desired - 1) {
        if (!sm_above || (node[v].height < node[sm_above].height)) {
          sm_above = v;
        }
      } else {
        if (!lg_below || (node[v].height > node[lg_below].height)) {
          lg_below = v;
        }
      }
    }
  }

  if ((sm_above && lg_below &&
       (2 + node[sm_above].height + node[lg_below].height > 2 * (desired - 1))) ||
      (sm_above &&
       (1 + node[sm_above].height) > 2 * (desired - 1))) {
    return 0;
  } else {
    return sm_above;
  }
}

void trim_children(int u) {
  int sm_above = find_children_to_keep(u);

  for (int ptr = node[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v) {
      if ((1 + node[v].height >= desired) && (v != sm_above)) {
        c[num_cuts++] = { u, v };
        list[ptr].v = 0;
      }
    }
  }
}

void dfs(int u, int parent) {
  node[u].parent = parent;
  node[u].depth = 1 + node[parent].depth;

  for (int ptr = node[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != node[u].parent) {
      dfs(v, u);
    } else {
      list[ptr].v = 0; // marchează muchia (u,v) ca ștearsă
    }
  }

  if (u != 1) {
    trim_children(u);
  }
  compute_diameter(u); // dintre fiii rămași
  compute_height(u);
}

void trim_root() {
  for (int ptr = node[1].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v && (node[v].height >= desired)) {
      c[num_cuts++] = { 1, v };
    }
  }
}

void write_solution() {
  FILE* f = fopen("redpanda.out", "w");
  fprintf(f, "%d\n", num_cuts);
  for (int i = 0; i < num_cuts; i++) {
    fprintf(f, "%d %d 1 %d\n", c[i].u, c[i].v, node[c[i].v].center);
  }
  fclose(f);
}

int main() {
  read_data();
  dfs(1, 0);
  trim_root();
  write_solution();

  return 0;
}
