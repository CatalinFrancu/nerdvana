// Method:
//
// 1. Let t[i][j] bne the number of thieves still allowed past alarm i while
// carrying weight j.
//
// 2. Run the knapsack algorithm for each thief in turn while obeying t[][].
//
// 3. Once a maximum is found, trace back the path and decrement t[][] along it.
//
// This is BUGGY! It misses the following situation: 2 rooms, 3 thieves,
// knapsack size 2.
// Room 1: value 5, weight 1, alarm 1
// Room 2: value 100, weight 1, alarm 1
//
// Optimal solution: the three thieves collect 0, 1 and 2 items respectively
// from room 1. They collect nothing from room 2.
//
// Our solution: Thief 1 collects 0 and 2 items respectively for profit
// 200. Thief 2 must collect 1 item from room 1, for profit 5 (that is the
// only path that does not violate constraints in t[][]). Thief 3 has no valid
// paths. Our program returns -1.
#include <stdio.h>

#define MAX_N 300
#define MAX_W 300
#define BAD -1

// values, weights, alarms
int v[MAX_N], w[MAX_N], a[MAX_N];
int rooms, thieves, knapsack;

// t[i][j] = number of thieves still allowed in room i with knapsack weight j
int t[MAX_N + 1][MAX_W + 1];

// c[i][j] = dynamic programming data, filled once per thief
int c[MAX_N + 1][MAX_W + 1];

void dynamic_programming() {
  c[0][0] = 0;
  for (int room = 0; room <= rooms; room++) {
    for (int weight = room ? 0 : 1; weight <= knapsack; weight++) {
      // take an object
      int x = ((room < rooms) && (weight >= w[room]) && (c[room][weight - w[room]] != BAD))
        ? (v[room] + c[room][weight - w[room]])
        : BAD;

      // advance from the previous room
      int y = (room && t[room - 1][weight])
        ? c[room - 1][weight]
        : BAD;

      c[room][weight] = (x > y) ? x : y;
    }
  }

  // for (int i = 0; i <= rooms; i++) {
  //   for (int j = 0; j <= knapsack; j++) {
  //     printf("%d ", c[i][j]);
  //   }
  //   printf("\n");
  // }
  // printf("\n");

}

// Interprets the DP table to produce a path for the current thief. Decrements
// values in t[][] along the way. Returns the profit of the path (which can be
// BAD).
int path() {
  int weight = 0;
  for (int i = 1; i <= knapsack; i++) {
    if (c[rooms][i] > c[rooms][weight]) {
      weight = i;
    }
  }

  if (c[rooms][weight] == BAD) {
    return BAD;
  } else {
    int result = 0, room = rooms;
    while (room || weight) {
      // printf("In room %d with weight %d\n", room, weight);
      if (room &&
          t[room - 1][weight] &&
          c[room - 1][weight] == c[room][weight]) {
        t[--room][weight]--;
      } else {
        result += v[room];
        weight -= w[room];
      }
    }
    return result;
  }
}

int main() {
  int num_tests;

  scanf("%d", &num_tests);
  while (num_tests--) {
    // read the data
    scanf("%d %d %d", &rooms, &thieves, &knapsack);
    for (int i = 0; i < rooms; i++) {
      scanf("%d %d %d", &v[i], &w[i], &a[i]);
    }

    for (int i = 0; i < rooms; i++) {
      for (int j = 0; j <= knapsack; j++) {
        t[i][j] = a[i];
      }
    }

    int sum = 0;
    bool bad = false;
    int i = 0;
    while ((i < thieves) && !bad) {
      dynamic_programming();
      int s = path();
      if (s == BAD) {
        bad = true;
      } else {
        sum += s;
      }
      i++;
    }

    printf("%d\n", bad ? BAD : sum);
  }
  return 0;
}
