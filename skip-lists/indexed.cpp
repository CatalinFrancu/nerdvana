#include <assert.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unordered_set>

typedef __gnu_pbds::tree<
  int,
  __gnu_pbds::null_type,
  std::less<int>,
  __gnu_pbds::rb_tree_tag,  
  __gnu_pbds::tree_order_statistics_node_update
  > ordered_set;

const int MAX_LEVELS = 20;
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
    a[0].height = a[1].height = MAX_LEVELS;
    a[0].val = -INF;
    a[1].val = +INF;
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
      // For now, store the origin of the incoming pointer and its order.
      a[size].next[l] = pos;
      a[size].dist[l] = order;
    }

    // Now that we know our own order, update the incoming/outgoing info.
    order++;
    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      int prev = a[size].next[l];
      if (l < a[size].height) {
        // intercept the pointer
        a[size].next[l] = a[prev].next[l];
        a[prev].next[l] = size;
        // split the distance
        int old_dist = a[prev].dist[l];
        a[prev].dist[l] = order - a[size].dist[l];
        a[size].dist[l] = old_dist - a[prev].dist[l] + 1;
      } else {
        // the pointer passes over our head
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

  // Assumes that val exists and is unique.
  int order_of(int val) {
    int pos = 0, order = 0;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[a[pos].next[l]].val < val) {
        order += a[pos].dist[l];
        pos = a[pos].next[l];
      }
    }

    // We should return the next element, but we skip the left sentinel, so...
    return order;
  }

  int kth_element(int k) {
    int pos = 0;
    k++; // skip the left sentinel

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[pos].dist[l] <= k) {
        k -= a[pos].dist[l];
        pos = a[pos].next[l];
      }
    }

    return a[pos].val;
  }
};

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);
}

skip_list sl;
ordered_set stl_set;

int main() {
  init_rng();

  sl.init();

  for (int i = 0; i < MAX_NODES; i++) {
    int x = rand() % INF;
    // For simplicity, don't deal with multisets.
    if (!sl.contains(x)) {
      sl.insert(x);
      stl_set.insert(x);
      assert(sl.order_of(x) == (int)stl_set.order_of_key(x));
    }
  }
  
  for (int i = stl_set.size() - 1; i >= 0; i--) {
    assert(sl.kth_element(i) == *stl_set.find_by_order(i));
  }

  return 0;
}
