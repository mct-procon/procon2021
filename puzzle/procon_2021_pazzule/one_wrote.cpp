#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include "status.h"
#include "lib.h"
#define abs(x) ((x > 0)? x : -x)
using namespace std;
extern vector<vector<unsigned char> > complete;
extern Status answer;
extern vector<int> goal_place;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir;


namespace {
	// �ړI�̏ꏊ�܂ňړ�����(A*�A���S���Y��) p => �ړI�n a => ���łɂ��낦���Ƃ���܂� m =>�@����������_��
	void move_place(Status& status, int px, int py, int ax, int ay, int mx, int my) {
		vector<vector<int> > table(h, vector<int>(w, 256));
		vector<int> route;
		priority_queue<pair<int, int> > q;
		int dx[4] = { 0, 1, 0, -1 }, dy[4] = { 1, 0, -1, 0 }, nx = px, ny = py, x, i;

		// �T��
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

		// ����
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

		//�ړ�
		for (i = 0; i < table[py][px]; i++) {
			status_move(status, dx[route[i]], dy[route[i]]);
		}
	}

	//�@�E�[2��̂��낦�ɂ����ꍇ�̑Ώ�
	void avoid_bad_case(Status& status, int x, int y) {
		if (status.place[y + 1][w - 2] == complete[y][w - 1] && status.place[y][w - 1] == complete[y][w - 2] && status.x == w - 2 && status.y == y) {
			status_move(status, 1, 0);
		}

		if (status.place[y + 1][w - 2] == complete[y][w - 2] && status.place[y][w - 1] == complete[y][w - 1] && status.y == y) {
			status_move(status, 0, 1);
		}

		if (status.place[y][w - 2] == complete[y][w - 1] && status.place[y][w - 1] == complete[y][w - 2]) {
			move_place(status, w - 2, y + 1, x, y, -1, -1);
			status_move(status, 0, -1);
		}

		if (status.place[y + 1][w - 2] == complete[y][w - 1] && status.place[y][w - 1] == complete[y][w - 2] && status.x == w - 2 && status.y == y) {
			status_move(status, 1, 0);
		}

		if (status.place[y + 1][w - 2] == complete[y][w - 2] && status.place[y + 1][w - 1] == complete[y][w - 1]) {
			move_place(status, w - 2, y, x, y, -1, -1);
			status_move(status, 0, 1);
		}

		if (status.place[y + 1][w - 2] == complete[y][w - 2] && ((status.x == x && status.y == y) || (status.x == x + 1 && status.y == y))) {
			move_place(status, w - 1, y + 1, x, y, w - 2, y + 1);
		}

		if (status.place[y][w - 2] == complete[y][w - 2] && status.place[y + 1][w - 2] == complete[y][w - 1]) {
			move_place(status, w - 3, y + 1, x, y, w - 2, y + 1);
			status_move(status, 1, 0);
		}
		if (status.place[y][w - 2] == complete[y][w - 2] && status.place[y + 1][w - 1] == complete[y][w - 1]) {
			move_place(status, w - 1, y + 2, x, y, y + 1, w - 1);
			status_move(status, 0, -1);
		}
		if (status.place[y][w - 2] == complete[y][w - 2]) {
			move_place(status, w - 1, y, x, y, w - 2, y);
			status_move(status, -1, 0);
		}

		if (status.place[y][w - 1] == complete[y][w - 1] && status.place[y + 1][w - 1] == complete[y][w - 2]) {
			move_place(status, w - 1, y + 2, x, y, w - 1, y + 1);
			status_move(status, 0, -1);
		}
	}

