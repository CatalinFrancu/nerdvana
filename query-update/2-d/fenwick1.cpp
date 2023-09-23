// Complexity: O(Q log N)
//
// Method: Fenwick trees
#include "common.h"

// Transforms a regular array into a Fenwick tree
// https://stackoverflow.com/a/31070683/6022817, adapted for 2-D
void fenwick_build() {
  for (int r = 1; r <= n; r++) {
    for (int c = 1; c <= n; c++) {
      int next_r = r + (r & -r);
      int next_c = c + (c & -c);

      if (next_r <= n) {
        mat[next_r][c] += mat[r][c];
      }

      if (next_c <= n) {
        mat[r][next_c] += mat[r][c];
      }

      if ((next_r <= n) && (next_c <= n)) {
        mat[next_r][next_c] -= mat[r][c];
      }
    }
  }
}

void fenwick_add(int row, int col, int val) {
  for (int r = row; r <= n; r += r & -r) {
    for (int c = col; c <= n; c += c & -c) {
      mat[r][c] += val;
    }
  }
}

long long fenwick_sum(int row, int col) {
  long long s = 0;

  for (int r = row; r; r &= r - 1) {
    for (int c = col; c; c &= c - 1) {
      s += mat[r][c];
    }
  }

  return s;
}

// Returns the sum of the inclusive rectangle.
long long fenwick_rectangle_sum(int row1, int col1, int row2, int col2) {
  return
    + fenwick_sum(row2, col2)
    - fenwick_sum(row2, col1 - 1)
    - fenwick_sum(row1 - 1, col2)
    + fenwick_sum(row1 - 1, col1 - 1);
}

void process_ops() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].t == OP_UPDATE) {
      fenwick_add(q[i].row1, q[i].col1, q[i].val);
    } else {
      answer[num_answers++] =
        fenwick_rectangle_sum(q[i].row1, q[i].col1, q[i].row2, q[i].col2);
    }
  }
}

int main() {

  read_data();
  mark_time();

  fenwick_build();
  process_ops();

  report_time("Fenwick tree, 1-based");
  write_data();

  return 0;
}
