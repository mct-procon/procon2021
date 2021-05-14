#include <iostream>
#include <vector>
#include "status.h"
using namespace std;

extern vector<vector<int> > complete;
extern int h, w, sel_lim, sel_rate, swap_rate;
extern status answer;

//IDA*‚É‚æ‚é‰æ‘œ•œŒ³’Tõ
int idastar_minsel(int depth, int count, int sel_lim, vector<vector<int> > table, status ans, int px, int py) {
	if (table == complete) {
		answer = ans;
		return 1;
	}
	if (depth == 0) return 0;

	int d[2][4] = {
		{ 0, 1, 0, -1},
		{ 1, 0, -1, 0}
	};

	vector<vector<int> > backup_table = table;
	status backup_ans = ans;
	int clear = 0;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			for (int k = 0; k < 4; k++) {
				int sel_num = 0;
				if (j + d[0][k] < 0 || j + d[0][k] >= w || i + d[1][k] < 0 || i + d[1][k] >= h) continue;
				if (count == 1 && depth == 2 && j == 1 && i == 1 && k == 2)
					printf("");
				swap(table[i + d[1][k]][j + d[0][k]], table[i][j]);
				if (py != i || px != j) {
					if (sel_lim == 0) {
						table = backup_table;
						continue;
					}
					sel_num++;
					ans.sellect(j + i * w, sel_rate);
				}
				ans.move(d[0][k], d[1][k], j + i * w, w, swap_rate);

				clear = idastar_minsel(depth - 1, count + 1, sel_lim - sel_num, table, ans, j + d[0][k], i + d[1][k]);
				if (clear) break;

				table = backup_table;
				ans = backup_ans;
			}
			if (clear) break;
		}
		if (clear) break;
	}
	return clear;
}

void idastar_ms_solve(vector<vector<int> > *table, status *ans) {
	int clear = 0;
	for (int depth = 0; !clear; depth++) {
		clear = idastar_minsel(depth, 0, sel_lim, *table, *ans, -1, -1);
		printf("%d\n", depth);
	}
	answer.show();
	answer.show_cost();
}