	// ����2�i�����낦�ɂ����Ƃ��̑Ώ��@
	void avoid_bad_case2(Status& status, int x, int y) {
		if (status.place[h - 2][x + 1] == complete[h - 2][x]) {
			move_place(status, x + 2, h - 2, x, y, x + 1, h - 2);
			status_move(status, -1, 0);
		}

		if (status.place[h - 2][x] == complete[h - 1][x] && status.place[h - 1][x] == complete[h - 2][x]) {
			move_place(status, x + 1, h - 1, x, y, x, h - 2);
			status_move(status, -1, 0);

			status_move(status, 0, -1);
		}

		if (status.place[h - 1][x] == complete[h - 1][x] && status.place[h - 2][x + 1] == complete[h - 2][x]) {
			if (status.x == x && status.y == h - 2) {
				status_move(status, 1, 0);
			}
			else {
				move_place(status, x + 2, h - 2, x, y, x + 1, h - 2);
				status_move(status, -1, 0);
			}
		}

		if (status.place[h - 2][x] == complete[h - 2][x] && status.place[h - 1][x + 1] == complete[h - 1][x]) {
			if (status.x == x && status.y == h - 1) {
				status_move(status, 1, 0);
			}
			else {
				move_place(status, x + 2, h - 1, x, y, x + 1, h - 1);
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
			move_place(status, x + 2, h - 1, x, y, x, h - 1);
			status_move(status, -1, 0);
		}

		if (status.place[h - 2][x] == complete[h - 2][x] && status.place[h - 2][x + 1] == complete[h - 1][x]) {
			move_place(status, x + 2, h - 2, x, y, x + 1, h - 2);
			status_move(status, -1, 0);
		}

		if (status.place[h - 2][x] == complete[h - 2][x] && status.place[h - 1][x + 1] == complete[h - 1][x]) {
			move_place(status, x + 2, h - 1, x, y, x + 1, h - 1);
			status_move(status, -1, 0);
		}
		if (status.place[h - 2][x] == complete[h - 2][x]) {
			move_place(status, x, h - 1, x, y, x, h - 2);
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
void ow_solve(Status &status) {
	// ������Ԃ�����E���ɂ�����������
	status_sellect(status, complete[h - 1][w - 1] % w, complete[h - 1][w - 1] / w);

	// ���낦��������=>complete[y][x] ���̐����̌��ݒn=>status.replace[complete[y][x]] �ڕW=>status.replace[complete[y][x]] == y * w + x
	// ��2��ȊO
	for (int y = 0; y < h - 2; y++) {
		for (int x = 0; x < w; x++) {
			int i, dx;
			int edge = (x == w - 1) ? 1 : 0; // �[�����p

			// i => ���������������̌��ݒn
			if (x < w - 2) {
				i = status.replace[complete[y][x]];
				if (i % w == x && i / w == y) continue;
			}
			else if (x == w - 2) {
        int cur_x = status.replace[complete[y][x]] % w;
        int cur_y = status.replace[complete[y][x]] / w;
        int next_x = status.replace[complete[y][x + 1]] % w;
        int next_y = status.replace[complete[y][x + 1]] / w;
				if (cur_x == x && cur_y == y && next_x == x + 1 && next_y == y) continue;
				if (cur_x % w == x && cur_y == y + 1 && next_x == x && next_y == y) continue;
				avoid_bad_case(status, x, y);
				i = status.replace[complete[y][w - 1]];
        cur_x = status.replace[complete[y][x]] % w;
        cur_y = status.replace[complete[y][x]] / w;
        next_x = status.replace[complete[y][x + 1]] % w;
        next_y = status.replace[complete[y][x + 1]] / w;
				if (cur_x == x && cur_y == y && next_x == x + 1 && next_y == y) continue;
				if (cur_x == x && cur_y == y + 1 && next_x == x && next_y == y) continue;
			}
			else if (x == w - 1) {
				i = status.replace[complete[y][w - 2]];
				if (i % w == x - 1 && i / w == y + 1) continue;
        int cur_x = status.replace[complete[y][x]] % w;
        int cur_y = status.replace[complete[y][x]] / w;
        int next_x = status.replace[complete[y][x - 1]] % w;
        int next_y = status.replace[complete[y][x - 1]] / w;
				if (cur_x == x && cur_y == y && next_x == x - 1 && next_y == y) continue;
			}

      int ix = i % w;
      int iy = i / w;

			// �ׂ�ɍs��
			if (x - edge != ix) {
				if (x - edge < ix)
					move_place(status, ix - 1, iy, x, y, ix, iy);
				else
					move_place(status, ix + 1, iy, x, y, ix, iy);
			}
			else if (y + edge != iy) {
				move_place(status, ix, iy - 1, x, y, ix, iy);
			}


			// ���E�����낦��
			if (ix < x - edge) {
				dx = -1;
			}
			else {
				dx = 1;
			}
      //�㉺���E�����낦�悤�Ƃ���
      //while (ix != x - edge && iy != y + edge) {
      //  move_place(status, ix - dx, iy, x, y, ix, iy);
      //  if (status.y == iy && status.x == ix - dx) {
      //    status_move(status, dx, 0);
      //    ix -= dx;
      //    i -= dx;
      //  }
      //  move_place(status, ix, iy - 1, x, y, ix, iy);
      //  if (status.y == iy - 1 && status.x == ix) {
      //    status_move(status, 0, 1);
      //    iy--;
      //    i -= w;
      //  }
      //}
      //���E�����낦��
			while (ix != x - edge) {
				move_place(status, ix - dx, iy, x, y, ix, iy);
				if (status.y == iy && status.x == ix - dx) {
					status_move(status, dx, 0);
          ix -= dx;
					i -= dx;
				}
			}

			// �㉺�����낦��
			while (iy != y + edge) {
				move_place(status, ix, iy - 1, x, y, ix, iy);
				if (status.y == iy - 1 && status.x == ix) {
					status_move(status, 0, 1);
          iy--;
					i -= w;
				}
			}

			// �E�[2������낦��
			if (x == w - 1) {
				move_place(status, x, y, x, y, x - 1, y + 1);
				if (status.y == y && status.x == x) {
					status_move(status, -1, 0);
				}
				if (status.y == y && status.x == x - 1) {
					status_move(status, 0, 1);
				}
			}
			
		}
	}

	// ��2��
	for (int x = 0; x < w - 2; x++) {
		for (int y = h - 2; y < h; y++) {
			int i, pre = (y == h - 1)?1:0;

			if (y == h - 2) {
        int cur_x = status.replace[complete[y][x]] % w;
        int cur_y = status.replace[complete[y][x]] / w;
        int next_x = status.replace[complete[y + 1][x]] % w;
        int next_y = status.replace[complete[y + 1][x]] / w;
				if (next_x == x && next_y == y && cur_x == x + 1 && cur_y == y) continue; // ���߂��珀�����������Ă���Ƃ�
				if (cur_x == x && cur_y == y && next_x == x && next_y == y + 1) continue; // ���߂��炻����Ă�Ƃ�
				avoid_bad_case2(status, x, y);
				i = status.replace[complete[h - 1][x]];
        cur_x = status.replace[complete[y][x]] % w;
        cur_y = status.replace[complete[y][x]] / w;
        next_x = status.replace[complete[y + 1][x]] % w;
        next_y = status.replace[complete[y + 1][x]] / w;
        if (cur_x == x && cur_y == y && next_x == x && next_y == y + 1) continue; // ���߂��炻����Ă�Ƃ�
				if (next_x == x && next_y == y && cur_x == x + 1 && cur_y == y) continue; // ���߂��珀�����������Ă���Ƃ�
			}
			else {
				i = status.replace[complete[h - 2][x]];
        int cur_x = status.replace[complete[y][x]] % w;
        int cur_y = status.replace[complete[y][x]] / w;
        int next_x = status.replace[complete[y - 1][x]] % w;
        int next_y = status.replace[complete[y - 1][x]] / w;
				if (cur_x == x && cur_y == y && next_x == x && next_y == y - 1) continue; // ���߂��炻����Ă�Ƃ�
			}

      int ix = i % w;
      int iy = i / w;

			// �㉺�����낦��
			if (iy == h - 1) {
				move_place(status, ix, iy - 1, x, y, ix, iy);
				status_move(status, 0, 1);
        iy--;
				i -= w;
			}

			// ���E�����낦��
			while (ix != x + pre) {
				move_place(status, ix - 1, iy, x, y, ix, iy);
				status_move(status, 1, 0);
        ix--;
				i -= 1;
			}

			// ��C�ɂ��낦��
			if (y == h - 1) {
				move_place(status, x, y, x, y, x + 1, y - 1);
				status_move(status, 0, -1);

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
