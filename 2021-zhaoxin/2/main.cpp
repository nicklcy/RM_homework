// This code is written by lcy.

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

const size_t N = 1E5 + 5;
const int INF = 1E9;

vector<int> g[N];

void dijkstra(int n, int dis[], int s) {
    fill(dis + 1, dis + n + 1, INF);
    priority_queue<pair<int, int>> pque;
    pque.emplace(-(dis[s] = 0), s);
    while (!pque.empty()) {
        int d = -pque.top().first, u = pque.top().second;
        pque.pop();
        if (dis[u] != d) continue;
        for (int v : g[u]) {
            if (d + 1 < dis[v]) {
                pque.emplace(-(dis[v] = d + 1), v);
            }
        }
    }
}

int dis1[N], dis2[N], disn[N];

using i64 = long long;

int main() {
    i64 B, E, P; int n, m;
    cin >> B >> E >> P >> n >> m;
    while (m--) {
        int u, v;
        cin >> u >> v;
        g[u].emplace_back(v);
        g[v].emplace_back(u);
    }
    dijkstra(n, dis1, 1);
    dijkstra(n, dis2, 2);
    dijkstra(n, disn, n);
    i64 ans = B * dis1[n] + E * dis2[n];
    for (int i = 1; i <= n; ++i) {
        ans = min(ans, B * dis1[i] + E * dis2[i] + P * disn[i]);
    }
    cout << ans << '\n';
    return 0;
}