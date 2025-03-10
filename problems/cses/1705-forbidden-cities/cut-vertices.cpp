// The basic question we need to answer is: if we remove vertex C, and if the
// graph becomes disconnected, will A and B fall into the same component?
//
// We could build the condensed biconnected component tree and perform LCA
// queries on it, which sounds ungodly. Instead, we use a notification system
// to achieve O(m + n) time.
//
// Consider query #k = <A, B, C>. When we begin exploring node C, we set the
// notify_me flag on C to register it for notifications from A and B (and any
// other endpoints for which C is a center). When we leave node C we unset the
// flag.
//
// Whenever we explore nodes A and B, and if C is currently registered for
// notifications, we notify it by pushing the query index k into C's
// notification list.
//
// After each of its DFS children, v, node C first considers whether v causes
// C to be a cut vertex. We use the usual Tarjan method: C is a cut vertex if
// v can climb no higher than C in DFS.
//
// If that is the case, then C evaluates its notifications. If exactly one of
// A or B was a descendant of v (as measured by the DFS times tin and tout),
// then C blocks the path from A to B.
//
// Regardless of the results, C clears its notifications before recursing on
// the next DFS child.
#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_EDGES = 200'000;
const int MAX_QUERIES = 100'000;

struct cell {
  int val, next;
};

struct node {
  int adj;           // my adjacency list
  int queries;       // queries in which I am an endpoint
  int notifications; // queries in which I am the center
  int tin, tout;
  int low;
  bool notify_me;
};

struct query {
  int a, b, c;
  bool red_flag;
};

node nd[MAX_NODES + 1];
cell edge_buf[2 * MAX_EDGES + 1];
cell query_buf[2 * MAX_QUERIES + 1];
cell notif_buf[2 * MAX_QUERIES + 1];
query q[MAX_QUERIES + 1];
int n, num_queries;

void add_edge(int u, int v) {
  static int pos = 1;
  edge_buf[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void add_query(int u, int qindex) {
  static int pos = 1;
  query_buf[pos] = { qindex, nd[u].queries };
  nd[u].queries = pos++;
}

void add_notification(int u, int qindex) {
  static int pos = 1;
  notif_buf[pos] = { qindex, nd[u].notifications };
  nd[u].notifications = pos++;
}

void read_data() {
  int num_edges, u, v;
  scanf("%d %d %d", &n, &num_edges, &num_queries);

  while (num_edges--) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }

  for (int i = 1; i <= num_queries; i++) {
    scanf("%d %d %d", &q[i].a, &q[i].b, &q[i].c);
    add_query(q[i].a, i);
    add_query(q[i].b, i);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

bool is_ancestor(int u, int v) {
  return (nd[v].tin >= nd[u].tin) && (nd[v].tout <= nd[u].tout);
}

void send_notifications(int u) {
  for (int ptr = nd[u].queries; ptr; ptr = query_buf[ptr].next) {
    int ind = query_buf[ptr].val;
    int c = q[ind].c;
    if (nd[c].notify_me) {
      add_notification(c, ind);
    }
  }
}

void receive_notifications(int u, int child) {
  for (int ptr = nd[u].notifications; ptr; ptr = notif_buf[ptr].next) {
    int ind = notif_buf[ptr].val;
    if (is_ancestor(child, q[ind].a) ^ is_ancestor(child, q[ind].b)) {
      q[ind].red_flag = true;
    }
  }
}

void clear_notifications(int u) {
  nd[u].notifications = 0;
}

void dfs(int u, int parent) {
  static int time = 0;
  nd[u].tin = nd[u].low = ++time;
  nd[u].notify_me = true;

  for (int ptr = nd[u].adj; ptr; ptr = edge_buf[ptr].next) {
    int v = edge_buf[ptr].val;
    if (!nd[v].tin) {
      dfs(v, u);
      nd[u].low = min(nd[u].low, nd[v].low);

      if (nd[v].low >= nd[u].tin) {
        // v has no way around u: u is a cut vertex
        receive_notifications(u, v);
      }
      clear_notifications(u);

    } else if (v != parent) {
      nd[u].low = min(nd[u].low, nd[v].tin);
    }
  }

  send_notifications(u);
  nd[u].notify_me = false;
  nd[u].tout = ++time;
}

void write_answers() {
  for (int i = 1; i <= num_queries; i++) {
    q[i].red_flag |= (q[i].a == q[i].c) || (q[i].b == q[i].c);
    printf(q[i].red_flag ? "NO\n" : "YES\n");
  }
}

int main() {
  read_data();
  dfs(1, 0);
  write_answers();

  return 0;
}
