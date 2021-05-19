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
			int past_place = sel_place;
			cout << "ˆÚ“®•ûŒü: " << answer.swap_operator[s][t] << endl;
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
						cout << "\033[31m" << table[y][x] << "\033[m" << " ";
					else if (past_place == x + y * w)
						cout << "\033[34m" << table[y][x] << "\033[m" << " ";
					else
					    cout << table[y][x] << " ";
				}
				cout << endl;
			}
		}
	}
}

