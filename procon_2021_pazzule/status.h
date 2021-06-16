//回答データ
#define min(x, y) (x < y)? x : y
using namespace std;
struct Status {
	vector<vector<unsigned char> > place; // 場所の位置
	short int x = -1, y = -1; //選択場所
	unsigned char sel_cnt = 0;  //選択回数(ライン数)(最大128回)
	vector<unsigned char> sel_place;  //選択画像位置
	vector<int> swap_cnt;  //交換回数
	vector<string> swap_operator; // 交換操作
	int eval_cost = 0; //評価値
	int total_cost = 0;  //総コスト
	int sellect_cost = 0;  //選択コスト

	//初期化
	void init(int _x, int _y, int h, int w, vector<int> goal_place) {
		place.resize(h);
		for (int i = 0; i < h; i++)
			place[i].resize(w);

		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
				place[i][j] = i * w + j;

		x = _x; y = _y;
		calc_val(goal_place);
	}

	//選択
	void sellect(int _x, int _y, int sel_rate) {
		x = _x; y = _y;
		sel_cnt += 1;
		sellect_cost += sel_rate;
		total_cost += sel_rate;
		sel_place.resize(sel_place.size() + 1);
		sel_place[sel_place.size() - 1] = x + y * (int)place[0].size();
		swap_cnt.resize(swap_cnt.size() + 1);
		swap_cnt[swap_cnt.size() - 1] = 0;
		swap_operator.resize(swap_operator.size() + 1);
		swap_operator[swap_operator.size() - 1] = "";
	}


	//交換操作
	void move(int dx, int dy, int h, int w, int swap_rate, vector<int> goal_place) {
		total_cost += swap_rate;
		swap_cnt[swap_cnt.size() - 1] += 1;
		if (dy == -1) swap_operator[swap_operator.size() - 1] += "U";
		if (dx == -1) swap_operator[swap_operator.size() - 1] += "L";
		if (dy == 1) swap_operator[swap_operator.size() - 1] += "D";
		if (dx == 1) swap_operator[swap_operator.size() - 1] += "R";
		swap(place[(y + dy + h) % h][(x + dx + w) % w], place[y][x]);
		x = (x + dx + w) % w; y = (y + dy + h) % h;
		update_val(goal_place, dx, dy, h, w);
	}

	//表示
	void show() {
		printf("選択回数: %d\n", sel_cnt);
		for (int i = 0; i < sel_cnt; i++) {
			printf("選択位置: %x\n", sel_place[i]);
			printf("交換回数: %d\n", swap_cnt[i]);
			cout << "交換操作: " << swap_operator[i] << endl;
		}
	}

	void show_cost() {
		printf("総コスト  : %d\n", total_cost);
		printf("選択コスト: %d\n", sellect_cost);
	}

	void sub_for(int w) {
		printf("%d\n", sel_cnt);
		for (int i = 0; i < sel_cnt; i++) {
			printf("%x%x\n", sel_place[i] % w, sel_place[i] / w);
			printf("%d\n", swap_cnt[i]);
			cout << swap_operator[i] << endl;
		}
	}

	// 最適解の下界
	// 各マスの最小移動距離の合計 / 2
	void calc_val(vector<int> goal_place) {
		int sum = 0;
		for (int y = 0; y < place.size(); y++) {
			for (int x = 0; x < place[y].size(); x++) {
				int dis_x = min(abs(goal_place[place[y][x]] % (int)place[y].size() - x), (int)place[y].size() - abs(goal_place[place[y][x]] % (int)place[y].size() - x));
				int dis_y = min(abs(goal_place[place[y][x]] / (int)place[y].size() - y), (int)place[y].size() - abs(goal_place[place[y][x]] / (int)place[y].size() - y));
				sum += dis_x + dis_y;
			}
		}
		eval_cost = sum * 5;
	}

	// 差分により評価値更新
	void update_val(vector<int> goal_place, int dx, int dy, int h, int w) {
		int pas_x = min(abs(goal_place[place[y][x]] % (int)place[y].size() - (x - dx + w) % w), (int)place[y].size() - abs(goal_place[place[y][x]] % (int)place[(y - dy + h) % h].size() - (x - dx + w) % w));
		int pas_y = min(abs(goal_place[place[y][x]] / (int)place[y].size() - (y - dy + h) % h), (int)place[y].size() - abs(goal_place[place[y][x]] / (int)place[y].size() - (y - dy + h) % h));
		int pas_dis = pas_x + pas_y;
		int new_x = min(abs(goal_place[place[y][x]] % (int)place[y].size() - x), (int)place[y].size() - abs(goal_place[place[y][x]] % (int)place[y].size() - x));
		int new_y = min(abs(goal_place[place[y][x]] / (int)place[y].size() - y), (int)place[y].size() - abs(goal_place[place[y][x]] / (int)place[y].size() - y));
		int new_dis = new_x + new_y;
		eval_cost += (new_dis - pas_dis) * 5;
		pas_x = min(abs(goal_place[place[(y - dy + h) % h][(x - dx + w) % w]] % (int)place[y].size() - x), (int)place[y].size() - abs(goal_place[place[(y - dy + h) % h][(x - dx + w) % w]] % (int)place[y].size() - x));
		pas_y = min(abs(goal_place[place[(y - dy + h) % h][(x - dx + w) % w]] / (int)place[y].size() - y), (int)place[y].size() - abs(goal_place[place[(y - dy + h) % h][(x - dx + w) % w]] / (int)place[y].size() - y));
		pas_dis = pas_x + pas_y;
		new_x = min(abs(goal_place[place[(y - dy + h) % h][(x - dx + w) % w]] % (int)place[y].size() - (x - dx + w) % w), (int)place[y].size() - abs(goal_place[place[(y - dy + h) % h][(x - dx + w) % w]] % (int)place[y].size() - (x - dx + w) % w));
		new_y = min(abs(goal_place[place[(y - dy + h) % h][(x - dx + w) % w]] / (int)place[y].size() - (y - dy + h) % h), (int)place[y].size() - abs(goal_place[place[(y - dy + h) % h][(x - dx + w) % w]] / (int)place[y].size() - (y - dy + h) % h));
		new_dis = new_x + new_y;
		eval_cost += (new_dis - pas_dis) * 5;
	}
};