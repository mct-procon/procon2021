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
namespace IDA {
	int next_eval_cost = 0; //–³‘Ê‚ÈŒvZ‚ğÈ‚­‚½‚ß‚É+1‚Å‚Í‚È‚­‘O‰ñ‚ÌŒvZ‚Å“¾‚½Å‘P‚Ì•]‰¿’l‚É‚·‚é
	const int d[2][4] = {
			{ 0, 1, 0, -1},
			{ 1, 0, -1, 0}
	};
	int node_cnt = 0;
}
using namespace IDA;
bool idastar(int goal_eval_cost, Status &sta) {
	node_cnt++;
	if (sta.place == complete) {
		answer = sta;
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
				clear = idastar(goal_eval_cost, sta);
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
					clear = idastar(goal_eval_cost, sta);
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
void idastar_solve(Status &sta){
	bool clear = false;
	for (int depth = sta.compare_cost + sel_rate; !clear; depth = next_eval_cost) {
		next_eval_cost = INT_MAX;
		printf("[search]depth: %d\n", depth);
		clear = idastar(depth, sta);
		printf("[end]nodes: %d\n", node_cnt);
		node_cnt = 0;
	}
	status_show(answer);
	status_show_cost(answer);
}