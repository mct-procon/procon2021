#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <cassert>
#include "status.h"
#include "lib.h"
//#define abs(x) ((x > 0) ? x : -x)
using namespace std;
extern vector<vector<unsigned char> > complete;
extern Status answer;
extern vector<int> goal_place;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir;


namespace {
  vector<vector<bool>> dontmove;

	// �ړI�̏ꏊ�܂ňړ�����(���D��T��) p => �ړI�n a => ���łɂ��낦���Ƃ���܂� m =>�@����������_��
  void move_place(Status& status, int px, int py) {
    vector<vector<int> > table(h, vector<int>(w, INT_MAX));
    vector<int> route;
    vector<vector<int>> prev(h, vector<int>(w, -1));
    queue<pair<int, int> > q;
    int dx[4] = { 0, 1, 0, -1 }, dy[4] = { 1, 0, -1, 0 }, nx = px, ny = py, x, i;

    // �T��
    q.push(make_pair(status.x, status.y));
    table[status.y][status.x] = 0;
    while (!q.empty()) {
      
      pair<int, int> p = q.front(); q.pop();
      int x = p.first, y = p.second;
      for (int k = 0; k < 4; k++) {
        int next_x = (x + dx[k] + w) % w, next_y = (y + dy[k] + h) % h;
        if (dontmove[next_y][next_x]) continue;

        int cnt = table[y][x] + 5;

        //�����ĂȂ��s�[�X��
        int goal_x = goal_place[status.place[next_y][next_x]] % w;
        int goal_y = goal_place[status.place[next_y][next_x]] / w;	//�ړI�n
        //�]���l����
        int pas_x = min(abs(goal_x - next_x), w - abs(goal_x - next_x));
        int pas_y = min(abs(goal_y - next_y), w - abs(goal_y - next_y));
        int pas_dis = pas_x + pas_y;
        int new_x = min(abs(goal_x - x), w - abs(goal_x - x));
        int new_y = min(abs(goal_y - y), w - abs(goal_y - y));
        int new_dis = new_x + new_y;
        cnt += new_dis - pas_dis;
        if (table[next_y][next_x] <= cnt) continue;

        table[next_y][next_x] = cnt;
        prev[next_y][next_x] = (k + 2) % 4;

        if (table[py][px] != INT_MAX) continue;
        q.push(make_pair(next_x, next_y));
      }
    }
    if (table[py][px] == INT_MAX)
      return;
    assert(table[py][px] != INT_MAX);


    // ����
    i = 0;
    while (!(status.y == ny && status.x == nx)) {
      int res_k = prev[ny][nx];
      prev[ny][nx] = -1;
      assert(res_k != -1);
      nx = (nx + dx[res_k] + w) % w;
      ny = (ny + dy[res_k] + h) % h;
      route.push_back((res_k + 2) % 4);
    }
    reverse(route.begin(), route.end());

    //�ړ�
    for (i = 0; i < route.size(); i++) {
      status_move(status, dx[route[i]], dy[route[i]]);
    }
    assert(status.x == px && status.y == py);

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
  dontmove.resize(h);
  for (int i = 0; i < h; i++) {
    dontmove[i].resize(w, 0);
  }

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
      if (ix == x && iy == y) {
        dontmove[y][x] = 1;
        continue;
      }

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
          dontmove[iy][ix] = 1;
          move_place(status, next_ix, iy);
          dontmove[iy][ix] = 0;
          assert(status.y == iy && status.x == next_ix);
          status_move(status, dx, 0);
          i = i - ix + next_ix;
          ix = next_ix;

          int next_iy = iy - dy;
          if (next_iy < 0) next_iy = h - 1;
          if (next_iy >= h) next_iy = 0;
          dontmove[iy][ix] = 1;
          move_place(status, ix, next_iy);
          dontmove[iy][ix] = 0;
          assert(status.y == next_iy && status.x == ix);
          status_move(status, 0, dy);
          i = i - (iy + next_iy) * w;
          iy = next_iy;
        }
        //���E�����낦��
        while (ix != nx) {
          int next_ix = ix - dx;
          if (next_ix < 0) next_ix = w - 1;
          if (next_ix >= w) next_ix = 0;
          dontmove[iy][ix] = 1;
          move_place(status, next_ix, iy);
          dontmove[iy][ix] = 0;
          assert(status.y == iy && status.x == next_ix);
          status_move(status, dx, 0);
          i = i - ix + next_ix;
          ix = next_ix;
        }
        // �㉺�����낦��
        while (iy != ny) {
          int next_iy = iy - dy;
          if (next_iy < 0) next_iy = h - 1;
          if (next_iy >= h) next_iy = 0;
          dontmove[iy][ix] = 1;
          move_place(status, ix, next_iy);
          dontmove[iy][ix] = 0;
          assert(status.y == next_iy && status.x == ix);
          status_move(status, 0, dy);
          i = i - (iy + next_iy) * w;
          iy = next_iy;
        }

