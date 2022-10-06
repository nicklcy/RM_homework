#include <cstdio>
#include <cassert>
#include <set>
#include <map>
#include <algorithm>

using i64 = long long;
const int INF = 1E9 + 5;

int n, m, k;
std::map<int, std::set<int> > mp_x, mp_y;

int find_greater(const std::set<int> &s, int b) { // 找出集合中刚好比它大的元素
	auto it = s.upper_bound(b);
	return it == s.end() ? INF : *it;
}

int find_smaller(const std::set<int> &s, int b) { // 找出集合中刚好比他小的元素
	auto it = s.lower_bound(b);
	return it == s.begin() ? -1 : *--it;
}

i64 doit(int sx, int sy, int ex, int ey) { // (sx, sy) 表示起始点，(ex, ey) 表示最右下角的点
	if (sx > ex || sy > ey) return 0; // 都走完了
	if (mp_x[sx].count(sy)) return 0; // 当前位置不能走
	i64 res = 1; // 当前经过格子数量
	int cx = sx, cy = sy; // 当前坐标
	/* 往右走 */ {
		int ny = std::min(find_greater(mp_x[cx], cy) - 1, ey);
		res += ny - cy;
		cy = ny;
	} /* 往下走 */ {
		int nx = std::min(find_greater(mp_y[cy], cx) - 1, ex);
		res += nx - cx;
		cx = nx;
	}
	int corner_x = cx, corner_y = cy; // 右下角坐标
	if (corner_x > sx) { // 往左走
		int ny = std::max(find_smaller(mp_x[cx], cy) + 1, sy);
		res += cy - ny;
		cy = ny;
	}
	if (corner_y > sy && cx > sx) { // 往上走
		int nx = std::max(find_smaller(mp_y[cy], cx) + 1, sx + 1);
		res += cx - nx;
		cx = nx;
	}
	res += doit(cx, cy + 1, corner_x - 1, corner_y - 1); // 往右走，递归
	return res;
}

int main() {
	scanf("%d%d%d", &n, &m, &k);
	if (!k) { // 没有障碍必然 Yes
		puts("Yes");
		return 0;
	}
	int rnds = INF; // 不碰到障碍物，能绕几圈
	for (int i = 0; i < k; ++i) {
		int x, y;
		scanf("%d%d", &x, &y);
		mp_x[x].emplace(y);
		mp_y[y].emplace(x);
		rnds = std::min(rnds, std::min(std::min(x - 1, n - x), std::min(y - 1, m - y)));
	}
	// 外面绕完了，减小需要绕的范围
	int sx = 1 + rnds, sy = 1 + rnds;
	int ex = n - rnds, ey = m - rnds;
	if (k < ex - sx - 5 && k < ey - sy - 5) {
		// 如果障碍物都不能把一行或者一列填满，必然为 No
		puts("No");
		return 0;
	}
	i64 ans = doit(sx, sy, ex, ey);
	assert(ans <= (i64) (ex - sx + 1) * (ey - sy + 1));
	if (ans < (i64) (ex - sx + 1) * (ey - sy + 1) - k) {
		puts("No");
	} else {
		puts("Yes");
	}
	return 0;
}
