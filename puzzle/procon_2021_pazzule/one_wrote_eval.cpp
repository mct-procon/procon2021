#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <cassert>
#include "status.h"
#include "lib.h"
#define abs(x) ((x > 0)? x : -x)
using namespace std;
extern vector<vector<unsigned char> > complete;
extern Status answer;
extern vector<int> goal_place;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir;


namespace {
	// 目的の場所まで移動する(A*アルゴリズム) p => 目的地 a => すでにそろえたところまで m =>　動かしちゃダメ
	void move_place(Status& status, vector<int>& cur_pos, int px, int py, int ax, int ay, int mx, int my) {
		vector<vector<int> > table(h, vector<int>(w, 256));
		vector<int> route;
		priority_queue<pair<int, int> > q;
		int dx[4] = { 0, 1, 0, -1 }, dy[4] = { 1, 0, -1, 0 }, nx = px, ny = py, i;

		// 探索
		q.push({ abs(px - status.x) + abs(py - status.y), status.x + status.y * w });
		table[status.y][status.x] = 0;
		while (table[py][px] == 256) {
			pair<int, int> v = q.top(); q.pop();
			int x = v.second % w, y = v.second / w;
			for (int k = 0; k < 4; k++) {
				if (x + dx[k] < 0 || x + dx[k] >= w || y + dy[k] < 0 || y + dy[k] >= h) continue;
				if (table[y + dy[k]][x + dx[k]] != 256) continue;
				if (mx == x + dx[k] && my == y + dy[k]) continue;
				if (ay < h - 2 && y + dy[k] < ay || (y + dy[k] == ay && x + dx[k] < ax)) continue;
				if (ay >= h - 2 && (x + dx[k] < ax || y + dy[k] < h - 2)) continue;

				table[y + dy[k]][x + dx[k]] = table[y][x] + 1;
				q.push({ abs(px - (x + dx[k])) + abs(py - (y + dy[k])), x + dx[k] + (y + dy[k]) * w });
			}
		}

		// 復元
		i = 0;
		route.resize(table[py][px]);
		for (int i = 0; i < table[py][px]; i++) {
			int res_k = -1;
			int res_num = 256;
			for (int k = 0; k < 4; k++) {
				if (nx + dx[k] < 0 || nx + dx[k] >= w || ny + dy[k] < 0 || ny + dy[k] >= h) continue;
				if (table[ny + dy[k]][nx + dx[k]] < res_num) {
					res_num = table[ny + dy[k]][nx + dx[k]];
					res_k = k;
				}
			}
			nx += dx[res_k];
			ny += dy[res_k];
			route[table[py][px] - i - 1] = (res_k + 2) % 4;
		}

		//移動
		for (i = 0; i < table[py][px]; i++) {
			cur_pos[status.place[status.y + dy[route[i]]][status.x + dx[route[i]]]] -= dx[route[i]] + dy[route[i]] * w;
			status_move(status, dx[route[i]], dy[route[i]]);
			cur_pos[status.place[status.y][status.x]] += dx[route[i]] + dy[route[i]] * w;
		}
	}

