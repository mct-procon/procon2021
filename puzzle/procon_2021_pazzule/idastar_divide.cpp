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
namespace IDA_D {
	int next_eval_cost = 0; //無駄な計算を省くために+1ではなく前回の計算で得た最善の評価値にする
	const int d[2][4] = {
			{ 0, 1, 0, -1},
			{ 1, 0, -1, 0}
	};
	int node_cnt = 0;
	//正解を部分的に示す
//最終的に正解と一致するように変化
	int divhw;
	int fixhw;

	bool match_div(vector<vector<unsigned char>> sta_place) {
		for (int i = 0; i <= divhw; i++) {
			for (int j = 0; j+i*w <= divhw; j++) {
				if (sta_place[i][j] == complete[i][j])
					continue;
				return 0;
			}
		}
		return 1;
	}

	void status_update_val_D(Status& sta, int dx, int dy, int divhw, int fixhw) {
		int prev_x = (sta.x - dx + w) % w, prev_y = (sta.y - dy + h) % h;	//持ってるピースの入れ替える前の場所

		//持ってるピースの
		int goal_x = goal_place[sta.place[sta.y][sta.x]] % w, goal_y = goal_place[sta.place[sta.y][sta.x]] / w;	//目的地
		//評価値差分
		int pas_x = min(abs(goal_x - prev_x), w - abs(goal_x - prev_x));
		int pas_y = min(abs(goal_y - prev_y), w - abs(goal_y - prev_y));
		int pas_dis = pas_x + pas_y;
		int new_x = min(abs(goal_x - sta.x), w - abs(goal_x - sta.x));
		int new_y = min(abs(goal_y - sta.y), w - abs(goal_y - sta.y));
		int new_dis = new_x + new_y;

		if (sta.x + sta.y * w <= fixhw) {
			sta.compare_cost = INT_MAX / 2;
			return;
		}
		if (prev_x + prev_y * w >= divhw) {
			pas_dis = 0;
		}
		if (sta.x + sta.y * w >= divhw) {
			new_dis = 0;
		}
		if (pas_dis == 0 && new_dis != 0)
			sta.dis_not_zero++;
		if (pas_dis != 0 && new_dis == 0)
			sta.dis_not_zero--;
		sta.dis_array[sta.y * w + sta.x] = new_dis;
		sta.total_dis += (new_dis - pas_dis);

		//持ってないピースの
		goal_x = goal_place[sta.place[prev_y][prev_x]] % w; goal_y = goal_place[sta.place[prev_y][prev_x]] / w;	//目的地
		//評価値差分
		pas_x = min(abs(goal_x - sta.x), w - abs(goal_x - sta.x));
		pas_y = min(abs(goal_y - sta.y), w - abs(goal_y - sta.y));
		pas_dis = pas_x + pas_y;
		new_x = min(abs(goal_x - prev_x), w - abs(goal_x - prev_x));
		new_y = min(abs(goal_y - prev_y), w - abs(goal_y - prev_y));
		new_dis = new_x + new_y;

		if (sta.x + sta.y * w >= divhw) {
			pas_dis = 0;
		}
		if (prev_x + prev_y * w >= divhw) {
			new_dis = 0;
		}

		if (pas_dis == 0 && new_dis != 0)
			sta.dis_not_zero++;
		if (pas_dis != 0 && new_dis == 0)
			sta.dis_not_zero--;
		sta.dis_array[prev_y * w + prev_x] = new_dis;
		sta.total_dis += (new_dis - pas_dis);

		sta.dis_top1 = 0;
		sta.dis_top2 = 0;

		for (int i = 0; i < sta.array_size; i++) {
			unsigned char num = sta.dis_array[i];
			//for(int num : sta.dis_array){
			if (chmax(sta.dis_top2, num)) {	//より大きいものに更新したら
				if (sta.dis_top1 < sta.dis_top2)
					swap(sta.dis_top1, sta.dis_top2);
			}
		}

		sta.eval_cost = max({ sta.total_dis / 2 + (sta.total_dis & 1), sta.dis_top1 + sta.dis_top2 - 1 });
		//sta.eval_cost = sta.total_dis / 2 + (sta.total_dis & 1);

		
		int heuristic = 0;
		int heuristic_all = 0;
		if (sta.dis_top2 > 1) {
			heuristic += swap_rate;
			heuristic_all += swap_rate;
		}

		sta.compare_cost = sta.total_cost + (sta.eval_cost * swap_rate + heuristic) + heuristic_all;
		if (sta.compare_cost > 500) {
			printf("?");
		}
	}

