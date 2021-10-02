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
namespace IDA {
	int next_eval_cost = 0; //���ʂȌv�Z���Ȃ����߂�+1�ł͂Ȃ��O��̌v�Z�œ����őP�̕]���l�ɂ���
	const int d[2][4] = {
			{ 0, 1, 0, -1},
			{ 1, 0, -1, 0}
	};
	int node_cnt = 0;
}
using namespace IDA;
bool idastar(int goal_eval_cost, Status &sta) {
	node_cnt++;
	if (sta.place == complete) {
		answer = sta;
		return true;
	}
	if (goal_eval_cost < sta.compare_cost) {
		next_eval_cost = min(next_eval_cost, sta.compare_cost);
		return 0;
	}

	bool clear = false;

	//�����Ă����𓮂���
	if (sta.sel_cnt != 0) {	//�����������Ă����Ԃł���
		for (int k = 0; k < ((search_dir) ? 2 : 4); k++) {	//����������

			//���O�ɖ߂����������Ȃ�
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
				clear = idastar(goal_eval_cost, sta);
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

	//�����ւ���
	if (sta.sel_cnt < sel_lim && !sta.sellected) {	//�񐔐����ƘA������
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int pre_x = sta.x, pre_y = sta.y;
				if (sta.x != x || sta.y != y) {
					status_sellect(sta, x, y);
					sta.sellected = 1;
				}
				else
					continue;

				if (goal_eval_cost >= sta.compare_cost) {
					clear = idastar(goal_eval_cost, sta);
					if (clear) return true;
				}
				else {
					next_eval_cost = min(next_eval_cost, sta.compare_cost);
				}

				status_unsellect(sta, pre_x, pre_y);
				sta.sellected = 0;
			}
		}
	}

	return clear;
}

//�����̗���(�����ł͔����[���̂�)
void idastar_solve(Status &sta){
	bool clear = false;
	for (int depth = sta.compare_cost + sel_rate; !clear; depth = next_eval_cost) {
		next_eval_cost = INT_MAX;
		printf("[search]depth: %d\n", depth);
		clear = idastar(depth, sta);
		printf("[end]nodes: %d\n", node_cnt);
		node_cnt = 0;
	}
	status_show(answer);
	status_show_cost(answer);
}