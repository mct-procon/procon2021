#include <iostream>
#include <vector>
#include <queue>
#include "status.h"
#include "lib.h"
using namespace std;
extern vector<vector<int> > complete;
extern vector<int> goal_place;
extern Status answer;
extern int h, w, sel_rate, swap_rate;

//IDA*‚É‚æ‚é‰æ‘œ•œŒ³’Tõ
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

	//è‚Ì¶¬
	auto compare = [](Status a, Status b) {
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
				if (x + d[0][k] < 0 || x + d[0][k] >= w || y + d[1][k] < 0 || y + d[1][k] >= h) continue;
				
				if (status.x == x + d[0][k]  && status.y == y + d[1][k])
					continue;
				if (status.x != x || status.y != y) status.sellect(x, y, sel_rate);
				status.move(d[0][k], d[1][k], w, swap_rate, goal_place);
				que.push(status);
				status = backup_status;
			}
		}
	}

	// ÀÛ‚É“Ç‚Ş

	bool clear = false;
	while (!que.empty() && !clear) {
		Status te = que.top(); que.pop();
		if (te.eval_cost >= depth) break;

		clear = idastar(depth -1, count + 1, te);
	}

	return clear;
}

//ˆ—‚Ì—¬‚ê(‚±‚±‚Å‚Í”½•œ[‰»‚Ì‚İ)
void idastar_solve(Status *status){
	bool clear = false;
	for (int depth = 0; !clear; depth++) {
		clear = idastar(depth, 0, *status);
		printf("depth: %d\n", depth);
	}
	answer.show();
	answer.show_cost();
}