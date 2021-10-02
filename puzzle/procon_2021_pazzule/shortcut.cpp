#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include "status.h"
#include "lib.h"
#include "one_wrote.h"
#include "output_csv.h"
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

	unsigned char mid_sel;
	Status mid_answer;
	int mid_cost = 0;
	clock_t start;
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
	if ((clock() - start) / CLOCKS_PER_SEC >= 30) {
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
	if (sta.sel_cnt < sel_lim - 2 && !sta.sellected) {	//‰ñ”§ŒÀ‚Æ˜A‘±”ğ‚¯
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int pre_x = sta.x, pre_y = sta.y;
				if (sta.x != x || sta.y != y) {
					status_sellect(sta, x, y);
					if (sta.place[y][x] != mid_sel) {
						sta.compare_cost += sel_rate;
					}
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

				if (sta.place[y][x] != mid_sel) {
					sta.compare_cost -= sel_rate;
				}
				status_unsellect(sta, pre_x, pre_y);
				sta.sellected = 0;
			}
		}
	}

	return clear;
}

void shortcut_solve(Status& sta) {
	Status ow_answer = sta;
	mid_answer = sta;
	//ˆê•M‘‚«‚ÌŒ‹‰Ê‚ğ“¾‚é
	ow_solve(ow_answer);
	answer = sta;
	int sta_array_size = 1 + ow_answer.swap_cnt[0] + 1;
	if (ow_answer.sel_cnt == 2) {
		sta_array_size += ow_answer.swap_cnt[1] + 1;
	}
	vector<Status> sta_array(sta_array_size);
	Status state = sta;
	int swap_op = 0;
	for (int i = 0; i < sta_array_size; i++) {
		sta_array[i] = state;
		if (i == sta_array_size - 1) break;
		
		if (i == 0) {
			status_sellect(state, ow_answer.sel_place[0] % w, ow_answer.sel_place[0] / w);
			mid_sel = sta.place[ow_answer.sel_place[0] / w][ow_answer.sel_place[0] % w];
		}
		if (i <= ow_answer.swap_cnt[0]) {
			if (ow_answer.swap_operator[0][swap_op] == 'U')
				status_move(state, 0, -1);
			if (ow_answer.swap_operator[0][swap_op] == 'D')
				status_move(state, 0, 1);
			if (ow_answer.swap_operator[0][swap_op] == 'R')
				status_move(state, 1, 0);
			if (ow_answer.swap_operator[0][swap_op] == 'L')
				status_move(state, -1, 0);
			swap_op++;
		}
		if (i == ow_answer.swap_cnt[0] + 1) {
			status_sellect(state, ow_answer.sel_place[1] % w, ow_answer.sel_place[1] / w);
			swap_op = 0;
		}
		if (i > ow_answer.swap_cnt[0] + 1) {
			if (ow_answer.swap_operator[1][swap_op] == 'U')
				status_move(state, 0, -1);
			if (ow_answer.swap_operator[1][swap_op] == 'D')
				status_move(state, 0, 1);
			if (ow_answer.swap_operator[1][swap_op] == 'R')
				status_move(state, 1, 0);
			if (ow_answer.swap_operator[1][swap_op] == 'L')
				status_move(state, -1, 0);
			swap_op++;
		}
	}
	output_csv(sta, ow_answer, "OneWrote.csv");


	//‰ğ‚ğ’Zk‚µ‚Ä‚¢‚­
	start = clock();
	//ˆê•M‘‚«“r’†‚Ìó‘Ô‚ğ‚Æ‚é
	int length = 28;
	int end_num = length;
	Status end_sta = sta_array[end_num];
	auto pre_comp = complete;
	while ((clock() - start) / CLOCKS_PER_SEC < 30)
	{
		bool clear = false;
		
		complete = end_sta.place;
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++)
				goal_place[complete[y][x]] = x + y * w;
		status_calc_val(mid_answer);

		for (int depth = mid_answer.compare_cost; !clear && (clock() - start) / CLOCKS_PER_SEC < 30 && depth < end_sta.total_cost; depth = next_eval_cost) {
			next_eval_cost = INT_MAX;
			printf("[search]depth: %d\n", depth);
			clear = shortcut(depth, mid_answer);
			printf("[end]nodes: %d\n", node_cnt);
			node_cnt = 0;
		}
		if (clear) {
			//cout << "OK!" << endl;
		}
		else {
			if (mid_answer.sel_cnt == 0 || mid_answer.place[mid_answer.y][mid_answer.x] != mid_sel) {
				status_sellect(mid_answer, mid_answer.replace[mid_sel] % w, mid_answer.replace[mid_sel] / w);
			}
			for (int j = end_num - length; j < end_num; j++) {
				if (ow_answer.swap_operator[0][j] == 'U')
					status_move(mid_answer, 0, -1);
				if (ow_answer.swap_operator[0][j] == 'D')
					status_move(mid_answer, 0, 1);
				if (ow_answer.swap_operator[0][j] == 'R')
					status_move(mid_answer, 1, 0);
				if (ow_answer.swap_operator[0][j] == 'L')
					status_move(mid_answer, -1, 0);
			}
		}
		end_num += length;
		if (end_num > ow_answer.swap_cnt[0]) {
			break;
		}
		end_sta = sta_array[end_num];
	}
	if (mid_answer.sel_cnt == 0 || mid_answer.place[mid_answer.y][mid_answer.x] != mid_sel) {
		status_sellect(mid_answer, mid_answer.replace[mid_sel] % w, mid_answer.replace[mid_sel] / w);
	}
	for (int j = end_num - length; j < ow_answer.swap_cnt[0]; j++) {
		if (ow_answer.swap_operator[0][j] == 'U')
			status_move(mid_answer, 0, -1);
		if (ow_answer.swap_operator[0][j] == 'D')
			status_move(mid_answer, 0, 1);
		if (ow_answer.swap_operator[0][j] == 'R')
			status_move(mid_answer, 1, 0);
		if (ow_answer.swap_operator[0][j] == 'L')
			status_move(mid_answer, -1, 0);
	}
	if (ow_answer.sel_cnt == 2) {
		status_sellect(mid_answer, ow_answer.sel_place[1] % w, ow_answer.sel_place[1] / w);
		for (int j = 0; j < ow_answer.swap_cnt[1]; j++) {
			if (ow_answer.swap_operator[1][j] == 'U')
				status_move(mid_answer, 0, -1);
			if (ow_answer.swap_operator[1][j] == 'D')
				status_move(mid_answer, 0, 1);
			if (ow_answer.swap_operator[1][j] == 'R')
				status_move(mid_answer, 1, 0);
			if (ow_answer.swap_operator[1][j] == 'L')
				status_move(mid_answer, -1, 0);
		}
	}

	answer = mid_answer;
	complete = pre_comp;
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			goal_place[complete[y][x]] = x + y * w;

//	status_show(answer);
	status_show_cost(answer);

	output_csv(sta, answer, "ShortCut.csv");
}