	//　右端2列のそろえにくい場合の対処
	void avoid_bad_case(Status& status, vector<int>& cur_pos, int x, int y) {
		if (status.place[y + 1][w - 2] == complete[y][w - 1] && status.place[y][w - 1] == complete[y][w - 2] && status.x == w - 2 && status.y == y) {
			cur_pos[status.place[status.y][status.x + 1]] -= 1;
			status_move(status, 1, 0);
			cur_pos[status.place[status.y][status.x]] += 1;
		}

		if (status.place[y + 1][w - 2] == complete[y][w - 2] && status.place[y][w - 1] == complete[y][w - 1] && status.y == y) {
			cur_pos[status.place[status.y + 1][status.x]] -= w;
			status_move(status, 0, 1);
			cur_pos[status.place[status.y][status.x]] += w;
		}

		if (status.place[y][w - 2] == complete[y][w - 1] && status.place[y][w - 1] == complete[y][w - 2]) {
			move_place(status, cur_pos, w - 2, y + 1, x, y, -1, -1);
			cur_pos[status.place[status.y - 1][status.x]] -= -w;
			status_move(status, 0, -1);
			cur_pos[status.place[status.y][status.x]] += -w;
		}

		if (status.place[y + 1][w - 2] == complete[y][w - 1] && status.place[y][w - 1] == complete[y][w - 2] && status.x == w - 2 && status.y == y) {
			cur_pos[status.place[status.y][status.x + 1]] -= 1;
			status_move(status, 1, 0);
			cur_pos[status.place[status.y][status.x]] += 1;
		}

		if (status.place[y + 1][w - 2] == complete[y][w - 2] && status.place[y + 1][w - 1] == complete[y][w - 1]) {
			move_place(status, cur_pos, w - 2, y, x, y, -1, -1);
			cur_pos[status.place[status.y + 1][status.x]] -= w;
			status_move(status, 0, 1);
			cur_pos[status.place[status.y][status.x]] += w;
		}

		if (status.place[y + 1][w - 2] == complete[y][w - 2] && ((status.x == x && status.y == y) || (status.x == x + 1 && status.y == y))) {
			move_place(status, cur_pos, w - 1, y + 1, x, y, w - 2, y + 1);
		}

		if (status.place[y][w - 2] == complete[y][w - 2] && status.place[y + 1][w - 2] == complete[y][w - 1]) {
			move_place(status, cur_pos, w - 3, y + 1, x, y, w - 2, y + 1);
			cur_pos[status.place[status.y][status.x + 1]] -= 1;
			status_move(status, 1, 0);
			cur_pos[status.place[status.y][status.x]] += 1;
		}
		if (status.place[y][w - 2] == complete[y][w - 2] && status.place[y + 1][w - 1] == complete[y][w - 1]) {
			move_place(status, cur_pos, w - 1, y + 2, x, y, y + 1, w - 1);
			cur_pos[status.place[status.y - 1][status.x]] -= -w;
			status_move(status, 0, -1);
			cur_pos[status.place[status.y][status.x]] += -w;
		}
		if (status.place[y][w - 2] == complete[y][w - 2]) {
			move_place(status, cur_pos, w - 1, y, x, y, w - 2, y);
			cur_pos[status.place[status.y][status.x - 1]] -= -1;
			status_move(status, -1, 0);
			cur_pos[status.place[status.y][status.x]] += -1;
		}

		if (status.place[y][w - 1] == complete[y][w - 1] && status.place[y + 1][w - 1] == complete[y][w - 2]) {
			move_place(status, cur_pos, w - 1, y + 2, x, y, w - 1, y + 1);
			cur_pos[status.place[status.y - 1][status.x]] -= -w;
			status_move(status, 0, -1);
			cur_pos[status.place[status.y][status.x]] += -w;
		}
	}

