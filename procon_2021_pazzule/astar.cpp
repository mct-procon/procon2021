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

//A*Ç…ÇÊÇÈíTçı(ÉÅÉÇÉäÇÕÇªÇÒÇ»Ç…êHÇ◊ÇÁÇÍÇ»Ç¢Ç∆ó\ë™)
void astar_solve(Status &status) {
	int d[2][4] = {
		{ 0, 1, 0, -1},
		{ 1, 0, -1, 0}
	};

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
	que.push(status);

	//íTçı
	bool clear = false;
	while (!que.empty()) {
		Status sta = que.top(); que.pop();
		Status backup = sta;
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				for (int k = 0; k < ((search_dir) ? 2 : 4); k++) {
					int next_x = (x + d[0][k] + w) % w, next_y = (y + d[1][k] + h) % h;

					if (sta.x == next_x && sta.y == next_y)
						continue;
					if (sta.x != x || sta.y != y) {
						if (sta.sel_cnt >= sel_lim) continue;
						status_sellect(sta, x, y);
					}
					status_move(sta, d[0][k], d[1][k]);
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

	status_show(answer);
	status_show_cost(answer);
}
