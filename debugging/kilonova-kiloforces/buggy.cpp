#include <algorithm>
#include <iostream>
#include <set>

using namespace std;

const int BLOCK_SIZE = 300;
const int MAX_N = 1e5;
const int MAX_Q = 1e5;

struct query {
    int l, r;
    int pos;
};

int N, Q;

int p[MAX_N], t[MAX_N];
int prefix_sum[MAX_N];

query queries[MAX_Q];

int ans[MAX_Q];

void read() {
    cin >> N >> Q;
    for (int i = 0; i < N; ++i) {
        cin >> p[i];
    }
    for (int i = 0; i < N; ++i) {
        cin >> t[i];
    }
    for (int i = 0; i < Q; ++i) {
        cin >> queries[i].l >> queries[i].r;
        --queries[i].l;
        --queries[i].r;
        queries[i].pos = i;
    }
}

void build_prefix_sum() {
    prefix_sum[0] = p[0];
    for (int i = 1; i < N; ++i) {
        prefix_sum[i] = prefix_sum[i - 1] + p[i];
    }
}

void sort_queries() {
    sort(queries, queries + Q, [](query a, query b) {
        int block_a = a.l / BLOCK_SIZE;
        int block_b = b.l / BLOCK_SIZE;
        if (block_a != block_b) {
            return block_a < block_b;
        }
        else if (block_a & 1) {
            return a.r > b.r;
        }
        else {
            return a.r < b.r;
        }
    });
}

struct block_data {
    set<int> penalties;
    int points(int l, int r) {
        if (l) {
            return prefix_sum[r] - prefix_sum[l - 1];
        }
        return prefix_sum[r];
    }
    int penalty(int l, int r) {
        int val = 0;
        int count = r - l;
        for (auto x : penalties) {
            val += x * count;
            --count;
        }
        return val;
    }
    int score(int l, int r) {
        return points(l, r) - penalty(l, r);
    }
};

void mo() {
    block_data data;
    int left = queries[0].l;
    int right = queries[0].r;
    for (int i = left; i <= right; ++i) {
        data.penalties.insert(t[i]);
    }
    for (int i = 0; i < Q; ++i) {
        while (left < queries[i].l) {
            data.penalties.erase(t[left++]);
        }
        while (left > queries[i].l) {
            data.penalties.insert(t[--left]);
        }
        while (right < queries[i].r) {
            data.penalties.insert(t[++right]);
        }
        while (right > queries[i].r) {
            data.penalties.erase(t[--right]);
        }
        ans[queries[i].pos] = data.score(left, right);
    }
}

void write() {
    for (int i = 0; i < Q; ++i) {
        cout << ans[i] << '\n';
    }
}

int main() {
    read();
    build_prefix_sum();
    sort_queries();
    mo();
    write();
    return 0;
}