	// 下の2段がそろえにくいときの対処法
	void avoid_bad_case2(Status& status, vector<int>& cur_pos, int x, int y) {
		if (status.place[h - 2][x + 1] == complete[h - 2][x]) {
			move_place(status, cur_pos, x + 2, h - 2, x, y, x + 1, h - 2);
			cur_pos[status.place[status.y][status.x - 1]] -= -1;
			status_move(status, -1, 0);
			cur_pos[status.place[status.y][status.x]] += -1;
		}

		if (status.place[h - 2][x] == complete[h - 1][x] && status.place[h - 1][x] == complete[h - 2][x]) {
			move_place(status, cur_pos, x + 1, h - 1, x, y, x, h - 2);
			cur_pos[status.place[status.y][status.x - 1]] -= -1;
			status_move(status, -1, 0);
			cur_pos[status.place[status.y][status.x]] += -1;

			cur_pos[status.place[status.y - 1][status.x]] -= -w;
			status_move(status, 0, -1);
			cur_pos[status.place[status.y][status.x]] += -w;
		}

		if (status.place[h - 1][x] == complete[h - 1][x] && status.place[h - 2][x + 1] == complete[h - 2][x]) {
			if (status.x == x && status.y == h - 2) {
				cur_pos[status.place[status.y][status.x + 1]] -= 1;
				status_move(status, 1, 0);
				cur_pos[status.place[status.y][status.x]] += 1;
			}
			else {
				move_place(status, cur_pos, x + 2, h - 2, x, y, x + 1, h - 2);
				cur_pos[status.place[status.y][status.x - 1]] -= -1;
				status_move(status, -1, 0);
				cur_pos[status.place[status.y][status.x]] += -1;
			}
		}

		if (status.place[h - 2][x] == complete[h - 2][x] && status.place[h - 1][x + 1] == complete[h - 1][x]) {
			if (status.x == x && status.y == h - 1) {
				cur_pos[status.place[status.y][status.x + 1]] -= 1;
				status_move(status, 1, 0);
				cur_pos[status.place[status.y][status.x]] += 1;
			}
			else {
				move_place(status, cur_pos, x + 2, h - 1, x, y, x + 1, h - 1);
				cur_pos[status.place[status.y][status.x - 1]] -= -1;
				status_move(status, -1, 0);
				cur_pos[status.place[status.y][status.x]] += -1;
			}
		}

		if (status.place[h - 2][x + 1] == complete[h - 1][x] && status.place[h - 1][x] == complete[h - 2][x] && status.x == x && status.y == h - 2) {
			cur_pos[status.place[status.y + 1][status.x]] -= w;
			status_move(status, 0, 1);
			cur_pos[status.place[status.y][status.x]] += w;
		}
		if (status.place[h - 2][x] == complete[h - 1][x] && status.place[h - 1][x + 1] == complete[h - 2][x] && status.x == x && status.y == h - 1) {
			cur_pos[status.place[status.y - 1][status.x]] -= -w;
			status_move(status, 0, -1);
			cur_pos[status.place[status.y][status.x]] += -w;
		}
		if (status.place[h - 1][x] == complete[h - 1][x] && status.place[h - 1][x + 1] == complete[h - 2][x]) {
			move_place(status, cur_pos, x + 2, h - 1, x, y, x, h - 1);
			cur_pos[status.place[status.y][status.x - 1]] -= -1;
			status_move(status, -1, 0);
			cur_pos[status.place[status.y][status.x]] += -1;
		}

		if (status.place[h - 2][x] == complete[h - 2][x] && status.place[h - 2][x + 1] == complete[h - 1][x]) {
			move_place(status, cur_pos, x + 2, h - 2, x, y, x + 1, h - 2);
			cur_pos[status.place[status.y][status.x - 1]] -= -1;
			status_move(status, -1, 0);
			cur_pos[status.place[status.y][status.x]] += -1;
		}

		if (status.place[h - 2][x] == complete[h - 2][x] && status.place[h - 1][x + 1] == complete[h - 1][x]) {
			move_place(status, cur_pos, x + 2, h - 1, x, y, x + 1, h - 1);
			cur_pos[status.place[status.y][status.x - 1]] -= -1;
			status_move(status, -1, 0);
			cur_pos[status.place[status.y][status.x]] += -1;
		}
		if (status.place[h - 2][x] == complete[h - 2][x]) {
			move_place(status, cur_pos, x, h - 1, x, y, x, h - 2);
			cur_pos[status.place[status.y - 1][status.x]] -= -w;
			status_move(status, 0, -1);
			cur_pos[status.place[status.y][status.x]] += -w;
		}
	}

	// 4マスぐるぐるのときの隣り合う2マスがそろったかの判定
	int check_two_pass(vector<vector<unsigned char> >& place) {
		int x[4] = { w - 2, w - 1, w - 1, w - 2 }, y[4] = { h - 2, h - 2, h - 1, h - 1 };

		for (int i = 0; i < 4; i++) {
			int j = (i + 1) % 4;
			if (place[y[i]][x[i]] == complete[y[i]][x[i]] && place[y[j]][x[j]] == complete[y[j]][x[j]])
				return i;
		}

		return -1;
	}

