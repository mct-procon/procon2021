#include <iostream>
#include <vector>
#include "status.h"
#include "lib.h"
using namespace std;

extern vector<vector<int> > complete;
extern vector<int> goal_place;
extern int h, w, sel_lim, sel_rate, swap_rate;
extern Status answer;

//IDA*‚É‚æ‚é‰æ‘œ•œŒ³’Tõ
int idastar_minsel(int depth, int count, int sel_lim, Status status) {
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
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			for (int k = 0; k < 4; k++) {
				int sel_num = 0;
				if (x + d[0][k] < 0 || x + d[0][k] >= w || y + d[1][k] < 0 || y + d[1][k] >= h) continue;
				
				if (status.x == x + d[0][k] && status.y == y + d[1][k])
					continue;
				if (status.x != x || status.y != y) {
					if (sel_lim == 0) {
						status = backup_status;
						continue;
					}
					sel_num++;
					status.sellect(x, y, sel_rate);
				}
				status.move(d[0][k], d[1][k], w, swap_rate, goal_place);
				clear = idastar_minsel(depth - 1, count + 1, sel_lim - sel_num, status);
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
		clear = idastar_minsel(depth, 0, sel_lim, *status);
		printf("depth: %d\n", depth);
	}
	answer.show();
	answer.show_cost();
}