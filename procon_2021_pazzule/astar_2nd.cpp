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

//A*による探索(メモリはそんなに食べられないと予測)
void astar2_solve(Status &status) {
	int d[2][4] = {
		{ 0, 1, 0, -1},
		{ 1, 0, -1, 0}
	};

	//priority_queueの設定
	//書き換えた
	auto compare = [](Status a, Status b) {
		if (a.eval_cost * swap_rate + a.total_cost == b.eval_cost * swap_rate + b.total_cost)
			return a.sel_cnt > b.sel_cnt;
		return a.eval_cost * swap_rate + a.total_cost > b.eval_cost * swap_rate + b.total_cost;
	};
	//元
	/*auto compare = [](Status a, Status b) {
		if (a.eval_cost == b.eval_cost)
			return a.sel_cnt > b.sel_cnt;
		return a.eval_cost > b.eval_cost;
	};*/

	priority_queue <
		Status,
		vector<Status>,
		decltype(compare)
	> que{ compare };
	que.push(status);

	//探索
	bool clear = false;
	while (!que.empty()) {
		Status sta = que.top(); que.pop();
		Status backup = sta;

		//持っているやつを動かす
		if (sta.sel_cnt != 0) {
			for (int k = 0; k < ((search_dir) ? 2 : 4); k++) {

				status_move(sta, d[0][k], d[1][k]);
				if (sta.place == complete) {
					answer = sta;
					clear = true;
					break;
				}
				sta.sellected = 0;
				que.push(sta);
				sta = backup;
			}
		}
		if (clear) break;

		//持ち替える
		if (!sta.sellected) {	//連続で持ち替えるのを防ぐ
			for (int y = 0; y < h; y++) {
				for (int x = 0; x < w; x++) {

					if (sta.x != x || sta.y != y) {
						if (sta.sel_cnt >= sel_lim) continue;
						status_sellect(sta, x, y);
						sta.sellected = 1;
					}
					else
						continue;	//変化しないので

					que.push(sta);
					sta = backup;
				}
			}
		}
	}

	status_show(answer);
	status_show_cost(answer);
}
