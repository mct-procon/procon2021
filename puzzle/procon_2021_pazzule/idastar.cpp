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

//IDA*�ɂ��摜�����T��
int max_depth = 0; //���ʂȌv�Z���Ȃ����߂�+1�ł͂Ȃ��O��̌v�Z�œ����őP�̕]���l�ɂ���
bool idastar(int depth, int count, Status &status) {
	if (status.place == complete) {
		answer = status;
		return true;
	}
	if (depth == 0) {
		max_depth = min(max_depth, count + status.eval_cost / 2 + ((status.eval_cost % 2 == 0) ? 0 : 1));
		return 0;
	}

	int d[2][4] = {
		{ 0, 1, 0, -1},
		{ 1, 0, -1, 0}
	};

	bool clear = false;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			for (int k = 0; k < ((search_dir)? 2 : 4); k++) {
				//����ւ���
				int next_x = (x + d[0][k] + w) % w, next_y = (y + d[1][k] + h) % h;
			    int pre_x = status.x, pre_y = status.y;
				if (status.x == next_x  && status.y == next_y)
					continue;
				if (status.x != x || status.y != y) {
					if (status.sel_cnt >= sel_lim) continue;
					status_sellect(status, x, y);
				}

				//���ۂɓǂ�
				status_move(status, d[0][k], d[1][k]);
				if (depth * 2 >= status.eval_cost)
					clear = idastar(depth - 1, count + 1, status);
				else
					max_depth = min(max_depth, count + 1 + status.eval_cost / 2 + ((status.eval_cost % 2 == 0) ? 0 : 1));
				if (clear) return true;
				status_unmove(status, pre_x, pre_y);
			}
		}
	}

	return clear;
}

//�����̗���(�����ł͔����[���̂�)
void idastar_solve(Status &status){
	bool clear = false;
	for (int depth = status.eval_cost / 2 + ((status.eval_cost % 2 == 0) ? 0 : 1); !clear; depth = max_depth) {
		max_depth = 10000000;
		printf("[search]depth: %d\n", depth);
		clear = idastar(depth, 0, status);
	}
	status_show(answer);
	status_show_cost(answer);
}