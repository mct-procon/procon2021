#include <iostream>
#include <vector>
#include <queue>
#include "status.h"
#include "lib.h"
using namespace std;
extern vector<vector<unsigned char> > complete;
extern vector<int> goal_place;
extern Status answer;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir;

Status best_ans;

//ê[Ç≥10ÇÃíTçı => ç≈Ç‡ÇÊÇ¢ï]âøÇëIë => çƒÇ—ê[Ç≥10ÇÃíTçı
bool db_dfs(int depth, Status sta) {
	if (complete == sta.place) {
		answer = sta;
		return true;
	}

	if (best_ans.eval_cost >= sta.eval_cost) best_ans = sta;
	if (depth == 0) return false;

	//priority_queueÇÃê›íË
	auto compare = [](Status a, Status b) {
		if (a.eval_cost == b.eval_cost)
			return a.sel_cnt > b.sel_cnt;
		return a.eval_cost > b.eval_cost;
	};
	priority_queue <
		Status,
		vector<Status>,
		decltype(compare)
	> que{ compare };
	int d[2][4] = {
		{ 0, 1, 0, -1},
		{ 1, 0, -1, 0}
	};
	Status backup;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			for (int k = 0; k < ((search_dir) ? 2 : 4); k++) {
				int next_x = (x + d[0][k] + w) % w, next_y = (y + d[1][k] + h) % h;
				backup = sta;

				if (sta.x == next_x && sta.y == next_y)
					continue;
				if (sta.x != x || sta.y != y) {
					if (sta.sel_cnt >= sel_lim) continue;
					status_sellect(sta, x, y);
				}
				status_move(sta, d[0][k], d[1][k]);
				que.push(sta);
				sta = backup;
			}
		}
	}

	// é¿ç€Ç…ì«Çﬁ
	bool clear = false;
	while (!que.empty() && !clear) {
		Status te = que.top(); que.pop();

		clear = db_dfs(depth - 1, te);
	}
	return clear;
}

void db_solve(Status &status) {
	bool clear = false;
	best_ans = status;
	while (!clear) {
		clear = db_dfs(10, best_ans);
	}
	status_show(answer);
	status_show_cost(answer);
}