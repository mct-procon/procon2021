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

//IDA*による画像復元探索
int max_depth = 0; //無駄な計算を省くために+1ではなく前回の計算で得た最善の評価値にする
bool idastar(int depth, int count, Status *status) {
	if (status->place == complete) {
		answer = *status;
		return true;
	}
	if (depth == 0) {
		max_depth = min(max_depth, count + status->eval_cost / 2 + ((status->eval_cost % 2 == 0) ? 0 : 1));
		return 0;
	}

	int d[2][4] = {
		{ 0, 1, 0, -1},
		{ 1, 0, -1, 0}
	};

	bool clear = false;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			for (int k = 0; k < ((search_dir)? 2 : 4); k++) {
				//入れ替え方
				int next_x = (x + d[0][k] + w) % w, next_y = (y + d[1][k] + h) % h;
			    int pre_x = status->x, pre_y = status->y;
				if (status->x == next_x  && status->y == next_y)
					continue;
				if (status->x != x || status->y != y) {
					if (status->sel_cnt >= sel_lim) continue;
					status->sellect(x, y, sel_rate);
				}

				//実際に読む
				status->move(d[0][k], d[1][k], h, w, swap_rate, goal_place);
				if (depth * 2 >= status->eval_cost)
					clear = idastar(depth - 1, count + 1, status);
				else
					max_depth = min(max_depth, count + 1 + status->eval_cost / 2 + ((status->eval_cost % 2 == 0) ? 0 : 1));
				if (clear) return true;
				status->unmove(pre_x, pre_y, h, w, sel_rate, swap_rate, goal_place);
			}
		}
	}

	return clear;
}

//処理の流れ(ここでは反復深化のみ)
void idastar_solve(Status *status){
	bool clear = false;
	for (int depth = status->eval_cost / 2 + ((status->eval_cost % 2 == 0) ? 0 : 1); !clear; depth = max_depth) {
		max_depth = 10000000;
		printf("[search]depth: %d\n", depth);
		clear = idastar(depth, 0, status);
	}
	answer.show();
	answer.show_cost();
}