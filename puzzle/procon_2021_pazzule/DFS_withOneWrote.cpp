#include <iostream>
#include <vector>
#include <queue>
#include "status.h"
#include "lib.h"
#include "one_wrote_eval.h"
using namespace std;
extern vector<vector<unsigned char> > complete;
extern vector<int> goal_place;
extern Status answer;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir;


namespace DFS {
	const int d[2][4] = {
			{ 0, 1, 0, -1},
			{ 1, 0, -1, 0}
	};
	int node_cnt = 0;
	int min_cost = INT_MAX;
}
using namespace DFS;
bool dfs(int depth, Status& sta) {
	node_cnt++;
	if (sta.place == complete) {
		answer = sta;
		return true;
	}
	if (depth == 0) {
		if (chmin(min_cost, ow_solve_eval(sta, false))) {
			answer = sta;
		}
		return false;
	}

	bool clear = false;

	//‚Á‚Ä‚¢‚é‚â‚Â‚ğ“®‚©‚·
	if (sta.sel_cnt != 0) {	//‰½‚©‚ğ‚Á‚Ä‚¢‚éó‘Ô‚Å‚ ‚é
		for (int k = 0; k < ((search_dir) ? 2 : 4); k++) {	//“®‚©‚·•ûŒü

			//ˆê‰ñ‘O‚É–ß‚·“®‚«‚ğ‚µ‚È‚¢
			if (!sta.sellected) {
				if ((sta.last_swap_dir ^ k) == 2) {
					continue;
				}
			}

			int prev_last_swap_dir = sta.last_swap_dir;
			sta.last_swap_dir = k;

			status_move(sta, d[0][k], d[1][k]);
			bool prev_sellected = sta.sellected;
			sta.sellected = 0;

			clear = dfs(depth - 1, sta);
			if (clear) return true;

			status_unmove(sta);
			sta.last_swap_dir = prev_last_swap_dir;
			sta.sellected = prev_sellected;
		}
	}

	//‚¿‘Ö‚¦‚é
	if (sta.sel_cnt < sel_lim - 2 && !sta.sellected) {	//‰ñ”§ŒÀ‚Æ˜A‘±”ğ‚¯
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int pre_x = sta.x, pre_y = sta.y;
				if (sta.x != x || sta.y != y) {
					status_sellect(sta, x, y);
					sta.sellected = 1;
				}
				else
					continue;

				clear = dfs(depth, sta);
				if (clear) return true;

				status_unsellect(sta, pre_x, pre_y);
				sta.sellected = 0;
			}
		}
	}

	return clear;
}

//ˆ—‚Ì—¬‚ê(‚±‚±‚Å‚Í”½•œ[‰»‚Ì‚İ)
void dfs_solve(Status& sta) {
	bool clear = false;

	if (sel_lim == 2) {
		status_sellect(sta, complete[h - 1][w - 1] % w, complete[h - 1][w - 1] / w);
	}

	min_cost = ow_solve_eval(sta, false);

	answer = sta;
	clock_t start = clock();
	while (!clear && (double)(clock() - start) / CLOCKS_PER_SEC < 5) {
		sta = answer;
		clear = dfs(5, sta);
		printf("[end]nodes: %d\n", node_cnt);
		node_cnt = 0;
	}
	if (!clear) {
		ow_solve_eval(answer, true);
	}
	status_show(answer);
	status_show_cost(answer);
}