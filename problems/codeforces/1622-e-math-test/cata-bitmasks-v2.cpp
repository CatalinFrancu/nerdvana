// In addition to v1, we can get rid of the list array. We use it in order to
// build the permutation, but that is wasted effort. We only need to do this
// once at the end, once we determine the student mask that maximizes the
// surprise.
//
// What we do need is the distribution of questions by fitness scores, from -n
// to +n. Assume this distribution is (3 5 2 10 4) (in this example,
// n=2). Then questions with a fitness score of -2 will receive 1, 2 and 3
// points respectively. Questions with a fitness score of -1 will receive 4,
// 5, 6, 7 and 8 points respectively. Therefore, it suffices to compute
// partial sums over the distribution to get (3 8 10 20 24). Then we iterate
// over the questions and attribute points according to fitness scores.
#include <stdio.h>

const int MAX_STUDENTS = 10;
const int MAX_QUESTIONS = 10'000;

short solvers[MAX_QUESTIONS];
int expect[MAX_STUDENTS];
int n, m;

struct surprise {
  int freq[2 * MAX_STUDENTS + 1]; // positions from -n to +n, offset by +n
  int score;

  void init(int stud_mask) {
    score = 0;
    for (int i = 0; i < n; i++) {
      int coef = (stud_mask & (1 << i)) ? -1 : +1;
      score += coef * expect[i];
    }

    for (int i = 0; i <= 2 * n; i++) {
      freq[i] = 0;
    }
  }

  void add(int val) {
    freq[val + n]++;
  }

  int get_score() {
    int first = 1;

    for (int i = 0; i <= 2 * n; i++) {
      int last = first + freq[i] - 1;
      score += (i - n) * (last + first) * freq[i] / 2;
      first += freq[i];
    }

    return score;
  }

  void partial_sums() {
    for (int i = 1; i <= 2 * n; i++) {
      freq[i] += freq[i - 1];
    }
  }

  int get_and_decrease(int val) {
    return freq[val + n]--;
  }
};

surprise surp;

void read_data() {
  scanf("%d %d", &n, &m);
  for (int i = 0; i < n; i++) {
    scanf("%d ", &expect[i]);
  }
  for (int i = 0; i < m; i++) {
    solvers[i] = 0;
  }
  for (int stud = 0; stud < n; stud++) {
    for (int q = 0; q < m; q++) {
      int bit = getchar() - '0';
      solvers[q] |= bit << stud;
    }
    getchar();
  }
}

int get_fit_score(int solvers, int stud_mask) {
  int good = __builtin_popcount(solvers & stud_mask);
  int bad = __builtin_popcount(solvers & ~stud_mask);
  return good - bad;
}

int get_student_mask_score(int stud_mask) {
  surp.init(stud_mask);
  for (int q = 0; q < m; q++) {
    int score = get_fit_score(solvers[q], stud_mask);
    surp.add(score);
  }
  return surp.get_score();
}

int try_all_student_masks() {
  int best_mask = 0;
  int best_score = -1;

  for (int stud_mask = 0; stud_mask < (1 << n); stud_mask++) {
    int score = get_student_mask_score(stud_mask);
    if (score > best_score) {
      best_mask = stud_mask;
      best_score = score;
    }
  }

  return best_mask;
}

void gen_and_write_perm(int mask) {
  get_student_mask_score(mask);
  surp.partial_sums();
  for (int q = 0; q < m; q++) {
    int score = get_fit_score(solvers[q], mask);
    int points = surp.get_and_decrease(score);
    printf("%d ", points);
  }
  printf("\n");
}

void solve_test() {
  read_data();
  int best_mask = try_all_student_masks();
  gen_and_write_perm(best_mask);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test();
  }

  return 0;
}
