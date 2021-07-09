#include <iostream>
#include <vector>
#include <time.h>
#include "status.h"
using namespace std;

extern Status answer;
extern vector<vector<unsigned char> > complete;
extern vector<int> goal_place;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir, move_style;

void input(vector<vector<int> > *table) {
	int target_i;

	//���͂��󂯎��
	cout << "�c�̕�����(2�`16)>";
	cin >> h;
	cout << "���̕�����(2�`16)>";
	cin >> w;
	cout << "�I���R�X�g�ϊ����[�g(1�`500)>";
	cin >> sel_rate;
	cout << "�����R�X�g�ϊ����[�g(1�`100)>";
	cin >> swap_rate;
	cout << "�I���񐔐���(2�`128)>";
	cin >> sel_lim;
	cout << "�p�Y����@(0:�蓮   1:IDA*   2:A*   3:�����ړ��@4:�[���̉�)>";
	cin >> move_style;
	cout << "�ړ�����(0: �S���� 1:�E�Ɖ�)>";
	cin >> search_dir;
	table->resize(h);
	for (int i = 0; i < h; i++)
		(*table)[i].resize(w);
	complete.resize(h);
	for (int i = 0; i < h; i++)
		complete[i].resize(w);

	cout << "�ڕW�摜(0: �w��   1: �����_��)>";
	cin >> target_i;
	switch (target_i) {
	case 0:
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++) {
				int num; cin >> num;
				complete[y][x] = num;
			}
		break;
	case 1:
		vector<int> remain(h * w, 1);

		srand((unsigned int)time(NULL));
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int r = rand()% (h * w);
				while (!remain[r]) r = (r + 1) % (h * w);
				complete[y][x] = r;
				remain[r] = 0;
			}
		}

		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++)
				printf("%2x ", complete[y][x]);
			cout << endl;
		}
	}
	//�}���n�b�^�������v�Z�p���W�̈ʒu
	goal_place.resize(h * w);
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			goal_place[complete[y][x]] = x + y * w;


	//�������ꂽ�摜�ɔԍ�������
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			(*table)[i][j] = i * w + j;
}

void show_table(vector<vector<int> > table) {

	for (int s = 0; s < answer.sel_cnt; s++) {
		int sel_place = answer.sel_place[s];
		cout << "�I���ʒu: " << sel_place << endl;
		for (int t = 0; t < answer.swap_cnt[s]; t++) {
			int past_place = sel_place;
			cout << "�ړ�����: " << answer.swap_operator[s][t] << endl;
			switch (answer.swap_operator[s][t])
			{
			case 'U':
				swap(table[sel_place / w][sel_place % w], table[(sel_place / w - 1 + h) % h][sel_place % w]);
				sel_place = ((sel_place / w - 1 + h) % h) * w + sel_place % w;
				break;
			case 'L':
				swap(table[sel_place / w][sel_place % w], table[sel_place / w][(sel_place % w - 1 + w) % w]);
				sel_place = sel_place / w * w + (sel_place % w - 1 + w) % w;
				break;
			case 'R':
				swap(table[sel_place / w][sel_place % w], table[sel_place / w][(sel_place % w + 1) % w]);
				sel_place = sel_place / w * w + (sel_place % w + 1) % w;
				break;
			case 'D':
				swap(table[sel_place / w][sel_place % w], table[(sel_place / w + 1) % h][sel_place % w]);
				sel_place = ((sel_place / w + 1) % h) * w + sel_place % w;
				break;
			}
			for (int y = 0; y < h; y++) {
				for (int x = 0; x < w; x++) {
					if (sel_place == x + y * w)
						printf("\033[31m%2x\033[m ", table[y][x]);
					else if (past_place == x + y * w)
						printf("\033[34m%2x\033[m ", table[y][x]);
					else
						printf("%2x ", table[y][x]);
				}
				cout << endl;
			}
		}
	}
}

