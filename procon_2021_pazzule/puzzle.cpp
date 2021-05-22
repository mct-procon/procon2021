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
#include "idastar_minsel.h"
#include "astar.h"
#define HAND 0
#define IDAS 1
#define IDAS_MS 2
#define ASTAR 3
using namespace std;

vector<vector<int> > complete;
vector<int> goal_place;
int px, py, h, w, move_style, sel_or_swap = 0, sel_rate, swap_rate, sel_lim;
Status answer;
int main() {
	vector<vector<int> > table;
    Status init_status;
	cout << "縦の分割数(2～16)>";
	cin >> h;
	cout << "横の分割数(2～16)>";
	cin >> w;
	cout << "選択コスト変換レート(1～500)>";
	cin >> sel_rate;
	cout << "交換コスト変換レート(1～100)>";
	cin >> swap_rate;
	cout << "パズル手法(0:手動   1:IDA*   2:選択回数制限付きIDA*   3:A*)>";
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

	//マンハッタン距離計算用座標の位置
	goal_place.resize(h * w);
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			goal_place[complete[y][x]] = x + y * w;

	//回答用データ初期化
	init_status.init(-1, -1, h, w, goal_place);

	//パズル
	int clear = 0;
	switch (move_style) {
	case HAND:
		hand_solve(&init_status);
		break;
	case IDAS:
		idastar_solve(&init_status);
		break;
	case IDAS_MS:
		idastar_ms_solve(&init_status);
		break;
	case ASTAR:
		astar_solve(&init_status);
		break;
	}
	cout << endl;
	show_table(table);
	return 0;
}
