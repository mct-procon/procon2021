#include <iostream>
#include <vector>
#include <conio.h>
#include "status.h"
using namespace std;
extern vector<vector<int> > complete;
extern int px, py, h, w, move_style, sel_or_swap, sel_rate, swap_rate, sel_lim;
extern Status answer;
//画像の入れ替え処理
void move(Status *status, int dx, int dy) {
	if (px + dx < 0 || px + dx >= (status->place)[0].size() || py + dy < 0 || py + dy >= (status->place).size()) return;

	if (sel_or_swap)
		status->move(dx, dy, px, py, w, swap_rate);

    px += dx; py += dy;
}

//表示
void show(vector<vector<int> > *place) {
	system("cls");
	for (unsigned int i = 0; i < place->size(); i++) {
		for (unsigned int j = 0; j < (*place)[0].size(); j++)
			printf("%2x  ", (*place)[i][j]);

		if (py == i)
			cout << "←";
		cout << endl;
	}
	for (int i = 0; i < px; i++) cout << "    ";
	cout << "↑" << endl;
}


//手動のパズル
void hand_puzzle(Status* status) {
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
			status->spin(px + py * w);
			break;
		case 'g':
			if (!(sel_or_swap))
				status->sellect(px + py * w, sel_rate);
			sel_or_swap ^= 1;
			break;
		}
		if (v != -1) {
			move(status, d[0][v], d[1][v]);
		}
	}
}

//人力操作の処理の流れ
void hand_solve(Status* status) {
	while (1) {
		if (status->place == complete) {
			answer = *status;
			break;
		}
		show(&status->place);
		status->show();
		status->show_cost();
		hand_puzzle(status);
	}
}
