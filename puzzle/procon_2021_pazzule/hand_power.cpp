#include <iostream>
#include <vector>
#include <conio.h>
#include "status.h"
#include "lib.h"

using namespace std;
extern vector<vector<unsigned char> > complete;
extern vector<int> goal_place;
extern int px, py, h, w, move_style, sel_or_swap, sel_rate, swap_rate, sel_lim;
extern Status answer;
//�摜�̓���ւ�����
void move(Status &status, int dx, int dy) {
	if (sel_or_swap)
		status_move(status, dx, dy);

    px = (px + dx + w) % w; py = (py + dy + h) % h;
}

//�\��
void show(vector<vector<unsigned char> > &place) {
	system("cls");
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++)
			printf("%2x  ", place[y][x]);

		if (py == y)
			cout << "��";
		cout << endl;
	}
	for (int i = 0; i < px; i++) cout << "    ";
	cout << "��" << endl;
}


//�蓮�̃p�Y��
void hand_puzzle(Status &status) {
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
		case 'g':
			if (!(sel_or_swap))
				status_sellect(status, px, py);
			sel_or_swap ^= 1;
			break;
		}
		if (v != -1) {
			move(status, d[0][v], d[1][v]);
		}
	}
}

//�l�͑���̏����̗���
void hand_solve(Status &status) {
	status.x = 0; status.y = 0;
	while (1) {
		if (status.place == complete) {
			answer = status;
			break;
		}
		show(status.place);
		status_show(status);
		status_show_cost(status);
		hand_puzzle(status);
	}
}
