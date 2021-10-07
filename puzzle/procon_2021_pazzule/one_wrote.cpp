#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include "status.h"
#include "lib.h"
#define abs(x) ((x > 0) ? x : -x)
using namespace std;
extern vector<vector<unsigned char> > complete;
extern Status answer;
extern vector<int> goal_place;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir;


namespace {
	vector<bool> dontmove;

	// �ړI�̏ꏊ�܂ňړ�����(���D��T��) p => �ړI�n a => ���łɂ��낦���Ƃ���܂� m =>�@����������_��
	void move_place(Status& status, int px, int py) {
		vector<vector<int> > table(h, vector<int>(w, 256));
		vector<int> route;
		queue<pair<int, int> > q;
		int dx[4] = { 0, 1, 0, -1 }, dy[4] = { 1, 0, -1, 0 }, nx = px, ny = py, x, i;

		// �T��
		int dis_x = (px - status.x);
		int dis_y = py - status.y;
		table[status.y][status.x] = 0;
		while (table[py][px] == 256) {
			if (q.empty()) {
				cout << "Queue is Empty!!" << endl;
				return;
			}
			pair<int, int> p = q.front(); q.pop();
			int x = p.first, y = p.second;
			for (int k = 0; k < 4; k++) {
				int next_x = (x + dx[k] + w) % w, next_y = (y + dy[k] + h) % h;
				if (table[next_y][next_x] != 256) continue;
				if (dontmove[next_x + next_y * w]) continue;

				table[next_y][next_x] = table[y][x] + 1;

				dis_x = (px - next_x);
				dis_y = (py - next_y);
				q.push(make_pair(next_x, next_y));
			}
		}


		// ����
		i = 0;
		route.resize(table[py][px]);
		for (int i = 0; i < table[py][px]; i++) {
			int res_k = -1;
			int res_num = 256;
			for (int k = 0; k < 4; k++) {
				int next_x = (nx + dx[k] + w) % w, next_y = (ny + dy[k] + h) % h;
				if (table[next_y][next_x] < res_num) {
					res_num = table[next_y][next_x];
					res_k = k;
				}
			}
			nx += dx[res_k];
			ny += dy[res_k];
			route[table[py][px] - i - 1] = (res_k + 2) % 4;
		}

		//�ړ�
		for (i = 0; i < table[py][px]; i++) {
			status_move(status, dx[route[i]], dy[route[i]]);
		}

	}

	//�@�E�[2��̂��낦�ɂ����ꍇ�̑Ώ�
	void avoid_bad_case(Status& status, int x, int y) {
		int R = w - 1, L = w - 2;
		unsigned char compL = complete[y][L], compR = complete[y][R];
		if (status.place[y + 1][L] == compR && status.place[y][R] == compL && status.x == L && status.y == y) {
			status_move(status, 1, 0);
		}

		if (status.place[y + 1][L] == compL && status.place[y][R] == compR && status.y == y) {
			status_move(status, 0, 1);
		}

		if (status.place[y][L] == compR && status.place[y][R] == compL) {
			move_place(status, L, y + 1);
			status_move(status, 0, -1);
		}

		if (status.place[y + 1][L] == compR && status.place[y][R] == compL && status.x == w - 2 && status.y == y) {
			status_move(status, 1, 0);
		}

		if (status.place[y + 1][L] == compL && status.place[y + 1][R] == compR) {
			move_place(status, L, y);
			status_move(status, 0, 1);
		}

		if (status.place[y + 1][L] == compL && ((status.x == x && status.y == y) || (status.x == x + 1 && status.y == y))) {
			move_place(status, R, y + 1);
		}

		if (status.place[y][L] == compL && status.place[y + 1][L] == compR) {
			move_place(status, w - 3, y + 1);
			status_move(status, 1, 0);
		}
		if (status.place[y][L] == compL && status.place[y + 1][R] == compR) {
			move_place(status, R, y + 2);
			status_move(status, 0, -1);
		}
		if (status.place[y][L] == compL) {
			move_place(status, R, y);
			status_move(status, -1, 0);
		}

		if (status.place[y][R] == compR && status.place[y + 1][R] == compL) {
			move_place(status, R, y + 2);
			status_move(status, 0, -1);
		}
	}

