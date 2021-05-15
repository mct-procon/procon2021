#include <iostream>
#include <vector>
#include "status.h"
using namespace std;

extern vector<vector<int> > complete;
extern Status answer;
extern int h, w, sel_rate, swap_rate;

//IDA*‚É‚æ‚é‰æ‘œ•œŒ³’Tõ
int idastar(int depth, int count, Status status, int px, int py) {
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
				if (j + d[0][k] < 0 || j + d[0][k] >= w || i + d[1][k] < 0 || i + d[1][k] >= h) continue;
				
				if (py != i || px != j) status.sellect(j + i * w, sel_rate);
				status.move(d[0][k], d[1][k], j, i, w, swap_rate);

				clear = idastar(depth - 1, count + 1, status, j + d[0][k], i + d[1][k]);
				if (clear) break;

				status = backup_status;
			}
			if (clear) break;
		}
		if (clear) break;
	}
	return clear;
}

//ˆ—‚Ì—¬‚ê(‚±‚±‚Å‚Í”½•œ[‰»‚Ì‚İ)
void idastar_solve(Status *status){
	int clear = 0;
	for (int depth = 0; !clear; depth++) {
		clear = idastar(depth, 0, *status, -1, -1);
		printf("%d\n", depth);
	}
	answer.show();
	answer.show_cost();
}