	void status_unmove_D(Status& sta, int divhw, int fixhw) {
		sta.total_cost -= swap_rate;	//status_update_valで使うので先に

		int swap_op_last = sta.sel_cnt - 1;

		if (sta.last_swap_dir & 1) {	//右か左かに動かした
			int dx = (1 - (sta.last_swap_dir & 2));
			int prev_x = (sta.x - dx + w) % w;
			swap(sta.place[sta.y][prev_x], sta.place[sta.y][sta.x]);
			sta.x = prev_x;
			status_update_val_D(sta, -dx, 0, divhw, fixhw);
		}
		else {	//上か下かに動かした
			int dy = (1 - (sta.last_swap_dir & 2));
			int prev_y = (sta.y - dy + h) % h;
			swap(sta.place[prev_y][sta.x], sta.place[sta.y][sta.x]);
			sta.y = prev_y;
			status_update_val_D(sta, 0, -dy, divhw, fixhw);
		}

		//sta.swap_operator[swap_op_last].erase(sta.swap_operator[swap_op_last].begin() + sta.swap_operator[swap_op_last].size() - 1);
		sta.swap_operator[swap_op_last].pop_back();
		sta.swap_cnt[sta.swap_cnt.size() - 1]--;
	}
	void status_move_D(Status& sta, int dx, int dy, int divhw, int fixhw) {
		sta.total_cost += swap_rate;
		sta.swap_cnt[sta.swap_cnt.size() - 1] += 1;
		if (dy == -1) sta.swap_operator[sta.swap_operator.size() - 1] += "U";
		if (dx == -1) sta.swap_operator[sta.swap_operator.size() - 1] += "L";
		if (dy == 1) sta.swap_operator[sta.swap_operator.size() - 1] += "D";
		if (dx == 1) sta.swap_operator[sta.swap_operator.size() - 1] += "R";
		swap(sta.place[(sta.y + dy + h) % h][(sta.x + dx + w) % w], sta.place[sta.y][sta.x]);
		sta.x = (sta.x + dx + w) % w; sta.y = (sta.y + dy + h) % h;
		status_update_val_D(sta, dx, dy, divhw, fixhw);

	}
}
using namespace IDA_D;
bool idastar_D(int goal_eval_cost, Status& sta) {
	node_cnt++;
	if (match_div(sta.place)) {
		answer = sta;
		return true;
	}
	if (goal_eval_cost < sta.compare_cost) {
		next_eval_cost = min(next_eval_cost, sta.compare_cost);
		return 0;
	}

	bool clear = false;

	//持っているやつを動かす
	if (sta.sel_cnt != 0) {	//何かを持っている状態である
		for (int k = 0; k < ((search_dir) ? 2 : 4); k++) {	//動かす方向

			//一回前に戻す動きをしない
			if (!sta.sellected) {
				if ((sta.last_swap_dir ^ k) == 2) {
					continue;
				}
			}

			int prev_last_swap_dir = sta.last_swap_dir;
			sta.last_swap_dir = k;

			status_move_D(sta, d[0][k], d[1][k], divhw, fixhw);
			bool prev_sellected = sta.sellected;
			sta.sellected = 0;

			if (goal_eval_cost >= sta.compare_cost) {
				clear = idastar_D(goal_eval_cost, sta);
				if (clear) return true;
			}
			else {
				next_eval_cost = min(next_eval_cost, sta.compare_cost);
			}

			status_unmove_D(sta, divhw, fixhw);
			sta.last_swap_dir = prev_last_swap_dir;
			sta.sellected = prev_sellected;
		}
	}

	//持ち替える
	//if (sta.sel_cnt < sel_lim && !sta.sellected) {	//回数制限と連続避け
	//	for (int y = 0; y < h; y++) {
	//		for (int x = 0; x < w; x++) {
	//			int pre_x = sta.x, pre_y = sta.y;
	//			if (sta.x != x || sta.y != y) {
	//				status_sellect(sta, x, y);
	//				sta.sellected = 1;
	//			}
	//			else
	//				continue;

	//			if (goal_eval_cost >= sta.compare_cost) {
	//				clear = idastar_D(goal_eval_cost, sta);
	//				if (clear) return true;
	//			}
	//			else {
	//				next_eval_cost = min(next_eval_cost, sta.compare_cost);
	//			}

	//			status_unsellect(sta, pre_x, pre_y);
	//			sta.sellected = 0;
	//		}
	//	}
	//}

	return clear;
}

//処理の流れ(ここでは反復深化のみ)
void idastar_D_solve(Status& sta) {
	bool clear = false;
	divhw = -1;
	fixhw = -1;

	status_sellect(sta, complete[h - 1][w - 1] % w, complete[h - 1][w - 1] / w);


	// 上手く動いていないので修正頑張って()


	while (divhw < h*w) {
		divhw++;
		if (h - (divhw / w) == 1) {
			divhw = h*w;
		}
		printf("divhw: %d\n", divhw);
		clear = false;
		for (int depth = sta.compare_cost + swap_rate; !clear; depth = next_eval_cost) {
			next_eval_cost = INT_MAX;
			printf("[search]depth: %d\n", depth);
			clear = idastar_D(depth, sta);
			printf("[end]nodes: %d\n", node_cnt);
			node_cnt = 0;
		}
		fixhw = divhw;
	}
	status_show(answer);
	status_show_cost(answer);
}