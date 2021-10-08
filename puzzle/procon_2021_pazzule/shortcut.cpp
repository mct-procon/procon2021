#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <cassert>
#include "status.h"
#include "lib.h"
#include "one_wrote.h"
#include "output_csv.h"
using namespace std;
extern vector<vector<unsigned char> > complete;
extern vector<int> goal_place;
extern Status answer;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir;

#define TIME_LIMIT 5

namespace SCUT {
	int next_eval_cost = 0;
	const int d[2][4] = {
			{ 0, 1, 0, -1},
			{ 1, 0, -1, 0}
	};
	int node_cnt = 0;

	unsigned char mid_sel, mid_begin_sel;
	Status mid_answer;
	clock_t start;
	int sel_resi;
  int maxlength = 5;
}
using namespace SCUT;
bool idastar_sc(int goal_eval_cost, Status& sta) {
	node_cnt++;
	if (sta.place == complete) {
		mid_answer = sta;
		return true;
	}
	if (goal_eval_cost < sta.compare_cost) {
		next_eval_cost = min(next_eval_cost, sta.compare_cost);
		return 0;
	}
	if ((clock() - start) / CLOCKS_PER_SEC >= TIME_LIMIT) {
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

			status_move(sta, d[0][k], d[1][k]);
			bool prev_sellected = sta.sellected;
			sta.sellected = 0;

			if (goal_eval_cost >= sta.compare_cost) {
				clear = idastar_sc(goal_eval_cost, sta);
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

	//持ち替える
	if (sta.sel_cnt < sel_lim - sel_resi && !sta.sellected) {	//回数制限と連続避け
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int pre_x = sta.x, pre_y = sta.y;
				if (sta.x != x || sta.y != y) {
					if (sta.place[y][x] != mid_sel) {
						if (sta.sel_cnt + 1 >= sel_lim - sel_resi) {
							continue;
						}
						sta.compare_cost += sel_rate;
					}
					status_sellect(sta, x, y);
					sta.sellected = 1;
				}
				else
					continue;

				if (goal_eval_cost >= sta.compare_cost) {
					clear = idastar_sc(goal_eval_cost, sta);
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

void shortcut(Status sta, Status pre_answer) {
	mid_answer = sta;
	//一筆書きの結果を得る
	answer = sta;
	mid_sel = sta.place[pre_answer.sel_place[0] / w][pre_answer.sel_place[0] % w];
	sel_resi = pre_answer.sel_cnt;
	int sta_array_size = 1;
	sta_array_size += pre_answer.sel_cnt;
	for (int i = 0; i < pre_answer.sel_cnt; i++) {
		sta_array_size += pre_answer.swap_cnt[i];
	}
	vector<int> operation(sta_array_size, -1);
	vector<Status> sta_array(sta_array_size);
	Status state = sta;
	int swap_op = 0, swap_num = 0;
	int sel_op = -1;
	for (int i = 0; i < sta_array_size; i++) {
		sta_array[i] = state;
		if (i == sta_array_size - 1) break;
		
		if (i == swap_num) {
			sel_op++;
			status_sellect(state, pre_answer.sel_place[sel_op] % w, pre_answer.sel_place[sel_op] / w);
			operation[i] = ((state.place[pre_answer.sel_place[sel_op] / w][pre_answer.sel_place[sel_op] % w]) << 4) + 5;
			swap_op = 0;
			swap_num += pre_answer.swap_cnt[sel_op] + 1;
		}
		else {
			if (pre_answer.swap_operator[sel_op][swap_op] == 'D') {
				status_move(state, 0, 1);
				operation[i] = 0;
			}
			else if (pre_answer.swap_operator[sel_op][swap_op] == 'R') {
				status_move(state, 1, 0);
				operation[i] = 1;
			}
			else if (pre_answer.swap_operator[sel_op][swap_op] == 'U') {
				status_move(state, 0, -1);
				operation[i] = 2;
			}
			else if (pre_answer.swap_operator[sel_op][swap_op] == 'L') {
				status_move(state, -1, 0);
				operation[i] = 3;
			}
			else {
				assert(false);
			}
			swap_op++;
		}
	}
	//output_csv(sta, ow_answer, "OneWrote.csv");


	//解を短縮していく
	start = clock();
	int length = maxlength;  //この長さ間隔で圧縮
	int end_num = 0;  //lengthの間隔で抜き取った終わりの番号
	int begin_num = 0;//lengthの間隔で抜き取ったはじめの番号
	Status end_sta = sta_array[end_num];  //抜き取った終わりの盤面
	auto pre_comp = complete; //completeを変更するのでバックアップをとる
	while ((clock() - start) / CLOCKS_PER_SEC < TIME_LIMIT)
	{
		bool clear = false;
		
    // 探索に関する値の更新
    //
		begin_num = end_num;
		mid_begin_sel = mid_sel;
		//length = rand() % 10 + maxlength;	//すぐ収束しないようにランダムな長さ
		end_num += length;
		if (end_num >= sta_array_size - 1) {
			end_num = sta_array_size - 1;
		}
		if (operation[end_num - 1] >= 5) {
			end_num++;
		}
		end_sta = sta_array[end_num];

    // 完成形を抜き取った終わりの盤面にする
		complete = end_sta.place;
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++)
				goal_place[complete[y][x]] = x + y * w;
		status_calc_val(mid_answer);
    //

    // 抜き取った範囲の選択操作を数える
    // 最後の選択操作を見る
		for (int i = begin_num; i < end_num; i++) {
			if (operation[i] >= 5) {
				sel_resi--;
				mid_sel = (operation[i] >> 4);
			}
		}

		//cout << "total_dis:" << mid_answer.total_dis << endl;
		//cout << "starttotal" << mid_answer.total_cost << " startcost:" << mid_answer.compare_cost << " endcost:" << end_sta.total_cost << endl;
		if (!(mid_answer.compare_cost <= end_sta.total_cost)) {
			assert(false);
		}
		for (int depth = mid_answer.compare_cost; !clear && (clock() - start) / CLOCKS_PER_SEC < TIME_LIMIT && depth < end_sta.total_cost; depth = next_eval_cost) {
			next_eval_cost = INT_MAX;
			//printf("[search]depth: %d\n", depth);
			clear = idastar_sc(depth, mid_answer);
			//printf("[end]nodes: %d\n", node_cnt);
			node_cnt = 0;
		}
		if (clear) {
			//cout << "OK!" << endl;
		}
		else {  // 圧縮できる方法が見つからなかったら、元々の操作をする
			//cout << "NG" << endl;
			if (mid_answer.sel_cnt == 0 || mid_answer.place[mid_answer.y][mid_answer.x] != mid_begin_sel) {
				status_sellect(mid_answer, mid_answer.replace[mid_begin_sel] % w, mid_answer.replace[mid_begin_sel] / w);
			}
			for (int j = begin_num; j < end_num; j++) {
				if (operation[j] == 0)
					status_move(mid_answer, 0, 1);
				else if (operation[j] == 1)
					status_move(mid_answer, 1, 0);
				else if (operation[j] == 2)
					status_move(mid_answer, 0, -1);
				else if (operation[j] == 3)
					status_move(mid_answer, -1, 0);
				else {
					int rep = mid_answer.replace[operation[j] >> 4];
					if(mid_answer.place[mid_answer.y][mid_answer.x] != (operation[j] >> 4))
						status_sellect(mid_answer, rep % w, rep / w);
				}
			}
		}
		if (end_num == sta_array_size - 1) {
			assert(pre_comp == sta_array[sta_array_size - 1].place);
			assert(mid_answer.place == sta_array[sta_array_size - 1].place);
			end_num++;
			break;
		}
		
	}
	if (end_num <= sta_array_size - 1) {  // 探索が終わらなかった部分は元々の操作をする
		assert(mid_answer.place == sta_array[end_num].place);
		if (mid_answer.sel_cnt == 0 || mid_answer.place[mid_answer.y][mid_answer.x] != mid_sel) {
			status_sellect(mid_answer, mid_answer.replace[mid_sel] % w, mid_answer.replace[mid_sel] / w);
		}
		for (int j = end_num; j < sta_array_size - 1; j++) {
			if (operation[j] == 0)
				status_move(mid_answer, 0, 1);
			else if (operation[j] == 1)
				status_move(mid_answer, 1, 0);
			else if (operation[j] == 2)
				status_move(mid_answer, 0, -1);
			else if (operation[j] == 3)
				status_move(mid_answer, -1, 0);
			else {
				int rep = mid_answer.replace[operation[j] >> 4];
				if (mid_answer.place[mid_answer.y][mid_answer.x] != (operation[j] >> 4))
					status_sellect(mid_answer, rep % w, rep / w);
			}
		}
		assert(mid_answer.place == sta_array[sta_array_size - 1].place);
	}

	answer = mid_answer;
	complete = pre_comp;
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			goal_place[complete[y][x]] = x + y * w;
	assert(mid_answer.place == complete);

	//status_show(answer);
	status_show_cost(answer);

	//output_csv(sta, answer, "ShortCut.csv");
}


void shortcut_solve(Status& sta) {
	Status pre_answer = sta;
	ow_solve(pre_answer);
  //output_csv(sta, answer, "ow_solve.csv");
	shortcut(sta, pre_answer);
	assert(answer.sel_cnt <= sel_lim);
	assert(answer.place == complete);
	clock_t start = clock();
	while ((clock() - start) / CLOCKS_PER_SEC < TIME_LIMIT * 5) {
		pre_answer = answer;
    int pre_cost = pre_answer.total_cost;
    cout << maxlength << endl;
		shortcut(sta, pre_answer);
    if (pre_cost <= answer.total_cost + answer.total_cost / 10000) {
      maxlength+=5;
    }
		assert(answer.sel_cnt <= sel_lim);
		assert(answer.place == complete);
	}
	//output_csv(sta, answer, "ShortCut.csv");
}