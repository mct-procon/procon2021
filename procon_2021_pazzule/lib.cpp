#include <iostream>
#include <vector>
#include "status.h"
using namespace std;

extern Status answer;
extern vector<int> goal_place;
extern int h, w;

void show_table(vector<vector<int> > table) {

	for (int s = 0; s < answer.sel_cnt; s++) {
		int sel_place = answer.sel_place[s];
		cout << "‘I‘ðˆÊ’u: " << sel_place << endl;
		for (int t = 0; t < answer.swap_cnt[s]; t++) {
			cout << "ˆÚ“®•ûŒü: " << answer.swap_operator[s][t] << endl;
			switch (answer.swap_operator[s][t])
			{
			case 'U':
				swap(table[sel_place / w][sel_place % w], table[sel_place / w - 1][sel_place % w]);
				sel_place -= w;
				break;
			case 'L':
				swap(table[sel_place / w][sel_place % w], table[sel_place / w][sel_place % w - 1]);
				sel_place -= 1;
				break;
			case 'R':
				swap(table[sel_place / w][sel_place % w], table[sel_place / w][sel_place % w + 1]);
				sel_place += 1;
				break;
			case 'D':
				swap(table[sel_place / w][sel_place % w], table[sel_place / w + 1][sel_place % w]);
				sel_place += w;
				break;
			}
			for (int y = 0; y < h; y++) {
				for (int x = 0; x < w; x++) {
					cout << table[y][x] << " ";
				}
				cout << endl;
			}
		}
	}
}

