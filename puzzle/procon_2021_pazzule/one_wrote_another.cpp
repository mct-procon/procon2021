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

typedef pair<int, pair<int, int>> p3;

namespace owa {
  vector<vector<bool>> dontmove;
  int dx[4] = { 0, 1, 0, -1 }, dy[4] = { 1, 0, -1, 0 };

  int kyu_distance(int ax, int ay, int bx, int by) {
    if (ax == bx && ay == by) return 0;
    int x = min(abs(ax - bx), w - abs(ax - bx));
    int y = min(abs(ay - by), h - abs(ay - by));
    int d = abs(x - y);
    int m = min(x, y);
    int ret = d * 5 + m * 6 - 4;
    if (x == y)
      ret += 2;
    return ret;
  }

  int distance(int ax, int ay, int bx, int by) {
    int x = min(abs(ax - bx), w - abs(ax - bx));
    int y = min(abs(ay - by), h - abs(ay - by));
    return x + y;
  }

  // 目的の場所まで移動する(幅優先探索) p => 目的地 a => すでにそろえたところまで m =>　動かしちゃダメ
  void move_place(Status& status, int px, int py) {
    vector<vector<int> > table(h, vector<int>(w, INT_MAX));
    vector<int> route;
    vector<vector<int>> prev(h, vector<int>(w, -1));
    priority_queue<p3, vector<p3>, greater<p3>> q;
    vector<vector<bool>> used(h, vector<bool>(w, 0));
    int  nx = px, ny = py, x, i;

    // 探索
    q.push(make_pair(0, make_pair(status.x, status.y)));
    table[status.y][status.x] = 0;
    used[status.y][status.x] = 1;
    while (!q.empty()) {

      p3 p = q.top(); q.pop();
      int x = p.second.first, y = p.second.second;
      for (int k = 0; k < 4; k++) {
        int next_x = (x + dx[k] + w) % w, next_y = (y + dy[k] + h) % h;
        if (dontmove[next_y][next_x]) continue;
        if (used[next_y][next_x]) continue;

        int cnt = table[y][x] + 1;

        //持ってないピースの
        int goal_x = goal_place[status.place[next_y][next_x]] % w;
        int goal_y = goal_place[status.place[next_y][next_x]] / w;	//目的地
        //評価値差分
        int pas_dis = kyu_distance(goal_x, goal_y, next_x, next_y);
        int new_dis = kyu_distance(goal_x, goal_y, x, y);
        cnt += (new_dis - pas_dis);
        if (table[next_y][next_x] <= cnt) continue;

        table[next_y][next_x] = cnt;
        prev[next_y][next_x] = k;

        if (table[py][px] != INT_MAX) continue;
        used[next_y][next_x] = 1;
        q.push(make_pair(cnt, make_pair(next_x, next_y)));
      }
    }
    assert(table[py][px] != INT_MAX);


    // 復元
    i = 0;
    while (!(status.y == ny && status.x == nx)) {
      int res_k = prev[ny][nx];
      prev[ny][nx] = -1;
      assert(res_k != -1);
      nx = (nx - dx[res_k] + w) % w;
      ny = (ny - dy[res_k] + h) % h;
      route.push_back(res_k);
    }
    reverse(route.begin(), route.end());

    //移動
    for (i = 0; i < route.size(); i++) {
      status_move(status, dx[route[i]], dy[route[i]]);
    }
    assert(status.x == px && status.y == py);

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

  vector<int> carry_route(int px, int py, int sx, int sy) {
    vector<vector<int> > table(h, vector<int>(w, INT_MAX));
    vector<vector<int>> prev(h, vector<int>(w, -1));
    vector<int> route;
    priority_queue<p3, vector<p3>, greater<p3>> q;
    vector<vector<bool>> used(h, vector<bool>(w, 0));
    int nx = px, ny = py, x, i;

    // 探索
    q.push(make_pair(0, make_pair(sx, sy)));
    table[sy][sx] = 0;
    used[sy][sx] = 1;
    while (!q.empty()) {

      p3 p = q.top(); q.pop();
      int x = p.second.first, y = p.second.second;
      for (int k = 0; k < 4; k++) {
        int next_x = (x + dx[k] + w) % w, next_y = (y + dy[k] + h) % h;
        if (dontmove[next_y][next_x]) continue;
        if (used[next_y][next_x]) continue;

        int cnt = table[y][x] + 2;
        if (prev[y][x] == k) {
          cnt += 2;
        }

        if (table[next_y][next_x] <= cnt) continue;
        table[next_y][next_x] = cnt;
        prev[next_y][next_x] = k;

        if (table[py][px] != INT_MAX) continue;
        used[next_y][next_x] = 1;
        q.push(make_pair(cnt, make_pair(next_x, next_y)));
      }
    }

    assert(table[py][px] != INT_MAX);


    // 復元
    i = 0;
    int res_k = -1;
    while (!(sy == ny && sx == nx)) {
      res_k = prev[ny][nx];
      prev[ny][nx] = -1;
      assert(res_k != -1);
      nx = (nx - dx[res_k] + w) % w;
      ny = (ny - dy[res_k] + h) % h;
      route.push_back(res_k);
    }
    reverse(route.begin(), route.end());

    return route;
  }

  void nomalmove(Status& status, int x, int y) {
    if (dontmove[y][x]) return;
    int i;

    // i => 動かしたい数字の現在地
    i = status.replace[complete[y][x]];
    int ix = i % w;
    int iy = i / w;
    if (ix == x && iy == y) {
      dontmove[y][x] = 1;
      return;
    }

    vector<int> route = carry_route(x, y, ix, iy);
    for (int r : route) {
      int next_ix = ix + dx[r];
      int next_iy = iy + dy[r];
      if (next_ix < 0) next_ix = w - 1;
      if (next_ix >= w) next_ix = 0;
      if (next_iy < 0) next_iy = h - 1;
      if (next_iy >= h) next_iy = 0;
      dontmove[iy][ix] = 1;
      assert(!dontmove[next_iy][next_ix]);
      move_place(status, next_ix, next_iy);
      dontmove[iy][ix] = 0;
      assert(status.y == next_iy && status.x == next_ix);
      status_move(status, -dx[r], -dy[r]);
      ix = next_ix;
      iy = next_iy;
      i = iy * w + ix;
    }

    assert(status.place[y][x] == complete[y][x]);
    assert(y == iy && x == ix);
    dontmove[y][x] = 1;
  }

}

// 右下のものを持って、左上から合わせていく
void owa_solve(Status& status) {
  using namespace owa;
  // 初期状態だから右下にあるやつが分かる
  status_sellect(status, complete[h - 1][w - 1] % w, complete[h - 1][w - 1] / w);
  dontmove.resize(h);
  for (int i = 0; i < h; i++) {
    dontmove[i].resize(w, 0);
  }

  // そろえたい数字=>complete[y][x] その数字の現在地=>status.replace[complete[y][x]] 目標=>status.replace[complete[y][x]] == y * w + x
  for (int k = 0; k < max(h, w) - 2; k++) {
    for (int y = 0; y <= k && y < h - 2; y++) {
      for (int x = 0; x <= k && x < w - 2; x++) {
        nomalmove(status, x, y);
      }
    }
  }


  // 右2列
  for (int y = 0; y < h - 2; y++) {
    for (int x = w - 2; x < w; x++) {
      int i;

      i = status.replace[complete[y][x]];
      int ix = i % w;
      int iy = i / w;
      if (ix == x && iy == y) {
        dontmove[y][x] = 1;
        if (x == w - 2 && status.x == w - 1 && status.y == y && status.place[y + 1][x + 1] == complete[y][x + 1])
          status_move(status, 0, 1);
        continue;
      }


      if (x == w - 2) {
        vector<int> route = carry_route(x, y, ix, iy);
        for (int r : route) {
          int next_ix = ix + dx[r];
          int next_iy = iy + dy[r];
          if (next_ix < 0) next_ix = w - 1;
          if (next_ix >= w) next_ix = 0;
          if (next_iy < 0) next_iy = h - 1;
          if (next_iy >= h) next_iy = 0;
          dontmove[iy][ix] = 1;
          move_place(status, next_ix, next_iy);
          dontmove[iy][ix] = 0;
          assert(status.y == next_iy && status.x == next_ix);
          status_move(status, -dx[r], -dy[r]);
          ix = next_ix;
          iy = next_iy;
          i = iy * w + ix;
        }

        if (status.x == w - 1 && status.place[y + 1][x + 1] == complete[y][x + 1])
          status_move(status, 0, 1);

        dontmove[y][x] = 1;
        assert(status.place[y][x] == complete[y][x]);
        continue;
      }
      else {
        int ny = y + 2;
        // 上下をそろえる
        vector<int> route = carry_route(x, ny, ix, iy);
        for (int r : route) {
          int next_ix = ix + dx[r];
          int next_iy = iy + dy[r];
          if (next_ix < 0) next_ix = w - 1;
          if (next_ix >= w) next_ix = 0;
          if (next_iy < 0) next_iy = h - 1;
          if (next_iy >= h) next_iy = 0;
          dontmove[iy][ix] = 1;
          move_place(status, next_ix, next_iy);
          dontmove[iy][ix] = 0;
          assert(status.y == next_iy && status.x == next_ix);
          status_move(status, -dx[r], -dy[r]);
          ix = next_ix;
          iy = next_iy;
          i = iy * w + ix;
        }


        // 一気にそろえる
        dontmove[iy][ix] = 1;
        if (!(ix == x && iy == ny))
          assert(ix == x && iy == ny);
        move_place(status, x, y);
        assert(status.y == y && status.x == x);
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
      }

    }
  }

  // 下2列
  for (int x = 0; x < w - 2; x++) {
    for (int y = h - 2; y < h; y++) {
      int i;

      i = status.replace[complete[y][x]];
      int ix = i % w;
      int iy = i / w;
      if (ix == x && iy == y) {
        dontmove[y][x] = 1;
        if (y == h - 2 && status.x == x && status.y == h - 1 && status.place[y + 1][x + 1] == complete[y + 1][x])
          status_move(status, 1, 0);
        continue;
      }


      if (y == h - 2) {
        vector<int> route = carry_route(x, y, ix, iy);
        for (int r : route) {
          int next_ix = ix + dx[r];
          int next_iy = iy + dy[r];
          if (next_ix < 0) next_ix = w - 1;
          if (next_ix >= w) next_ix = 0;
          if (next_iy < 0) next_iy = h - 1;
          if (next_iy >= h) next_iy = 0;
          dontmove[iy][ix] = 1;
          move_place(status, next_ix, next_iy);
          dontmove[iy][ix] = 0;
          assert(status.y == next_iy && status.x == next_ix);
          status_move(status, -dx[r], -dy[r]);
          ix = next_ix;
          iy = next_iy;
          i = iy * w + ix;
        }
        if (status.y == h - 1 && status.place[y + 1][x + 1] == complete[y + 1][x])
          status_move(status, 1, 0);

        dontmove[y][x] = 1;
        assert(status.place[y][x] == complete[y][x]);
        continue;
      }
      else {
        int nx = x + 2;
        // 左右をそろえる
        vector<int> route = carry_route(nx, y, ix, iy);
        for (int r : route) {
          int next_ix = ix + dx[r];
          int next_iy = iy + dy[r];
          if (next_ix < 0) next_ix = w - 1;
          if (next_ix >= w) next_ix = 0;
          if (next_iy < 0) next_iy = h - 1;
          if (next_iy >= h) next_iy = 0;
          dontmove[iy][ix] = 1;
          assert(!dontmove[next_iy][next_ix]);
          move_place(status, next_ix, next_iy);
          dontmove[iy][ix] = 0;
          assert(status.y == next_iy && status.x == next_ix);
          status_move(status, -dx[r], -dy[r]);
          ix = next_ix;
          iy = next_iy;
          i = iy * w + ix;
        }

        // 一気にそろえる
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


  //右下4マス
  if (rota_last_cost(status, 1) < rota_last_cost(status, -1))
    rota_last(status, 1);
  else
    rota_last(status, -1);

  answer = status;
}
