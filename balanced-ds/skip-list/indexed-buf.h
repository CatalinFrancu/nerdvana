const int MAX_NODES = 300'000;
const int INF = 2'000'000'000;
const char METHOD[] = "buffer global";

struct node {
  int val;
  int height;
  int* next;
  int* dist;
};

struct skip_list {
  node a[MAX_NODES + 2];
  int buf[MAX_NODES * 5], buf_ptr;
  int prev[MAX_LEVELS], prev_ord[MAX_LEVELS];
  int size;

  void init() {
    size = 2;
    a[0].val = -INF;
    a[1].val = +INF;
    a[0].height = a[1].height = MAX_LEVELS;
    a[0].next = buf;
    a[0].dist = buf + MAX_LEVELS;
    buf_ptr = 2 * MAX_LEVELS;
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
    int h = get_height();
    a[size].height = h;
    a[size].next = buf + buf_ptr;
    a[size].dist = buf + buf_ptr + h;
    buf_ptr += 2 * h;

    int pos = 0, order = 0;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[a[pos].next[l]].val < val) {
        order += a[pos].dist[l];
        pos = a[pos].next[l];
      }

      a[pos].dist[l]++;
      if (l < h) {
        a[size].next[l] = a[pos].next[l];
        a[pos].next[l] = size;
        prev[l] = pos;
        prev_ord[l] = order;
      }
    }

    order++;
    for (int l = 0; l < h; l++) {
      a[size].dist[l] = a[prev[l]].dist[l] - (order - prev_ord[l]);
      a[prev[l]].dist[l] = order - prev_ord[l];
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
