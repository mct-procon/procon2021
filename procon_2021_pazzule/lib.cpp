#include <iostream>
#include <vector>
#include <time.h>
#include "status.h"
#include "lib.h"
using namespace std;

extern Status answer;
extern vector<vector<unsigned char> > complete;
extern vector<int> goal_place;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir, move_style;

void input(vector<vector<int> > &table) {
	int target_i;

	//入力を受け取る
	cout << "縦の分割数(2～16)>";
	cin >> h;
	cout << "横の分割数(2～16)>";
	cin >> w;
	cout << "選択コスト変換レート(1～500)>";
	cin >> sel_rate;
	cout << "交換コスト変換レート(1～100)>";
	cin >> swap_rate;
	cout << "選択回数制限(2～128)>";
	cin >> sel_lim;
	cout << "パズル手法(0:手動   1:IDA*   2:A*   3:強制移動　4:深さの塊)>";
	cin >> move_style;
	cout << "移動方向(0: 全方向 1:右と下)>";
	cin >> search_dir;
	table.resize(h);
	for (int i = 0; i < h; i++)
		table[i].resize(w);
	complete.resize(h);
	for (int i = 0; i < h; i++)
		complete[i].resize(w);

	cout << "目標画像(0: 指定   1: ランダム)>";
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
	//マンハッタン距離計算用座標の位置
	goal_place.resize(h * w);
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			goal_place[complete[y][x]] = x + y * w;


	//分割された画像に番号をつける
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			table[i][j] = i * w + j;
}