	// 右下4マスをぐるぐる回すやつ
	// 右回り=> r = 1 左回り => r = -1
	void rota_last(Status& sta, int r) {
		int dx[4] = { 1, 0, -1, 0 }, dy[4] = { 0, 1, 0, -1 };

		while (sta.place != complete) {
			int d, g = check_two_pass(sta.place);
			int x[4] = { w - 2, w - 1, w - 1, w - 2 }, y[4] = { h - 2, h - 2, h - 1, h - 1 };

			if (g != -1 && ((x[g] == sta.x && y[g] == sta.y) || (x[(g + 1) % 4] == sta.x && y[(g + 1) % 4] == sta.y))) {
				int dx = x[(g - 2 + 4) % 4] - x[(g - 1 + 4) % 4], dy = y[(g - 2 + 4) % 4] - y[(g - 1 + 4) % 4];
				status_sellect(sta, x[(g - 1 + 4) % 4], y[(g - 1 + 4) % 4]);
				status_move(sta, x[(g - 2 + 4) % 4] - x[(g - 1 + 4) % 4], y[(g - 2 + 4) % 4] - y[(g - 1 + 4) % 4]);
				return;
			}

			if (sta.x == w - 2 && sta.y == h - 2) d = (r == 1) ? 0 : 1;
			else if (sta.x == w - 1 && sta.y == h - 2) d = (r == 1) ? 1 : 2;
			else if (sta.x == w - 2 && sta.y == h - 1) d = (r == 1) ? 3 : 0;
			else if (sta.x == w - 1 && sta.y == h - 1) d = (r == 1) ? 2 : 3;
			else assert(false);
			status_move(sta, dx[d], dy[d]);
		}

	}
	int rota_last_cost(Status sta, int r) {
		int dx[4] = { 1, 0, -1, 0 }, dy[4] = { 0, 1, 0, -1 }, res = 0;

		while (sta.place != complete) {
			int d, g = check_two_pass(sta.place);
			int x[4] = { w - 2, w - 1, w - 1, w - 2 }, y[4] = { h - 2, h - 2, h - 1, h - 1 };

			if (g != -1 && ((x[g] == sta.x && y[g] == sta.y) || (x[(g + 1) % 4] == sta.x && y[(g + 1) % 4] == sta.y))) {
				return ++res;
			}

			if (sta.x == w - 2 && sta.y == h - 2) d = (r == 1) ? 0 : 1;
			else if (sta.x == w - 1 && sta.y == h - 2) d = (r == 1) ? 1 : 2;
			else if (sta.x == w - 2 && sta.y == h - 1) d = (r == 1) ? 3 : 0;
			else if (sta.x == w - 1 && sta.y == h - 1) d = (r == 1) ? 2 : 3;
			else assert(false);
			status_move(sta, dx[d], dy[d]);
			res++;
		}

		return res;
	}

	// これより上↑はone_wrote.cppそのまま
}

