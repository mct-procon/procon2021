/*
* 第32回高専プロコン競技部門
* パズル
*/

#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include "status.h"
#include "lib.h"
#include "hand_power.h"
#include "idastar.h"
#include "idastar_divide.h"
#include "astar.h"
#include "force_bring.h"
#include "depth_block.h"
#include "one_wrote.h"
#include "one_wrote_eval.h"
#include "DFS_withOneWrote.h"
#include "shortcut.h"
#include "output_csv.h"
using namespace std;

vector<vector<unsigned char> > complete;
vector<int> goal_place;
int px, py, h, w, move_style, sel_or_swap = 0, sel_rate, swap_rate, sel_lim, search_dir;
Status answer;
vector<int> spin;

int main() {
	vector<vector<int> > table;
    Status init_status;

	//入力
	input(table);

	//回答用データ初期化
	status_init(init_status, -1, -1);

	//パズル
	if (complete != init_status.place) {
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
		case OW:
			ow_solve(init_status);
			break;
		case IDASD:
			idastar_D_solve(init_status);
			break;
		case DFS_OW:
			dfs_solve(init_status);
			break;
		case SHORTCUT:
			shortcut_solve(init_status);
			break;
		}
	}

	cout << endl;
	show_table(table);
	cout << endl;
	status_sub_for();
	return 0;
}
