#include <iostream>
#include <vector>
#include <queue>
#include "status.h"
#include "lib.h"
using namespace std;
extern vector<vector<unsigned _int8> > complete;
extern vector<int> goal_place;
extern Status answer;
extern int h, w, sel_rate, swap_rate;

//IDA*による画像復元探索
bool idastar(int depth, int count, Status status) {
	if (status.place == complete) {
		answer = status;
		return true;
	}
	if (depth == 0) return 0;

	int d[2][4] = {
		{ 0, 1, 0, -1},
		{ 1, 0, -1, 0}
	};

	//手の生成
	auto compare = [](Status a, Status b) {
		if (a.eval_cost == b.eval_cost)
			return a.sel_cnt > b.sel_cnt;
		return a.eval_cost > b.eval_cost;
	};
	priority_queue <
		Status,
		vector<Status>,
		decltype(compare)
	> que {compare};
	Status backup_status = status;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			for (int k = 0; k < 4; k++) {
				int next_x = (x + d[0][k] + w) % w, next_y = (y + d[1][k] + h) % h;
				
				if (status.x == next_x  && status.y == next_y)
					continue;
				if (status.x != x || status.y != y) status.sellect(x, y, sel_rate);
				status.move(d[0][k], d[1][k], h, w, swap_rate, goal_place);
				que.push(status);
				status = backup_status;
			}
		}
	}

	// 実際に読む

	bool clear = false;
	while (!que.empty() && !clear) {
		Status te = que.top(); que.pop();
		if (te.eval_cost >= depth) break;

		clear = idastar(depth -1, count + 1, te);
	}

	return clear;
}

//処理の流れ(ここでは反復深化のみ)
void idastar_solve(Status *status){
	bool clear = false;
	for (int depth = 0; !clear; depth++) {
		clear = idastar(depth, 0, *status);
		printf("depth: %d\n", depth);
	}
	answer.show();
	answer.show_cost();
}