#include <iostream>
#include <vector>
#include <queue>
#include "status.h"
#include "lib.h"
#include "one_wrote.h"
using namespace std;
extern vector<vector<unsigned char> > complete;
extern vector<int> goal_place;
extern Status answer;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir;

namespace SCUT {
	int next_eval_cost = 0;
	const int d[2][4] = {
			{ 0, 1, 0, -1},
			{ 1, 0, -1, 0}
	};
	int node_cnt = 0;

	vector<vector<unsigned char>> mid_comp;
	Status mid_answer;
	int mid_cost = 0;
}
using namespace SCUT;
bool shortcut(int goal_eval_cost, Status& sta) {
	node_cnt++;
	if (sta.place == complete) {
		mid_answer = sta;
		return true;
	}
	if (goal_eval_cost < sta.compare_cost) {
		next_eval_cost = min(next_eval_cost, sta.compare_cost);
		return 0;
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

			if (goal_eval_cost >= sta.compare_cost) {
				clear = shortcut(goal_eval_cost, sta);
				if (clear) return true;
			}
			else {
				next_eval_cost = min(next_eval_cost, sta.compare_cost);
			}

			status_unmove(sta);
			sta.last_swap_dir = prev_last_swap_dir;
			sta.sellected = prev_sellected;
		}
	}

	//‚¿‘Ö‚¦‚é
	if (sta.sel_cnt < sel_lim && !sta.sellected) {	//‰ñ”§ŒÀ‚Æ˜A‘±”ğ‚¯
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int pre_x = sta.x, pre_y = sta.y;
				if (sta.x != x || sta.y != y) {
					status_sellect(sta, x, y);
					sta.sellected = 1;
				}
				else
					continue;

				if (goal_eval_cost >= sta.compare_cost) {
					clear = shortcut(goal_eval_cost, sta);
					if (clear) return true;
				}
				else {
					next_eval_cost = min(next_eval_cost, sta.compare_cost);
				}

				status_unsellect(sta, pre_x, pre_y);
				sta.sellected = 0;
			}
		}
	}

	return clear;
}

//ˆ—‚Ì—¬‚ê(‚±‚±‚Å‚Í”½•œ[‰»‚Ì‚İ)
void shortcut_solve(Status& sta) {
	mid_answer = sta;
	//ˆê•M‘‚«‚ÌŒ‹‰Ê‚ğ“¾‚é
	ow_solve(mid_answer);
	int sta_array_size = 1 + mid_answer.swap_cnt[0] + 1;
	if (mid_answer.sel_cnt == 2) {
		sta_array_size += mid_answer.swap_cnt[1] + 1;
	}
	vector<Status> sta_array(sta_array_size);
	Status state = sta;
	int swap_op = 0;
	FILE* fp;
	//fopen_s(&fp, "distance.csv", "w");
	for (int i = 0; i < sta_array_size; i++) {
		sta_array[i] = state;
		//cout << state.total_dis << endl;
		//fprintf_s(fp, "%d,\n", state.total_dis);
		if (i == sta_array_size - 1) break;
		
		if (i == 0) {
			status_sellect(state, mid_answer.sel_place[0] % w, mid_answer.sel_place[0] / w);
		}
		if (i <= mid_answer.swap_cnt[0]) {
			if (mid_answer.swap_operator[0][swap_op] == 'U')
				status_move(state, 0, -1);
			if (mid_answer.swap_operator[0][swap_op] == 'D')
				status_move(state, 0, 1);
			if (mid_answer.swap_operator[0][swap_op] == 'R')
				status_move(state, 1, 0);
			if (mid_answer.swap_operator[0][swap_op] == 'L')
				status_move(state, -1, 0);
			swap_op++;
		}
		if (i == mid_answer.swap_cnt[0] + 1) {
			status_sellect(state, mid_answer.sel_place[1] % w, mid_answer.sel_place[1] / w);
			swap_op = 0;
		}
		if (i > mid_answer.swap_cnt[0] + 1) {
			if (mid_answer.swap_operator[1][swap_op] == 'U')
				status_move(state, 0, -1);
			if (mid_answer.swap_operator[1][swap_op] == 'D')
				status_move(state, 0, 1);
			if (mid_answer.swap_operator[1][swap_op] == 'R')
				status_move(state, 1, 0);
			if (mid_answer.swap_operator[1][swap_op] == 'L')
				status_move(state, -1, 0);
			swap_op++;
		}
	}
	//fclose(fp);
	//

	//‰ğ‚ğ’Zk‚µ‚Ä‚¢‚­
	clock_t start = clock();
	//while ((clock() - start) / CLOCKS_PER_SEC < 5) 
	{
		bool clear = false;
		//ˆê•M‘‚«“r’†‚Ìó‘Ô‚ğ‚Æ‚é
		

		//for (int depth = sta.compare_cost + sel_rate; !clear; depth = next_eval_cost) {
		//	next_eval_cost = INT_MAX;
		//	printf("[search]depth: %d\n", depth);
		//	clear = shortcut(depth, sta);
		//	printf("[end]nodes: %d\n", node_cnt);
		//	node_cnt = 0;
		//}
	}
	status_show(answer);
	status_show_cost(answer);
}