	// ����2�i�����낦�ɂ����Ƃ��̑Ώ��@
	void avoid_bad_case2(Status& status, int x, int y) {
		if (status.place[h - 2][x + 1] == complete[h - 2][x]) {
			move_place(status, x + 2, h - 2);
			status_move(status, -1, 0);
		}

		if (status.place[h - 2][x] == complete[h - 1][x] && status.place[h - 1][x] == complete[h - 2][x]) {
			move_place(status, x + 1, h - 1);
			status_move(status, -1, 0);

			status_move(status, 0, -1);
		}

		if (status.place[h - 1][x] == complete[h - 1][x] && status.place[h - 2][x + 1] == complete[h - 2][x]) {
			if (status.x == x && status.y == h - 2) {
				status_move(status, 1, 0);
			}
			else {
				move_place(status, x + 2, h - 2);
				status_move(status, -1, 0);
			}
		}

		if (status.place[h - 2][x] == complete[h - 2][x] && status.place[h - 1][x + 1] == complete[h - 1][x]) {
			if (status.x == x && status.y == h - 1) {
				status_move(status, 1, 0);
			}
			else {
				move_place(status, x + 2, h - 1);
				status_move(status, -1, 0);
			}
		}

		if (status.place[h - 2][x + 1] == complete[h - 1][x] && status.place[h - 1][x] == complete[h - 2][x] && status.x == x && status.y == h - 2) {
			status_move(status, 0, 1);
		}
		if (status.place[h - 2][x] == complete[h - 1][x] && status.place[h - 1][x + 1] == complete[h - 2][x] && status.x == x && status.y == h - 1) {
			status_move(status, 0, -1);
		}
		if (status.place[h - 1][x] == complete[h - 1][x] && status.place[h - 1][x + 1] == complete[h - 2][x]) {
			move_place(status, x + 2, h - 1);
			status_move(status, -1, 0);
		}

		if (status.place[h - 2][x] == complete[h - 2][x] && status.place[h - 2][x + 1] == complete[h - 1][x]) {
			move_place(status, x + 2, h - 2);
			status_move(status, -1, 0);
		}

		if (status.place[h - 2][x] == complete[h - 2][x] && status.place[h - 1][x + 1] == complete[h - 1][x]) {
			move_place(status, x + 2, h - 1);
			status_move(status, -1, 0);
		}
		if (status.place[h - 2][x] == complete[h - 2][x]) {
			move_place(status, x, h - 1);
			status_move(status, 0, -1);
		}
	}

	// 4�}�X���邮��̂Ƃ��ׂ̗荇��2�}�X������������̔���
	int check_two_pass(vector<vector<unsigned char> >& place) {
		int x[4] = { w - 2, w - 1, w - 1, w - 2 }, y[4] = { h - 2, h - 2, h - 1, h - 1 };

		for (int i = 0; i < 4; i++) {
			int j = (i + 1) % 4;
			if (place[y[i]][x[i]] == complete[y[i]][x[i]] && place[y[j]][x[j]] == complete[y[j]][x[j]])
				return i;
		}

		return -1;
	}

