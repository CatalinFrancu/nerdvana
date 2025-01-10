// Complexity: O(q log^2 n).
//
// Method: The heavy-light decomposition from hell. Comments at the end.
#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_SEGTREE_NODES = 1 << 19;
const int INFINITY = 3 * MAX_NODES;
const int OP_TOGGLE = 1;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int parent;
  int heavy;       // the child with the largest subtree
  int head;        // the top of our path
  int light_start; // start of the rest of the subtree, after the heavy path
  int d;           // depth
  int tin, tout;   // DFS entry/exit times
};

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

struct segment_tree {
  int v[MAX_SEGTREE_NODES];
  int n;

  void init(int n) {
    this->n = next_power_of_2(n);
    for (int i = 1; i < 2 * this->n; i++) {
      v[i] = INFINITY;
    }
  }

  void set(int pos, int val) {
    pos += n;
    v[pos] = val;
    for (pos >>= 1; pos; pos >>= 1) {
      v[pos] = min(v[2 * pos], v[2 * pos + 1]);
    }
  }

  int get(int pos) {
    return v[pos + n];
  }

  int get_min(int l, int r) {
    int result = INFINITY;
    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        result = min(result, v[l++]);
      }
      l >>= 1;

      if (!(r & 1)) {
        result = min(result, v[r--]);
      }
      r >>= 1;
    }

    return result;
  }

  int get_min_plus(int l, int r, int delta) {
    int x = get_min(l, r);
    return (x == INFINITY) ? INFINITY : (x + delta);
  }
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
segment_tree seg, seg_diff, seg_2diff;
int n, num_ops;

void add_edge(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree() {
  scanf("%d %d", &n, &num_ops);
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

// Compute parents, depths and heavy children.
int heavy_dfs(int u) {
  int my_size = 1, max_child_size = 0;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent) {

      nd[v].parent = u;
      nd[v].d = 1 + nd[u].d;
      int child_size = heavy_dfs(v);
      my_size += child_size;
      if (child_size > max_child_size) {
        max_child_size = child_size;
        nd[u].heavy = v;
      }

    }
  }

  return my_size;
}

// Flatten the tree.
void decompose_dfs(int u, int head) {
  static int time = 0;

  nd[u].head = head;
  nd[u].tin = time++;

  if (nd[u].heavy) {
    decompose_dfs(nd[u].heavy, head);
  }
  nd[u].light_start = time;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent && v != nd[u].heavy) {
      decompose_dfs(v, v); // start a new path
    }
  }

  nd[u].tout = time - 1;
}

void init_segment_trees() {
  seg.init(n);
  seg_diff.init(n);
  seg_2diff.init(n);
}

void toggle(int u) {
  int was_off = seg.get(nd[u].tin) == INFINITY;
  int val = was_off ? nd[u].d : INFINITY;
  seg.set(nd[u].tin, val);

  while (u) {
    seg_diff.set(nd[u].tin, (val == INFINITY) ? INFINITY : (val - nd[u].d));
    seg_2diff.set(nd[u].tin, (val == INFINITY) ? INFINITY : (val - 2 * nd[u].d));
    u = nd[nd[u].head].parent;
    if (u) {
      val = seg.get_min(nd[u].light_start, nd[u].tout);
    }
  }
}

// Find the minimum distance to a leaf when traveling up from u.
int up_query(int u) {
  int leaf_dist = INFINITY;
  int orig_u = u;

  while (u) {
    int h = nd[u].head;
    int on_path_h_u = seg_2diff.get_min_plus(nd[h].tin, nd[u].tin, nd[orig_u].d);
    int on_path_below_u = seg.get_min_plus(nd[u].tin, nd[u].tout, nd[orig_u].d - 2 * nd[u].d);
    leaf_dist = min(leaf_dist, on_path_h_u);
    leaf_dist = min(leaf_dist, on_path_below_u);
    u = nd[h].parent;
  }

  return leaf_dist;
}

// Find the minimum downward distance to a leaf from any node on the heavy
// path u-v.
int path_score(int u, int v) {
  int on_path_u_v = seg_diff.get_min(nd[u].tin, nd[v].tin);
  int in_subtree_of_v = seg.get_min_plus(nd[v].tin, nd[v].tout, -nd[v].d);
  return min(on_path_u_v, in_subtree_of_v);
}

