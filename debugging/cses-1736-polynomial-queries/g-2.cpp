#include <iostream>
#include <vector>

using namespace std;

const int nmax = 2e5;

vector<int> v(nmax + 3);

int64_t sum_prog(int64_t start, int64_t ratie, int lung) {
  return start * lung + ratie * (lung - 1) * lung / 2;
}

struct AInt {
  struct node {
    int64_t sum = 0, start = 0, ratie = 0;
  };
  vector<node>aint;
  AInt() {
    aint.resize(4 * nmax + 3);
  }
  void build(int nod, int st, int dr) {
    if(dr < st)
      return;
    if(st == dr) {
      aint[nod].sum = v[st];
    } else {
      int mid = (st + dr) / 2;
      build(nod * 2, st, mid);
      build(nod * 2 + 1, mid + 1, dr);
      aint[nod].sum = aint[nod * 2].sum + aint[nod * 2 + 1].sum;
    }
  }
  void push(int nod, int st, int dr) {
    cerr << "Push la nodul " << nod << "(" << aint[nod].sum
         << " " << aint[nod].start << " " << aint[nod].ratie << "\n";
    if(st > dr)
      return;
    if(st == dr) {
      if(aint[nod].start > 0)
        aint[nod].sum += aint[nod].start;
      aint[nod].start = aint[nod].ratie = 0;
      return;
    }
    int mid = (st + dr) / 2;
    aint[nod].sum += sum_prog(aint[nod].start, aint[nod].ratie, dr - st + 1);
    if(aint[nod].start > 0) {
      cerr << "fiul drept start cîștigă " << aint[nod].start
           << " + " << (mid + 1 - st) << "\n";
      aint[nod * 2].start += aint[nod].start;
      aint[nod * 2 + 1].start += aint[nod].start + (mid + 1 - st) * aint[nod].ratie;
    }
    aint[nod * 2].ratie += aint[nod].ratie;
    aint[nod * 2 + 1].ratie += aint[nod].ratie;
    aint[nod].start = aint[nod].ratie = 0;
    print();
  }
  void update(int nod, int st, int dr, int l, int r) {
    if(dr < st || l > dr || r < st)
      return;
    if(l <= st && dr <= r) {
      aint[nod].start += (st - l + 1);
      aint[nod].ratie++;
      // push(nod, st, dr);
    } else {
      push(nod, st, dr);
      int mid = (st + dr) / 2;
      update(nod * 2, st, mid, l, r);
      update(nod * 2 + 1, mid + 1, dr, l, r);
      // push(nod * 2, st, mid);
      // push(nod * 2 + 1, mid + 1, dr);
      aint[nod].sum =
        aint[nod * 2].sum +
        aint[nod * 2 + 1].sum +
        sum_prog(aint[nod * 2].start, aint[nod * 2].ratie, mid - st + 1) +
        sum_prog(aint[nod * 2 + 1].start, aint[nod * 2 + 1].ratie, dr - mid);
    }
  }
  int64_t query(int nod, int st, int dr, int l, int r) {
    if(dr < st || l > r || l > dr || r < st)
      return 0;
    push(nod, st, dr);
    if(st == l && dr == r) {
      return aint[nod].sum;
    } else {
      int mid = (st + dr) / 2;
      return query(nod * 2, st, mid, l, min(r, mid)) + query(nod * 2 + 1, mid + 1, dr, max(mid + 1, l), r);
    }
  }

  void print() {
    for (int i = 1; i <= 12; i++) {
      cerr << "(" << aint[i].sum << " " << aint[i].start << " "
           << aint[i].ratie << ") ";
      if ((i & (i + 1)) == 0) {
        cerr << "| ";
      }
    }
    cerr << "\n";
  }
};

AInt myAint;
int cer, st, dr, x, n, q;

void read() {
  cin >> n >> q;
  for(int i = 1; i <= n; i++)
    cin >> v[i];
}

void solve() {
  myAint.build(1, 1, n);
  myAint.print();
  for(int i = 1; i <= q; i++) {
    cin >> cer >> st >> dr;
    if(cer == 1) {
      myAint.update(1, 1, n, st, dr);
      cerr << "După update:\n";
      myAint.print();
    } else {
      cout << myAint.query(1, 1, n, st, dr) << "\n";
    }
  }
}

int main() {
  read();
  solve();
  return 0;
}
