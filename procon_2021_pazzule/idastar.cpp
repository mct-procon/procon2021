#include <iostream>
#include <vector>
#include "status.h"
using namespace std;

extern vector<vector<int> > complete;
extern status answer;
extern int h, w, sel_rate, swap_rate;

//IDA*‚É‚æ‚é‰æ‘œ•œŒ³’Tõ
int idastar(int depth, int count, vector<vector<int> > table, status ans, int px, int py) {
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
				if (j + d[0][k] < 0 || j + d[0][k] >= w || i + d[1][k] < 0 || i + d[1][k] >= h) continue;
				swap(table[i + d[1][k]][j + d[0][k]], table[i][j]);
				if (py != i || px != j) ans.sellect(j + i * w, sel_rate);
				ans.move(d[0][k], d[1][k], j + i * w, w, swap_rate);

				clear = idastar(depth - 1, count + 1, table, ans, j + d[0][k], i + d[1][k]);
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

//ˆ—‚Ì—¬‚ê(‚±‚±‚Å‚Í”½•œ[‰»‚Ì‚İ)
void idastar_solve(vector<vector<int> > *table, status *ans){
	int clear = 0;
	for (int depth = 0; !clear; depth++) {
		clear = idastar(depth, 0, *table, *ans, -1, -1);
		printf("%d\n", depth);
	}
	answer.show();
	answer.show_cost();
}