// 値を実際に変える必要はないので値渡し
// 右下のものを持って、左上から合わせていく
int ow_solve_eval(Status status, bool to_ans) {
	vector<int> cur_pos(h * w);
	for (int i = 0; i < h * w; i++) {
		cur_pos[i] = i;
	}
	
	// 初期状態は無理矢理右下行きのを持たせる
	int goal = complete[h - 1][w - 1];
	if (status.sel_cnt == 0 || status.sel_place[status.sel_cnt - 1] != complete[h - 1][w - 1]) {
		status_sellect(status, status.replace[goal] % w, status.replace[goal] / w);
	}

	// そろえたい数字=>complete[y][x] その数字の現在地=>cur_pos[complete[y][x]] 目標=>cur_pos[complete[y][x]] == y * w + x
	// 下2列以外
	for (int y = 0; y < h - 2; y++) {
		for (int x = 0; x < w; x++) {
			int i, dx;
			int edge = (x == w - 1) ? 1 : 0; // 端っこ用


			// i => 動かしたい数字の現在地
			if (x < w - 2) {
				i = cur_pos[complete[y][x]];
				if (i % w == x && i / w == y) continue;
			}
			else if (x == w - 2) {

				avoid_bad_case(status, cur_pos, x, y);
				i = cur_pos[complete[y][w - 1]];
				if (cur_pos[complete[y][x]] % w == x && cur_pos[complete[y][x]] / w == y && cur_pos[complete[y][x + 1]] % w == x + 1 && cur_pos[complete[y][x + 1]] / w == y) continue;
			}
			else if (x == w - 1) {
				i = cur_pos[complete[y][w - 2]];
				if (i % w == x - 1 && i / w == y + 1) continue;
				if (cur_pos[complete[y][x]] % w == x && cur_pos[complete[y][x]] / w == y && cur_pos[complete[y][x + 1]] % w == x + 1 && cur_pos[complete[y][x + 1]] / w == y) continue;
			}

			// 隣りに行く
			if (x - edge != i % w) {
				if (x - edge < i % w)
					move_place(status, cur_pos, i % w - 1, i / w, x, y, i % w, i / w);
				else
					move_place(status, cur_pos, i % w + 1, i / w, x, y, i % w, i / w);
			}
			else if (y + edge != i / w) {
				move_place(status, cur_pos, i % w, i / w - 1, x, y, i % w, i / w);
			}


			// 左右をそろえる
			if (i % w < x - edge) {
				dx = -1;
			}
			else {
				dx = 1;
			}
			while (i % w != x - edge) {
				move_place(status, cur_pos, i % w - dx, i / w, x, y, i % w, i / w);
				if (status.y == i / w && status.x == i % w - dx) {
					cur_pos[status.place[status.y][status.x + dx]] -= dx;
					status_move(status, dx, 0);
					cur_pos[status.place[status.y][status.x]] += dx;
					i -= dx;
				}
			}

			// 上下をそろえる
			while (i / w != y + edge) {
				move_place(status, cur_pos, i % w, i / w - 1, x, y, i % w, i / w);
				if (status.y == i / w - 1 && status.x == i % w) {
					cur_pos[status.place[status.y + 1][status.x]] -= w;
					status_move(status, 0, 1);
					cur_pos[status.place[status.y][status.x]] += w;
					i -= w;
				}
			}

			// 右端2列をそろえる
			if (x == w - 1) {
				move_place(status, cur_pos, x, y, x, y, x - 1, y + 1);
				if (status.y == y && status.x == x) {
					cur_pos[status.place[status.y][status.x - 1]] -= -1;
					status_move(status, -1, 0);
					cur_pos[status.place[status.y][status.x]] += -1;
				}
				if (status.y == y && status.x == x - 1) {
					cur_pos[status.place[status.y + 1][status.x]] -= w;
					status_move(status, 0, 1);
					cur_pos[status.place[status.y][status.x]] += w;
				}
			}

		}
	}

	// 下2列
	for (int x = 0; x < w - 2; x++) {
		for (int y = h - 2; y < h; y++) {
			int i, pre = (y == h - 1) ? 1 : 0;

			if (y == h - 2)
				avoid_bad_case2(status, cur_pos, x, y);

			if (y == h - 2) i = cur_pos[complete[h - 1][x]];
			else i = cur_pos[complete[h - 2][x]];

			// 上下をそろえる
			if (i / w == h - 1) {
				move_place(status, cur_pos, i % w, i / w - 1, x, y, i % w, i / w);
				cur_pos[status.place[status.y + 1][status.x]] -= w;
				status_move(status, 0, 1);
				cur_pos[status.place[status.y][status.x]] += w;
				i -= w;
			}

			// 左右をそろえる
			while (i % w != x + pre) {
				move_place(status, cur_pos, i % w - 1, i / w, x, y, i % w, i / w);
				cur_pos[status.place[status.y][status.x + 1]] -= 1;
				status_move(status, 1, 0);
				cur_pos[status.place[status.y][status.x]] += 1;
				i -= 1;
			}

			// 一気にそろえる
			if (y == h - 1) {
				move_place(status, cur_pos, x, y, x, y, x + 1, y - 1);
				cur_pos[status.place[status.y - 1][status.x]] -= -w;
				status_move(status, 0, -1);
				cur_pos[status.place[status.y][status.x]] += -w;

				cur_pos[status.place[status.y][status.x + 1]] -= 1;
				status_move(status, 1, 0);
				cur_pos[status.place[status.y][status.x]] += 1;
			}
		}
	}


	//右下4マス
	if (rota_last_cost(status, 1) < rota_last_cost(status, -1))
		rota_last(status, 1);
	else
		rota_last(status, -1);

	if(to_ans)
		answer = status;

	return status.total_cost;
}
