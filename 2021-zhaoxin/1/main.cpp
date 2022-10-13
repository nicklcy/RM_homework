// This code is written by lcy.

#include <iostream>
#include <vector>
#include <queue>
#include <variant>
#include <functional>
using namespace std;

enum OperatorType { // 操作类型
    Create, Add, Merge, Split, Remove, Clear, Quit, False
};

enum OperatorType read_type() { // 读取操作类型
    string str;
    cin >> str;
    if (str == "create") {
        return Create;
    } else if (str == "add") {
        return Add;
    } else if (str == "merge") {
        return Merge;
    } else if (str == "split") {
        return Split;
    } else if (str == "remove") {
        return Remove;
    } else if (str == "clear") {
        return Clear;
    } else if (str == "quit") {
        return Quit;
    } else {
        cerr << "Error! There is no such operation as " << str << "." << endl;
        return False;
    }
}

int n, m;

struct Grid {
    string val;
    int i1, j1, i2, j2;
    Grid() {}
    void set(int i, int j) { i1 = i2 = i; j1 = j2 = j; }
    Grid(int i, int j) { set(i, j); }
};

vector<vector<Grid> > grid;

void create_grid() {
    grid = vector<vector<Grid>>(n, vector<Grid>(m));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            grid[i][j] = Grid(i, j);
        }
    }
}

void do_create() { // create
    cin >> n >> m;
    if (n <= 0 || m <= 0) {
        cerr << "Error! n and m should be greater than 0." << endl;
        n = m = 0;
        return;
    } if (static_cast<long long>(n) * m > 1000000) {
        cerr << "Error! The size of the table should be less than or equal to 1000000." << endl;
        n = m = 0;
        return;
    }
    create_grid();
}

bool test_coordinates(int i, int j) { // test whether coordinates are in range
    if (i < 0 || i >= n || j < 0 || j >= m) {
        cerr << "Error! Coordinates (" << i << ", " << j << ") out of range!" << endl;
        return false;
    }
    return true;
}

void do_add() { // add
    int i, j; string x;
    cin >> i >> j >> x;
    if (!test_coordinates(i, j)) return;
    for (int ii = grid[i][j].i1; ii <= grid[i][j].i2; ++ii) {
        for (int jj = grid[i][j].j1; jj <= grid[i][j].j2; ++jj) {
            grid[ii][jj].val = x;
        }
    }
}

void do_merge() { // merge
    int i1, j1, i2, j2;
    cin >> i1 >> j1 >> i2 >> j2;
    if (!test_coordinates(i1, j1)) return;
    if (!test_coordinates(i2, j2)) return;
    if (i1 > i2) {
        cerr << "Error! i1 > i2!" << endl; return;
    } if (j1 > j2) {
        cerr << "Error! j1 > j2!" << endl; return;
    }
    function<void()> fail = []() {
        cerr << "Error! After merge it will not be a rectangle!" << endl;
    };
    int ti1 = grid[i1][j1].i1, tj1 = grid[i1][j1].j1, ti2 = grid[i2][j2].i2, tj2 = grid[i2][j2].j2;
    for (int i = i1; i <= i2; ++i) {
        if (grid[i][j1].j1 != tj1 || grid[i][j2].j2 != tj2) return fail();
    }
    for (int j = j1; j <= j2; ++j) {
        if (grid[i1][j].i1 != ti1 || grid[i2][j].i2 != ti2) return fail();
    }
    i1 = ti1; j1 = tj1; i2 = ti2; j2 = tj2;
    string new_val = grid[i1][j1].val;
    for (int i = i1; i <= i2; ++i) {
        for (int j = j1; j <= j2; ++j) {
            grid[i][j].val = new_val;
            grid[i][j].i1 = i1; grid[i][j].j1 = j1;
            grid[i][j].i2 = i2; grid[i][j].j2 = j2;
        }
    }
}

void do_split() { // split
    int i, j;
    cin >> i >> j;
    if (!test_coordinates(i, j)) return;
    int i1 = grid[i][j].i1, j1 = grid[i][j].j1;
    int i2 = grid[i][j].i2, j2 = grid[i][j].j2;
    for (int ii = i1; ii <= i2; ++ii) {
        for (int jj = j1; jj <= j2; ++jj) {
            grid[ii][jj].set(ii, jj);
        }
    }
}

