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


// �E���̂��̂������āA���ォ�獇�킹�Ă���
//�@���ӁF�E�[2��Ɖ��i2��͓���Ȃ�����
void ow_solve(Status* status) {
	status->sellect(complete[h - 1][w - 1] % w, complete[h - 1][w - 1] / w, sel_rate);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			// ��2��ȊO	
			if (y < h - 2) {
				// �ׂ�ɍs��(���ɍs���Ȃ獶�ׁA�E�ɍs���Ȃ�E��)
				bool yd = false;
				if (goal_place[complete[y][x]] % w) yd = true;
				if (goal_place[complete[y][x]] / w == status->y && yd) { // y���W�������ł���ɏ�����ق��������ꍇ
					if (status->y == h - 1) status->move(0, -1, h, w, swap_rate, goal_place);
					else status->move(0, 1, h, w, swap_rate, goal_place);
				}
				while (1) {

				}

			    // ���E�����낦��

				// �㉺�����낦��
			}
		}
	}


	answer = *status;
	answer.show();
	answer.show_cost();
}