        dontmove[iy][ix] = 1;

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

        dontmove[iy][ix] = 0;
        dontmove[y][x] = 1;
        assert(status.place[y][x] == complete[y][x]);

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
        dontmove[iy][ix] = 1;
        move_place(status, next_ix, iy);
        dontmove[iy][ix] = 0;
        assert(status.y == iy && status.x == next_ix);
        status_move(status, dx, 0);
        i = i - ix + next_ix;
        ix = next_ix;

        int next_iy = iy - dy;
        if (next_iy < 0) next_iy = h - 1;
        if (next_iy >= h) next_iy = 0;
        dontmove[iy][ix] = 1;
        move_place(status, ix, next_iy);
        dontmove[iy][ix] = 0;
        assert(status.y == next_iy && status.x == ix);
        status_move(status, 0, dy);
        i = i - (iy + next_iy) * w;
        iy = next_iy;
      }
      //���E�����낦��
      while (ix != x) {
        int next_ix = ix - dx;
        if (next_ix < 0) next_ix = w - 1;
        if (next_ix >= w) next_ix = 0;
        dontmove[iy][ix] = 1;
        move_place(status, next_ix, iy);
        dontmove[iy][ix] = 0;
        assert(status.y == iy && status.x == next_ix);
        status_move(status, dx, 0);
        i = i - ix + next_ix;
        ix = next_ix;
      }
      // �㉺�����낦��
      while (iy != y) {
        int next_iy = iy - dy;
        if (next_iy < 0) next_iy = h - 1;
        if (next_iy >= h) next_iy = 0;
        dontmove[iy][ix] = 1;
        move_place(status, ix, next_iy);
        dontmove[iy][ix] = 0;
        assert(status.y == next_iy && status.x == ix);
        status_move(status, 0, dy);
        i = i - (iy + next_iy) * w;
        iy = next_iy;
      }

      assert(status.place[y][x] == complete[y][x]);
      assert(y == iy && x == ix);
      dontmove[y][x] = 1;
    }
  }

  // ��2��
  for (int x = 0; x < w - 2; x++) {
    for (int y = h - 2; y < h; y++) {
      int i;

      i = status.replace[complete[y][x]];
      int ix = i % w;
      int iy = i / w;
      if (ix == x && iy == y) {
        dontmove[y][x] = 1;
        continue;
      }


      if (y == h - 2) {
        // ���E�����낦��
        assert(ix >= x);
        while (ix > x) {
          dontmove[iy][ix] = 1;
          move_place(status, ix - 1, iy);
          dontmove[iy][ix] = 0;
          status_move(status, 1, 0);
          ix--;
          i -= 1;
        }
        assert(ix == x);
        // �㉺�����낦��
        if (iy > y) {
          dontmove[iy][ix] = 1;
          move_place(status, ix, iy - 1);
          dontmove[iy][ix] = 0;
          status_move(status, 0, 1);
          iy--;
          i -= w;
        }
        assert(iy == y);
        if (status.y == h - 1)
          status_move(status, 1, 0);

        dontmove[y][x] = 1;
        assert(status.place[y][x] == complete[y][x]);
        continue;
      }
      else {
        int nx = x + 2;
        // ���E�����낦��
        while (ix > nx) {
          dontmove[iy][ix] = 1;
          move_place(status, ix - 1, iy);
          dontmove[iy][ix] = 0;
          status_move(status, 1, 0);
          ix--;
          i -= 1;
        }
        while (ix < nx) {
          dontmove[iy][ix] = 1;
          move_place(status, ix + 1, iy);
          dontmove[iy][ix] = 0;
          status_move(status, -1, 0);
          ix++;
          i += 1;
        }
        // �㉺�����낦��
        if (iy < y) {
          dontmove[iy][ix] = 1;
          move_place(status, ix, iy + 1);
          dontmove[iy][ix] = 0;
          status_move(status, 0, -1);
          iy++;
          i += w;
        }
        // ��C�ɂ��낦��
        dontmove[iy][ix] = 1;
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
        dontmove[iy][ix] = 0;
        dontmove[y][x] = 1;
        assert(status.place[y][x] == complete[y][x]);
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
