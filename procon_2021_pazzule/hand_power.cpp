#include <iostream>
#include <vector>
#include <conio.h>
#include "status.h"
using namespace std;
extern vector<vector<int> > complete;
extern vector<int> goal_place;
extern int px, py, h, w, move_style, sel_or_swap, sel_rate, swap_rate, sel_lim;
extern Status answer;
//画像の入れ替え処理
void move(Status *status, int dx, int dy) {
	if (sel_or_swap)
		status->move(dx, dy, h, w, swap_rate, goal_place);

    px = (px + dx + w) % w; py = (py + dy + h) % h;
}

//表示
void show(vector<vector<int> > *place) {
	system("cls");
	for (unsigned int y = 0; y < place->size(); y++) {
		for (unsigned int x = 0; x < (*place)[0].size(); x++)
			printf("%2x  ", (*place)[y][x]);

		if (py == y)
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
				status->sellect(px, py, sel_rate);
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
	status->x = 0; status->y = 0;
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
