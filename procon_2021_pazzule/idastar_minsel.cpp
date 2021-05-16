#include <iostream>
#include <vector>
#include "status.h"
#include "lib.h"
using namespace std;

extern vector<vector<int> > complete;
extern vector<int> goal_place;
extern int h, w, sel_lim, sel_rate, swap_rate;
extern Status answer;

//IDA*�ɂ��摜�����T��
int idastar_minsel(int depth, int count, int sel_lim, Status status, int px, int py) {
	if (status.place == complete) {
		answer = status;
		return 1;
	}
	if (depth == 0) return 0;

	int d[2][4] = {
		{ 0, 1, 0, -1},
		{ 1, 0, -1, 0}
	};

	Status backup_status = status;
	int clear = 0;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			for (int k = 0; k < 4; k++) {
				int sel_num = 0;
				if (j + d[0][k] < 0 || j + d[0][k] >= w || i + d[1][k] < 0 || i + d[1][k] >= h) continue;
				
				if (py != i || px != j) {
					if (sel_lim == 0) {
						status = backup_status;
						continue;
					}
					sel_num++;
					status.sellect(j, i, sel_rate);
				}
				status.move(d[0][k], d[1][k], w, swap_rate, goal_place);
				clear = idastar_minsel(depth - 1, count + 1, sel_lim - sel_num, status, j + d[0][k], i + d[1][k]);
				if (clear) break;

				status = backup_status;
			}
			if (clear) break;
		}
		if (clear) break;
	}
	return clear;
}

void idastar_ms_solve(Status *status) {
	int clear = 0;
	for (int depth = 0; !clear; depth++) {
		clear = idastar_minsel(depth, 0, sel_lim, *status, -1, -1);
		printf("%d\n", depth);
	}
	answer.show();
	answer.show_cost();
}