void do_remove() { // remove
    int i, j;
    cin >> i >> j;
    if (!test_coordinates(i, j)) return;
    int i1 = grid[i][j].i1, j1 = grid[i][j].j1;
    int i2 = grid[i][j].i2, j2 = grid[i][j].j2;
    for (int ii = i1; ii <= i2; ++ii) {
        for (int jj = j1; jj <= j2; ++jj) {
            grid[ii][jj].val = "";
        }
    }
}

void do_clear() { // clear
    create_grid();
}

struct DiffSystem { // 差分约束系统，求每一列的宽度
    int n;
    vector<vector<pair<int, int>>> graph;
    void init(int _n) {
        graph.resize((n = _n) + 1);
        for (int i = 0; i < n; ++i) {
            add_edge(i + 1, i, -4);
        }
    }
    void add_cons(int l, int r, int w) {
        add_edge(r + 1, l, -w - 3);
    }
    vector<int> get_width() { // SPFA
        vector<int> dis(n + 1, numeric_limits<int>::max());
        dis[n] = 0;
        vector<bool> inque(dis.size() + 1);
        queue<int> que;
        que.emplace(n); inque[n] = true;
        while (!que.empty()) {
            int u = que.front(); que.pop();
            inque[u] = false;
            for (auto [v, w] : graph[u]) {
                if (dis[v] > dis[u] + w) {
                    dis[v] = dis[u] + w;
                    if (!inque[v]) {
                        que.emplace(v); inque[v] = true;
                    }
                }
            }
        }
        vector<int> ret(n + 1);
        for (int i = 0; i <= n; ++i) {
            ret[i] = dis[i] - dis[0];
        }
        return ret;
    }
  private:
    void add_edge(int u, int v, int w) {
        graph[u].emplace_back(v, w);
    }
};

void print() { // print table
    if (!n || !m) return;
    DiffSystem diff;
    diff.init(m);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (grid[i][j].i1 == i && grid[i][j].j1 == j) {
                diff.add_cons(grid[i][j].j1, grid[i][j].j2, grid[i][j].val.size());
            }
        }
    }
    vector<int> end = diff.get_width();
    function<bool(int, int)> rht_con = [&](int i, int j) { // whether (i, j) is connected with (i, j + 1)
        return i >= 0 && j >= 0 && grid[i][j].j2 > j;
    };
    function<bool(int, int)> dwn_con = [&](int i, int j) { // whether (i, j) is connected with (i + 1, j)
        return i >= 0 && j >= 0 && grid[i][j].i2 > i;
    };
    vector<string> tab(2 * n + 1);
    // 分界线
    for (int i = 0; i < n; ++i) {
        {
            string &line = tab[i << 1] = "+";
            int cur = 0;
            for (int j = 0; j < m; ++j) {
                while (++cur < end[j + 1]) line += dwn_con(i - 1, j) ? ' ' : '-';
                line += (i > 0 && !rht_con(i - 1, j)) || !rht_con(i, j) ? '+' : dwn_con(i - 1, j) ? ' ' : '-';
            }
        } {
            string &line = tab[i << 1 | 1] = "|";
            int cur = 0;
            for (int j = 0; j < m; ++j) {
                while (++cur < end[j + 1]) line += ' ';
                line += rht_con(i, j) ? ' ' : '|';
            }
        }
    }
    {
        string &line = tab.back() = "+";
        int cur = 0;
        for (int j = 0; j < m; ++j) {
            while (++cur < end[j + 1]) line += '-';
            line += rht_con(n - 1, j) ? '-' : '+';
        }
    }
    // 居中
    function<void(string&, int, int, const string&)> do_mid = [&](string &line, int l, int r, const string &str) {
        int i = (r + l - (int) str.size() + 1) >> 1;
        for (char c : str) line[i++] = c;
    };
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (grid[i][j].i1 == i && grid[i][j].j1 == j) {
                do_mid(tab[grid[i][j].i1 + grid[i][j].i2 + 1], end[j], end[grid[i][j].j2 + 1], grid[i][j].val);
            }
        }
    }
    for (auto &str : tab) cout << str << '\n';
}

int main() {
    while (true) {
        enum OperatorType typ = read_type();
        bool flag = false;
        switch (typ) {
            case Create: do_create(); break;
            case Add: do_add(); break;
            case Merge: do_merge(); break;
            case Split: do_split(); break;
            case Remove: do_remove(); break;
            case Clear: do_clear(); break;
            case Quit: flag = true; break;
            default: break;
        }
        if (flag) break;
        print();
    }
    return 0;
}
