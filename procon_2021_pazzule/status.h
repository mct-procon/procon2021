//回答データ
using namespace std;
struct Status {
	vector<vector<unsigned _int8> > place; // 場所の位置
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
		calc_val(goal_place);
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

	// 最適解の下界
	// すべてのマスのマンハッタン距離の合計 / 2
	void calc_val(vector<int> goal_place) {
		int num = 0;
		for (int y = 0; y < place.size(); y++) {
			for (int x = 0; x < place[y].size(); x++) {
				num += abs(goal_place[place[y][x]] / (int)place[y].size() - y) + abs(goal_place[place[y][x]] % (int)place[y].size() - x);
			}
		}
		eval_cost = num / 2;
	}
};