int query(int u, int v) {
  int orig_u = u, orig_v = v;

  int leaf_dist = INFINITY;
  // Lift u or v while they are on different paths.
  while (nd[u].head != nd[v].head) {
    if (nd[v].tin > nd[u].tin) {
      int tmp = u; u = v; v = tmp;
    }
    int h = nd[u].head;
    leaf_dist = min(leaf_dist, path_score(h, u));
    u = nd[h].parent;
  }

  // One last operation when u and v are on a common path.
  if (nd[u].tin > nd[v].tin) {
    int tmp = u; u = v; v = tmp;
  }
  leaf_dist = min(leaf_dist, path_score(u, v));

  leaf_dist = min(leaf_dist, up_query(u));

  if (leaf_dist == INFINITY) {
    return -1;
  } else {
    int dist_uv = nd[orig_u].d + nd[orig_v].d - 2 * nd[u].d;
    return dist_uv + 2 * leaf_dist;
  }
}

void process_ops() {
  int type, u, v;

  while (num_ops--) {
    scanf("%d %d", &type, &u);
    if (type == OP_TOGGLE) {
      toggle(u);
    } else {
      scanf("%d", &v);
      printf("%d\n", query(u, v));
    }
  }
}

int main() {
  read_tree();
  heavy_dfs(1);
  decompose_dfs(1, 1);
  init_segment_trees();
  process_ops();

  return 0;
}

// The shortest route G_u to H_v will travel from u to v, but take a detour
// from somewhere along the path to an active leaf and back. Therefore,
// reformulate queries as "find the distance u-v plus twice the shortest
// distance to any active leaf from any node on the u-v path".
//
// To find the closest leaf, root the tree at node 1 and perform HLD. By
// definition, the shortest path from a node to a leaf may travel (1) only
// downward or (2) first upward, then downward. However, given a path u-v, it
// only makes sense to travel upward from the LCA. Travelling upward from a
// node u ≠ LCA will leave us at some other node u', so we could simply travel
// downward from u'.
//
// Therefore, the query part of the problem has two parts.
//
// Part I: The leaf can be found by traveling downward.
//
// Starting from u and v, we will query O(log n) paths. Consider a chunk of a
// path, (w,u). Typically, w is the start node of the path, though that may
// not be the case for the topmost path, in which case w is the LCA. Also, u
// is either the starting point of the query or the point where the previous
// (lower) path anchors.
//
// The path to the closest leaf may start from a node on w-u. In this case the
// distance to the leaf is the minimum difference between that start node's
// depth and the leaf's depth. The start node is unknown (we wish to avoid
// searching for it in linear time). So we will use a segment tree that stores
// values of the form (leaf_depth - node_depth) and allows range minimum
// queries.
//
// Example: we query a 6-node path spanning levels 15 through 20. The minimum
// difference reported on this path is 10. It could mean that the node at
// level 16 has a leaf at level 26 or the node at level 18 has a leaf at level
// 28 etc. In any case, we need to stray a distance of 10 from the path to a
// leaf.
//
// Otherwise, the path may start in v's subtree. In that case, the distance we
// need to travel is the difference between the leaf's level and v's level. We
// know that flattening the tree maps v's subtree to a contiguous interval.
// Therefore, we need a second segment tree that simply stores leaf depths,
// also with range minimum queries.
//
// Part II: The leaf can be found by travelling upward from the LCA.
//
// This time we travel all the way to the root, so all our queries will be on
// ranges (w, u), where w is the path's start and u is initially the LCA, then
// the node where the previous path anchors. Again, two cases arise.
//
// The path to some leaf f may start at some anchor a lying betwen w and u.
// Then we need to travel from LCA to a and from a to f. The total distance is
// [d(LCA)-d(a)] + [d(f) - d(a)] = d(LCA) + [d(f) - 2*d(a)]. Therefore, we
// need a third segment tree storing values of the form (leaf_depth -
// 2*node_depth).
//
// Otherwise, the path to f may start in u's subtree, which we have discussed
// above.
//
// A word about toggling the state of a leaf. We need to set the corresponding
// segment tree position to either the leaf's depth (if it became active) or
// to +∞ (if it became inactive). We then need to recompute the value of the
// node where the leaf's heavy path anchors to the next heavy path, then the
// next and so on.
//
// One caveat, however: when we recompute node u's value, we should query u's
// subtree *without* the heavy path from u. That is because deactivating a
// leaf does not notify the leaf's own path, only paths above it.
