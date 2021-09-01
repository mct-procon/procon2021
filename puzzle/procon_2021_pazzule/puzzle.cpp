/*
* ��32�񍂐�v���R�����Z����
* �p�Y��
*/

#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include "status.h"
#include "lib.h"
#include "hand_power.h"
#include "idastar.h"
#include "astar.h"
#include "force_bring.h"
#include "depth_block.h"
#include "one_wrote.h"
using namespace std;

vector<vector<unsigned char> > complete;
vector<int> goal_place;
int px, py, h, w, move_style, sel_or_swap = 0, sel_rate, swap_rate, sel_lim, search_dir;
Status answer;

int main() {
	vector<vector<int> > table;
    Status init_status;

	//����
	input(table);

	//�񓚗p�f�[�^������
	status_init(init_status, -1, -1);

	//�p�Y��
	int clear = 0;
	switch (move_style) {
	case HAND:
		hand_solve(init_status);
		break;
	case IDAS:
		idastar_solve(init_status);
		break;
	case ASTAR:
		astar_solve(init_status);
		break;
	case FB:
		fb_solve(init_status);
		break;
	case DB:
		db_solve(init_status);
		break;
	}
	cout << endl;
	show_table(table);
	cout << endl;
	status_sub_for();
	return 0;
}
