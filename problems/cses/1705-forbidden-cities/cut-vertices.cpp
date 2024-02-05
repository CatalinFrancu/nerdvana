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
// A or B was a descendant of v (as measured by the DFS times time_in and
// time_out), then C blocks the path from A to B.
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
  int time_in, time_out;
  int low;
  bool notify_me;
};

struct query {
  int a, b, c;
  bool red_flag;
};

node n[MAX_NODES + 1];
cell edge_buf[2 * MAX_EDGES + 1];
cell query_buf[2 * MAX_QUERIES + 1];
cell notif_buf[2 * MAX_QUERIES + 1];
query q[MAX_QUERIES + 1];
int num_nodes, num_queries, time;

void add_edge(int u, int v) {
  static int pos = 1;
  edge_buf[pos] = { v, n[u].adj };
  n[u].adj = pos++;
}

void add_query(int u, int qindex) {
  static int pos = 1;
  query_buf[pos] = { qindex, n[u].queries };
  n[u].queries = pos++;
}

void add_notification(int u, int qindex) {
  static int pos = 1;
  notif_buf[pos] = { qindex, n[u].notifications };
  n[u].notifications = pos++;
}

void read_data() {
  int num_edges, u, v;
  scanf("%d %d %d", &num_nodes, &num_edges, &num_queries);

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

bool inside(int child, int parent) {
  return (n[child].time_in >= n[parent].time_in) &&
    (n[child].time_out <= n[parent].time_out);
}

void send_notifications(int u) {
  for (int ptr = n[u].queries; ptr; ptr = query_buf[ptr].next) {
    int ind = query_buf[ptr].val;
    int c = q[ind].c;
    if (n[c].notify_me) {
      add_notification(c, ind);
    }
  }
}

void receive_notifications(int u, int child) {
  for (int ptr = n[u].notifications; ptr; ptr = notif_buf[ptr].next) {
    int ind = notif_buf[ptr].val;
    if (inside(q[ind].a, child) ^ inside(q[ind].b, child)) {
      q[ind].red_flag = true;
    }
  }
}

void clear_notifications(int u) {
  n[u].notifications = 0;
}

void dfs(int u, int parent) {
  n[u].time_in = n[u].low = ++time;
  n[u].notify_me = true;

  for (int ptr = n[u].adj; ptr; ptr = edge_buf[ptr].next) {
    int v = edge_buf[ptr].val;
    if (!n[v].time_in) {
      dfs(v, u);
      n[u].low = min(n[u].low, n[v].low);

      if (n[v].low >= n[u].time_in) {
        // v has no way around u: u is a cut vertex
        receive_notifications(u, v);
      }
      clear_notifications(u);

    } else if (v != parent) {
      n[u].low = min(n[u].low, n[v].time_in);
    }
  }

  send_notifications(u);
  n[u].notify_me = false;
  n[u].time_out = ++time;
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
