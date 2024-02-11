const int MAX_NODES = 300'000;
const int INF = 2'000'000'000;

struct node {
  int val;
  int height;
  int next[MAX_LEVELS];
  int dist[MAX_LEVELS];
};

struct skip_list {
  node a[MAX_NODES + 2];
  int size;

  void init() {
    size = 2;
    a[0].val = -INF;
    a[1].val = +INF;
    a[0].height = a[1].height = MAX_LEVELS;
    for (int l = 0; l < MAX_LEVELS; l++) {
      a[0].next[l] = 1;
      a[0].dist[l] = 1;
    }
  }

  int get_height() {
    int h = 1;
    while ((rand() & 1) && (h < MAX_LEVELS)) {
      h++;
    }
    return h;
  }

  void insert(int val) {
    a[size].val = val;
    a[size].height = get_height();

    int pos = 0, order = 0;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[a[pos].next[l]].val < val) {
        order += a[pos].dist[l];
        pos = a[pos].next[l];
      }
      a[size].next[l] = pos;
      a[size].dist[l] = order;
    }

    order++;
    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      int prev = a[size].next[l];
      if (l < a[size].height) {
        a[size].next[l] = a[prev].next[l];
        a[prev].next[l] = size;
        int old_dist = a[prev].dist[l];
        a[prev].dist[l] = order - a[size].dist[l];
        a[size].dist[l] = old_dist - a[prev].dist[l] + 1;
      } else {
        a[prev].dist[l]++;
      }
    }

    size++;
  }

  bool contains(int val) {
    int pos = 0;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[a[pos].next[l]].val < val) {
        pos = a[pos].next[l];
      }
    }

    return (a[a[pos].next[0]].val == val);
  }

  int order_of(int val) {
    int pos = 0, order = 0;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[a[pos].next[l]].val < val) {
        order += a[pos].dist[l];
        pos = a[pos].next[l];
      }
    }

    return order;
  }

  int kth_element(int k) {
    int pos = 0;
    k++;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[pos].dist[l] <= k) {
        k -= a[pos].dist[l];
        pos = a[pos].next[l];
      }
    }

    return a[pos].val;
  }
};
