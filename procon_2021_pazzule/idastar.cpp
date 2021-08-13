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

//IDA*‚É‚æ‚é‰æ‘œ•œŒ³’Tõ
int max_depth = 0; //–³‘Ê‚ÈŒvZ‚ğÈ‚­‚½‚ß‚É+1‚Å‚Í‚È‚­‘O‰ñ‚ÌŒvZ‚Å“¾‚½Å‘P‚Ì•]‰¿’l‚É‚·‚é
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

	bool clear = false;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			for (int k = 0; k < ((search_dir)? 2 : 4); k++) {
				//“ü‚ê‘Ö‚¦•û
				int next_x = (x + d[0][k] + w) % w, next_y = (y + d[1][k] + h) % h;
			    int pre_x = status.x, pre_y = status.y;
				if (status.x == next_x  && status.y == next_y)
					continue;
				if (status.x != x || status.y != y) {
					if (status.sel_cnt >= sel_lim) continue;
					status.sellect(x, y, sel_rate);
				}

				//ÀÛ‚É“Ç‚Ş
				status.move(d[0][k], d[1][k], h, w, swap_rate, goal_place);
				if (depth >= status.eval_cost / 2 + ((status.eval_cost % 2 == 0) ? 0 : 1))
					clear = idastar(depth - 1, count + 1, status);
				else
					max_depth = min(max_depth, count + status.eval_cost / 2 + ((status.eval_cost % 2 == 0) ? 0 : 1));
				if (clear) break;
				status.unmove(pre_x, pre_y, h, w, sel_rate, swap_rate, goal_place);
			}
			if (clear) break;
		}
		if (clear) break;
	}

	return clear;
}

//ˆ—‚Ì—¬‚ê(‚±‚±‚Å‚Í”½•œ[‰»‚Ì‚İ)
void idastar_solve(Status *status){
	bool clear = false;
	for (int depth = 1; !clear; depth = max_depth) {
		max_depth = 10000000;
		printf("[search]depth: %d\n", depth);
		clear = idastar(depth, 0, *status);
	}
	answer.show();
	answer.show_cost();
}