	// �E��4�}�X�����邮��񂷂��
	// �E���=> r = 1 ����� => r = -1
	void rota_last(Status& sta, int r) {
		int dx[4] = { 1, 0, -1, 0 }, dy[4] = { 0, 1, 0, -1 };

		while (sta.place != complete) {
			int d = 0, g = check_two_pass(sta.place);
			int x[4] = { w - 2, w - 1, w - 1, w - 2 }, y[4] = { h - 2, h - 2, h - 1, h - 1 };

			if (g != -1 && ((x[g] == sta.x && y[g] == sta.y) || (x[(g + 1) % 4] == sta.x && y[(g + 1) % 4] == sta.y))) {
				int dx = x[(g - 2 + 4) % 4] - x[(g - 1 + 4) % 4], dy = y[(g - 2 + 4) % 4] - y[(g - 1 + 4) % 4];
				status_sellect(sta, x[(g - 1 + 4) % 4], y[(g - 1 + 4) % 4]);
				status_move(sta, x[(g - 2 + 4) % 4] - x[(g - 1 + 4) % 4], y[(g - 2 + 4) % 4] - y[(g - 1 + 4) % 4]);
				return;
			}

			if (sta.x == w - 2 && sta.y == h - 2) d = (r == 1) ? 0 : 1;
			if (sta.x == w - 1 && sta.y == h - 2) d = (r == 1) ? 1 : 2;
			if (sta.x == w - 2 && sta.y == h - 1) d = (r == 1) ? 3 : 0;
			if (sta.x == w - 1 && sta.y == h - 1) d = (r == 1) ? 2 : 3;
			status_move(sta, dx[d], dy[d]);
		}

	}
	int rota_last_cost(Status sta, int r) {
		int dx[4] = { 1, 0, -1, 0 }, dy[4] = { 0, 1, 0, -1 }, res = 0;

		while (sta.place != complete) {
			int d = 0, g = check_two_pass(sta.place);
			int x[4] = { w - 2, w - 1, w - 1, w - 2 }, y[4] = { h - 2, h - 2, h - 1, h - 1 };

			if (g != -1 && ((x[g] == sta.x && y[g] == sta.y) || (x[(g + 1) % 4] == sta.x && y[(g + 1) % 4] == sta.y))) {
				return ++res;
			}

			if (sta.x == w - 2 && sta.y == h - 2) d = (r == 1) ? 0 : 1;
			if (sta.x == w - 1 && sta.y == h - 2) d = (r == 1) ? 1 : 2;
			if (sta.x == w - 2 && sta.y == h - 1) d = (r == 1) ? 3 : 0;
			if (sta.x == w - 1 && sta.y == h - 1) d = (r == 1) ? 2 : 3;
			status_move(sta, dx[d], dy[d]);
			res++;
		}

		return res;
	}

}

