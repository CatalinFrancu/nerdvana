// Treap implementat cu split și merge și cu alocare dinamică.
//
// Metodele de căutare și numărare au aceeași implementare. Doar inserarea și
// ștergerea diferă în funcție de mecanismul de echilibrare.
#include <string>

struct data_structure {
  int key, pri, cnt;
  data_structure *l, *r;

  const std::string get_description() {
    return "treap implementat cu split și merge, cu alocare dinamică";
  }

  data_structure() {
  }

  data_structure(int key) {
    this->key = key;
    pri = rand() >> 1;
    cnt = 1;
    l = r = NULL;
  }

  void init() {
    // Pentru compatibilitate cu structurile alocate static, folosim funcția
    // init(), nu constructorul, pentru inițializarea rădăcinii. Rădăcina va
    // exista mereu și va avea cheie minimă și prioritate maximă.
    key = 0;
    pri = RAND_MAX;
    cnt = 0;
    l = r = NULL;
  }

  data_structure* search(int key) {
    data_structure* t = this->r;
    while (t && key != t->key) {
      t = (key < t->key) ? t->l : t->r;
    }
    return t;
  }

  bool contains(int key) {
    return search(key) != NULL;
  }

  int count(data_structure* t) {
    return t ? t->cnt : 0;
  }

  int order_of(int key) {
    int result = 0;
    data_structure* t = this->r;
    while (t) {
      if (key > t->key) {
        result += 1 + count(t->l);
        t = t->r;
      } else {
        t = t->l;
      }
    }

    return result;
  }

  int kth_element(int k) {
    data_structure* t = this->r;
    int c;
    while (k != (c = count(t->l))) {
      if (k < c) {
        t = t->l;
      } else {
        k -= c + 1;
        t = t->r;
      }
    }
    return t->key;
  }

  void update_cnt(data_structure* t) {
    if (t) {
      t->cnt = 1 + count(t->l) + count(t->r);
    }
  }

  // Sparge subarborele lui t în valori ≤ key și > key și pune cele două
  // rădăcini în l și în r.
  void split(data_structure* t, int key, data_structure** l, data_structure** r) {
    if (!t) {
      *l = *r = NULL;
    } else if (t->key <= key) {
      split(t->r, key, &t->r, r);
      *l = t;
      update_cnt(t);
    } else {
      split(t->l, key, l, &t->l);
      *r = t;
      update_cnt(t);
    }
  }

  // Unifică subarborii lui l și r și pune rădăcina rezultată în t.
  void merge(data_structure** t, data_structure* l, data_structure* r) {
    if (!l) {
      *t = r;
    } else if (!r) {
      *t = l;
    } else if (l->pri > r->pri) {
      merge(&l->r, l->r, r);
      *t = l;
    } else {
      merge(&r->l, l, r->l);
      *t = r;
    }
    update_cnt(*t);
  }

  // Inserează cheia key în subarborele lui t și scrie în t noua rădăcină.
  void insert(data_structure** t, data_structure* elem) {
    if (!*t) {
      *t = elem;
    } else if (elem->pri > (*t)->pri) {
      split(*t, elem->key, &elem->l, &elem->r);
      *t = elem;
    } else {
      insert((*t)->key <= elem->key ? &(*t)->r : &(*t)->l, elem);
    }
    update_cnt(*t);
  }

  void insert(int key) {
    data_structure* elem = new data_structure(key);
    insert(&this->r, elem);
  }

  // Șterge cheia key din subarborele lui t și scrie în t noua rădăcină.
  void erase(data_structure** t, int key) {
    if ((*t)->key == key) {
      merge(t, (*t)->l, (*t)->r);
    } else {
      erase((key < (*t)->key) ? &(*t)->l : &(*t)->r, key);
    }
    update_cnt(*t);
  }

  void erase(int key) {
    erase(&this->r, key);
  }
};
