/*
* 第32回高専プロコン競技部門
* パズル
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
	cout << "縦の分割数(2～16)>";
	cin >> h;
	cout << "横の分割数(2～16)>";
	cin >> w;
	cout << "選択コスト変換レート(1～500)>";
	cin >> sel_rate;
	cout << "交換コスト変換レート(1～100)>";
	cin >> swap_rate;
	cout << "パズル手法(0: 手動 1: IDA* 2:選択回数制限付きIDA*)>";
	cin >> move_style;
	if (move_style == IDAS_MS) {
		cout << "選択回数制限>";
		cin >> sel_lim;
	}
	table.resize(h);
	for (int i = 0; i < h; i++)
		table[i].resize(w);
	complete.resize(h);
	for (int i = 0; i < h; i++)
		complete[i].resize(w);


	//分割された画像に番号をつける
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			table[i][j] = i * w + j;

	cout << "目標画像>\n";
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			cin >> complete[i][j];

	//回答用データ初期化
	ans.init(h, w);

	//パズル
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
