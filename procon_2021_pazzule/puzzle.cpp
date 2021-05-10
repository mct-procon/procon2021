/*
* ��32�񍂐�v���R�����Z����
* �p�Y��
*/

#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include "answer.h"
#include "lib.h"
#include "hand_power.h"
#include "idastar.h"
#include "idastar_minsel.h"
#define HAND 0
#define IDAS 1
#define IDAS_MS 2
using namespace std;

vector<vector<int> > complete;
int px, py, h, w, move_style, sel_or_swap = 0, sel_rate, swap_rate, sel_lim;
answer clear_ans;
int main() {
	vector<vector<int> > table;
    answer ans;
	cout << "�c�̕�����(2�`16)>";
	cin >> h;
	cout << "���̕�����(2�`16)>";
	cin >> w;
	cout << "�I���R�X�g�ϊ����[�g(1�`500)>";
	cin >> sel_rate;
	cout << "�����R�X�g�ϊ����[�g(1�`100)>";
	cin >> swap_rate;
	cout << "�p�Y����@(0: �蓮 1: IDA* 2:�I���񐔐����t��IDA*)>";
	cin >> move_style;
	if (move_style == IDAS_MS) {
		cout << "�I���񐔐���>";
		cin >> sel_lim;
	}
	table.resize(h);
	for (int i = 0; i < h; i++)
		table[i].resize(w);
	complete.resize(h);
	for (int i = 0; i < h; i++)
		complete[i].resize(w);


	//�������ꂽ�摜�ɔԍ�������
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			table[i][j] = i * w + j;

	cout << "�ڕW�摜>\n";
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			cin >> complete[i][j];

	//�񓚗p�f�[�^������
	ans.init(h, w);

	//�p�Y��
	int clear = 0;
	switch (move_style) {
	case HAND:
		hand_solve(&table, &ans);
		break;
	case IDAS:
		idastar_solve(&table, &ans);
		break;
	case IDAS_MS:
		idastar_ms_solve(&table, &ans);
		break;
	}
	show_table(table);
	return 0;
}