// �E���̂��̂������āA���ォ�獇�킹�Ă���
void ow_solve(Status& status) {
	// ������Ԃ�����E���ɂ�����������
	status_sellect(status, complete[h - 1][w - 1] % w, complete[h - 1][w - 1] / w);
	dontmove.resize(h * w, 0);

	// ���낦��������=>complete[y][x] ���̐����̌��ݒn=>status.replace[complete[y][x]] �ڕW=>status.replace[complete[y][x]] == y * w + x
	// ��2��ȊO
	for (int y = 0; y < h - 2; y++) {
		for (int x = 0; x < w; x++) {
			int i, dx, dy;
			//int edge = (x == w - 1) ? 1 : 0; // �[�����p

	  // i => ���������������̌��ݒn
			i = status.replace[complete[y][x]];
			int ix = i % w;
			int iy = i / w;
			if (ix == x && iy == y) continue;

			if (x == w - 1) {

				int nx = x, ny = y + 2;

				if ((nx + w) - ix < w / 2 && iy > ny) //�^�ѐ悪�E�ɍs���΂���
				{
					dx = -1;
				}
				else {
					dx = 1;
				}
				if (ny < iy) {
					dy = 1;
				}
				else {
					dy = -1;
				}

				//�㉺���E�����낦�悤�Ƃ���
				while (ix != nx && iy != ny) {
					int next_ix = ix - dx;
					if (next_ix < 0) next_ix = w - 1;
					if (next_ix >= w) next_ix = 0;
					move_place(status, next_ix, iy);
					if (status.y == iy && status.x == next_ix) {
						status_move(status, dx, 0);
						i = i - ix + next_ix;
						ix = next_ix;
					}
					int next_iy = iy - dy;
					if (next_iy < 0) next_iy = h - 1;
					if (next_iy >= h) next_iy = 0;
					move_place(status, ix, next_iy);
					if (status.y == next_iy && status.x == ix) {
						status_move(status, 0, dy);
						i = i - (iy + next_iy) * w;
						iy = next_iy;
					}
				}
				//���E�����낦��
				while (ix != nx) {
					int next_ix = ix - dx;
					if (next_ix < 0) next_ix = w - 1;
					if (next_ix >= w) next_ix = 0;
					move_place(status, next_ix, iy);
					if (status.y == iy && status.x == next_ix) {
						status_move(status, dx, 0);
						i = i - ix + next_ix;
						ix = next_ix;
					}
				}
				// �㉺�����낦��
				while (iy != ny) {
					int next_iy = iy - dy;
					if (next_iy < 0) next_iy = h - 1;
					if (next_iy >= h) next_iy = 0;
					move_place(status, ix, next_iy);
					if (status.y == next_iy && status.x == ix) {
						status_move(status, 0, dy);
						i = i - (iy + next_iy) * w;
						iy = next_iy;
					}
				}


				// �E�[2������낦��
				move_place(status, x, y);
				status_move(status, -1, 0);
				status_move(status, 0, 1);
				status_move(status, 1, 0);
				status_move(status, 0, 1);
				status_move(status, -1, 0);
				status_move(status, 0, -1);
				status_move(status, 0, -1);
				status_move(status, 1, 0);
				status_move(status, 0, 1);

				continue;
			}


			if ((x + w) - ix < w / 2 && iy - 1 > y) //�^�ѐ悪�E�ɍs���΂���
			{
				dx = -1;
			}
			else {
				dx = 1;
			}
			dy = 1;

			//�㉺���E�����낦�悤�Ƃ���
			while (ix != x && iy - 1 > y) {
				int next_ix = ix - dx;
				if (next_ix < 0) next_ix = w - 1;
				if (next_ix >= w) next_ix = 0;
				move_place(status, next_ix, iy);
				if (status.y == iy && status.x == next_ix) {
					status_move(status, dx, 0);
					i = i - ix + next_ix;
					ix = next_ix;
				}
				int next_iy = iy - dy;
				if (next_iy < 0) next_iy = h - 1;
				if (next_iy >= h) next_iy = 0;
				move_place(status, ix, next_iy);
				if (status.y == next_iy && status.x == ix) {
					status_move(status, 0, dy);
					i = i - (iy + next_iy) * w;
					iy = next_iy;
				}
			}
			//���E�����낦��
			while (ix != x) {
				int next_ix = ix - dx;
				if (next_ix < 0) next_ix = w - 1;
				if (next_ix >= w) next_ix = 0;
				move_place(status, next_ix, iy);
				if (status.y == iy && status.x == next_ix) {
					status_move(status, dx, 0);
					i = i - ix + next_ix;
					ix = next_ix;
				}
			}
			// �㉺�����낦��
			while (iy != y) {
				int next_iy = iy - dy;
				if (next_iy < 0) next_iy = h - 1;
				if (next_iy >= h) next_iy = 0;
				move_place(status, ix, next_iy);
				if (status.y == next_iy && status.x == ix) {
					status_move(status, 0, dy);
					i = i - (iy + next_iy) * w;
					iy = next_iy;
				}
			}

		}
	}

	// ��2��
	for (int x = 0; x < w - 2; x++) {
		for (int y = h - 2; y < h; y++) {
			int i;

			i = status.replace[complete[y][x]];
			int ix = i % w;
			int iy = i / w;
			if (ix == x && iy == y) continue;


			if (y == h - 2) {
				// ���E�����낦��
				while (ix > x) {
					move_place(status, ix - 1, iy);
					status_move(status, 1, 0);
					ix--;
					i -= 1;
				}
				// �㉺�����낦��
				if (iy > y) {
					move_place(status, ix, iy - 1);
					status_move(status, 0, 1);
					iy--;
					i -= w;
				}
				if (status.y == h - 1)
					status_move(status, 1, 0);
				continue;
			}
			else {
				int nx = x + 2;
				// ���E�����낦��
				while (ix > nx) {
					move_place(status, ix - 1, iy);
					status_move(status, 1, 0);
					ix--;
					i -= 1;
				}
				while (ix < nx) {
					move_place(status, ix + 1, iy);
					status_move(status, -1, 0);
					ix++;
					i += 1;
				}
				// �㉺�����낦��
				if (iy < y) {
					move_place(status, ix, iy + 1);
					status_move(status, 0, -1);
					iy++;
					i += w;
				}
				// ��C�ɂ��낦��
				move_place(status, x, y);
				status_move(status, 0, -1);
				status_move(status, 1, 0);
				status_move(status, 0, 1);
				status_move(status, 1, 0);
				status_move(status, 0, -1);
				status_move(status, -1, 0);
				status_move(status, -1, 0);
				status_move(status, 0, 1);
				status_move(status, 1, 0);
			}

		}
	}


	//�E��4�}�X
	if (rota_last_cost(status, 1) < rota_last_cost(status, -1))
		rota_last(status, 1);
	else
		rota_last(status, -1);

	answer = status;
}
