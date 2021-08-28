#include <iostream>
#include <vector>
#include <queue>
#include "status.h"
#include "lib.h"
using namespace std;
extern vector<vector<unsigned char> > complete;
extern Status answer;
extern vector<int> goal_place;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir;


// 右下のものを持って、左上から合わせていく
//　注意：右端2列と下段2列は特殊なやり方で
void ow_solve(Status* status) {
	status->sellect(complete[h - 1][w - 1] % w, complete[h - 1][w - 1] / w, sel_rate);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			// 下2列以外	
			if (y < h - 2) {
				// 隣りに行く(左に行くなら左隣、右に行くなら右隣)
				bool yd = false;
				if (goal_place[complete[y][x]] % w) yd = true;
				if (goal_place[complete[y][x]] / w == status->y && yd) { // y座標が同じでかつ上に上ったほうが速い場合
					if (status->y == h - 1) status->move(0, -1, h, w, swap_rate, goal_place);
					else status->move(0, 1, h, w, swap_rate, goal_place);
				}
				while (1) {

				}

			    // 左右をそろえる

				// 上下をそろえる
			}
		}
	}


	answer = *status;
	answer.show();
	answer.show_cost();
}