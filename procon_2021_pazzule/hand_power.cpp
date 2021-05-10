#include <iostream>
#include <vector>
#include <conio.h>
#include "answer.h"
using namespace std;
extern vector<vector<int> > complete;
extern int px, py, h, w, move_style, sel_or_swap, sel_rate, swap_rate, sel_lim;
extern answer clear_ans;
//画像の入れ替え処理
void move(vector<vector<int> > *table, int dx, int dy) {
	if (px + dx < 0 || px + dx >= (*table)[0].size() || py + dy < 0 || py + dy >= (*table).size()) return;

	if (sel_or_swap) {
		swap((*table)[py + dy][px + dx], (*table)[py][px]);
	}
    px += dx; py += dy;
}

//表示
void show(vector<vector<int> > *table) {
	system("cls");
	for (unsigned int i = 0; i < (*table).size(); i++) {
		for (unsigned int j = 0; j < (*table)[0].size(); j++)
			printf("%2x  ", (*table)[i][j]);

		if (py == i)
			cout << "←";
		cout << endl;
	}
	for (int i = 0; i < px; i++) cout << "    ";
	cout << "↑" << endl;
}


//手動のパズル
void hand_puzzle(vector<vector<int> >* table, answer* ans) {
	if (_kbhit()) {
		int pre_x = px, pre_y = py;
		int d[2][4] = {
			{0, -1, 0, 1},
			{-1, 0, 1, 0}
		};
		int v = -1;
		switch (_getch()) {
		case 'w':
			v = 0;
			break;
		case 'a':
			v = 1;
			break;
		case 's':
			v = 2;
			break;
		case 'd':
			v = 3;
			break;
		case 't':
			(*ans).spin(px + py * w);
			break;
		case 'g':
			if (!(sel_or_swap))
				(*ans).sellect(px + py * w, sel_rate);
			sel_or_swap ^= 1;
			break;
		}
		if (v != -1) {
			if (sel_or_swap) (*ans).move(d[0][v], d[1][v], px + py * w, w, swap_rate);
			move(table, d[0][v], d[1][v]);
		}
	}
}

//人力操作の処理の流れ
void hand_solve(vector<vector<int> > *table, answer* ans) {
	while (1) {
		if (*table == complete) {
			clear_ans = *ans;
			break;
		}
		show(table);
		(*ans).show();
		(*ans).show_cost();
		hand_puzzle(table, ans);
	}
}