void show_table(vector<vector<int> > table) {

	for (int s = 0; s < answer.sel_cnt; s++) {
		int sel_place = answer.sel_place[s];
		cout << "選択位置: " << sel_place << endl;
		for (int t = 0; t < answer.swap_cnt[s]; t++) {
			int past_place = sel_place;
			cout << "移動方向: " << answer.swap_operator[s][t] << endl;
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

//初期化
void status_init(Status &sta, int _x, int _y) {
	sta.place.resize(h);
	for (int i = 0; i < h; i++)
		sta.place[i].resize(w);

	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			sta.place[i][j] = i * w + j;

	sta.x = _x; sta.y = _y;
	status_calc_val(sta);
}

//選択
void status_sellect(Status &sta, int _x, int _y) {
	sta.x = _x; sta.y = _y;
	sta.sel_cnt += 1;
	sta.sellect_cost += sel_rate;
	sta.total_cost += sel_rate;
	sta.sel_place.resize(sta.sel_place.size() + 1);
	sta.sel_place[sta.sel_place.size() - 1] = sta.x + sta.y * w;
	sta.swap_cnt.resize(sta.swap_cnt.size() + 1);
	sta.swap_cnt[sta.swap_cnt.size() - 1] = 0;
	sta.swap_operator.resize(sta.swap_operator.size() + 1);
	sta.swap_operator[sta.swap_operator.size() - 1] = "";
}


//交換操作
void status_move(Status& sta, int dx, int dy) {
	sta.total_cost += swap_rate;
	sta.swap_cnt[sta.swap_cnt.size() - 1] += 1;
	if (dy == -1) sta.swap_operator[sta.swap_operator.size() - 1] += "U";
	if (dx == -1) sta.swap_operator[sta.swap_operator.size() - 1] += "L";
	if (dy == 1) sta.swap_operator[sta.swap_operator.size() - 1] += "D";
	if (dx == 1) sta.swap_operator[sta.swap_operator.size() - 1] += "R";
	swap(sta.place[(sta.y + dy + h) % h][(sta.x + dx + w) % w], sta.place[sta.y][sta.x]);
	sta.x = (sta.x + dx + w) % w; sta.y = (sta.y + dy + h) % h;
	status_update_val(sta, dx, dy);
}

//戻す
void status_unmove(Status& sta, int pre_x, int pre_y) {
	switch (sta.swap_operator[sta.swap_operator.size() - 1][sta.swap_operator[sta.swap_operator.size() - 1].size() - 1]) {
	case 'U':
		swap(sta.place[(sta.y + 1 + h) % h][(sta.x + w) % w], sta.place[sta.y][sta.x]);
		sta.y = (sta.y + 1 + h) % h;
		status_update_val(sta, 0, 1);
		break;
	case 'L':
		swap(sta.place[(sta.y + h) % h][(sta.x + 1 + w) % w], sta.place[sta.y][sta.x]);
		sta.x = (sta.x + 1 + w) % w;
		status_update_val(sta, 1, 0);
		break;
	case 'D':
		swap(sta.place[(sta.y - 1 + h) % h][(sta.x + w) % w], sta.place[sta.y][sta.x]);
		sta.y = (sta.y - 1 + h) % h;
		status_update_val(sta, 0, -1);
		break;
	case 'R':
		swap(sta.place[(sta.y + h) % h][(sta.x - 1 + w) % w], sta.place[sta.y][sta.x]);
		sta.x = (sta.x - 1 + w) % w;
		status_update_val(sta, -1, 0);
		break;
	}
	sta.swap_operator[sta.swap_operator.size() - 1].erase(sta.swap_operator[sta.swap_operator.size() - 1].begin() + sta.swap_operator[sta.swap_operator.size() - 1].size() - 1);
	sta.total_cost -= swap_rate;
	sta.swap_cnt[sta.swap_cnt.size() - 1] -= 1;
	if (pre_x != sta.x || pre_y != sta.y) {
		sta.x = pre_x; sta.y = pre_y;
		sta.sel_cnt -= 1;
		sta.sellect_cost -= sel_rate;
		sta.total_cost -= sel_rate;
		sta.sel_place.erase(sta.sel_place.begin() + sta.sel_place.size() - 1);
		sta.swap_cnt.erase(sta.swap_cnt.begin() + sta.swap_cnt.size() - 1);
		sta.swap_operator.erase(sta.swap_operator.begin() + sta.swap_operator.size() - 1);
	}
}

//表示
void status_show(Status &sta) {
	printf("選択回数: %d\n", sta.sel_cnt);
	for (int i = 0; i < sta.sel_cnt; i++) {
		printf("選択位置: %x\n", sta.sel_place[i]);
		printf("交換回数: %d\n", sta.swap_cnt[i]);
		cout << "交換操作: " << sta.swap_operator[i] << endl;
	}
}

void status_show_cost(Status &sta) {
	printf("総コスト  : %d\n", sta.total_cost);
	printf("選択コスト: %d\n", sta.sellect_cost);
}

void status_sub_for(Status &sta) {
	printf("%d\n", sta.sel_cnt);
	for (int i = 0; i < sta.sel_cnt; i++) {
		printf("%x%x\n", sta.sel_place[i] % w, sta.sel_place[i] / w);
		printf("%d\n", sta.swap_cnt[i]);
		cout << sta.swap_operator[i] << endl;
	}
}

// 最適解の下界
// 各マスの最小移動距離の合計 / 2
void status_calc_val(Status &sta) {
	int sum = 0;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int dis_x = min(abs(goal_place[sta.place[y][x]] % w - x), w - abs(goal_place[sta.place[y][x]] % w - x));
			int dis_y = min(abs(goal_place[sta.place[y][x]] / w - y), w - abs(goal_place[sta.place[y][x]] / w - y));
			sum += dis_x + dis_y;
		}
	}
	sta.eval_cost = sum;
}

// 差分により評価値更新
void status_update_val(Status& sta, int dx, int dy) {
	int pas_x = min(abs(goal_place[sta.place[sta.y][sta.x]] % w - (sta.x - dx + w) % w), w - abs(goal_place[sta.place[sta.y][sta.x]] % w - (sta.x - dx + w) % w));
	int pas_y = min(abs(goal_place[sta.place[sta.y][sta.x]] / w - (sta.y - dy + h) % h), w - abs(goal_place[sta.place[sta.y][sta.x]] / w - (sta.y - dy + h) % h));
	int pas_dis = pas_x + pas_y;
	int new_x = min(abs(goal_place[sta.place[sta.y][sta.x]] % w - sta.x), w - abs(goal_place[sta.place[sta.y][sta.x]] % w - sta.x));
	int new_y = min(abs(goal_place[sta.place[sta.y][sta.x]] / w - sta.y), w - abs(goal_place[sta.place[sta.y][sta.x]] / w - sta.y));
	int new_dis = new_x + new_y;
	sta.eval_cost += (new_dis - pas_dis);
	pas_x = min(abs(goal_place[sta.place[(sta.y - dy + h) % h][(sta.x - dx + w) % w]] % w - sta.x), w - abs(goal_place[sta.place[(sta.y - dy + h) % h][(sta.x - dx + w) % w]] % w - sta.x));
	pas_y = min(abs(goal_place[sta.place[(sta.y - dy + h) % h][(sta.x - dx + w) % w]] / w - sta.y), w - abs(goal_place[sta.place[(sta.y - dy + h) % h][(sta.x - dx + w) % w]] / w - sta.y));
	pas_dis = pas_x + pas_y;
	new_x = min(abs(goal_place[sta.place[(sta.y - dy + h) % h][(sta.x - dx + w) % w]] % w - (sta.x - dx + w) % w), w - abs(goal_place[sta.place[(sta.y - dy + h) % h][(sta.x - dx + w) % w]] % w - (sta.x - dx + w) % w));
	new_y = min(abs(goal_place[sta.place[(sta.y - dy + h) % h][(sta.x - dx + w) % w]] / w - (sta.y - dy + h) % h), w - abs(goal_place[sta.place[(sta.y - dy + h) % h][(sta.x - dx + w) % w]] / w - (sta.y - dy + h) % h));
	new_dis = new_x + new_y;
	sta.eval_cost += (new_dis - pas_dis);
}
