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

//A*�ɂ��T��(�������͂���ȂɐH�ׂ��Ȃ��Ɨ\��)
void astar_solve(Status *status) {
	int d[2][4] = {
		{ 0, 1, 0, -1},
		{ 1, 0, -1, 0}
	};

	//priority_queue�̐ݒ�
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
	que.push(*status);

	//�T��
	bool clear = false;
	while (!que.empty()) {
		Status sta = que.top(); que.pop();
		Status backup = sta;
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				for (int k = 0; k < 4; k++) {
					int next_x = (x + d[0][k] + w) % w, next_y = (y + d[1][k] + h) % h;

					if (sta.x == next_x && sta.y == next_y)
						continue;
					if (sta.x != x || sta.y != y) sta.sellect(x, y, sel_rate);
					sta.move(d[0][k], d[1][k], h, w, swap_rate, goal_place);
					if (sta.place == complete) {
						answer = sta;
						clear = true;
						break;
					}

					que.push(sta);
					sta = backup;
				}
				if (clear) break;
			}
			if (clear) break;
		}
		if (clear) break;
	}
}