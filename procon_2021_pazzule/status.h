//回答データ
using namespace std;
typedef struct status {
	string spin_info;  //回転情報
	int sel_cnt = 0;  //選択回数(ライン数)
	vector<int> sel_place;  //選択画像位置
	vector<int> swap_cnt;  //交換回数
	vector<string> swap_operator; // 交換操作

	int total_cost = 0;  //総コスト
	int sellect_cost = 0;  //選択コスト

	//初期化
	void init(int h, int w) {
		for (int i = 0; i < h * w; i++)
			spin_info += "0";
	}

	//選択
	void sellect(int place, int sel_rate) {
		sel_cnt += 1;
		sellect_cost += sel_rate;
		total_cost += sel_rate;
		sel_place.resize(sel_place.size() + 1);
		sel_place[sel_place.size() - 1] = place;
		swap_cnt.resize(swap_cnt.size() + 1);
		swap_cnt[swap_cnt.size() - 1] = 0;
		swap_operator.resize(swap_operator.size() + 1);
		swap_operator[swap_operator.size() - 1] = "";
	}


	//交換操作
	void move(int dx, int dy, int place, int w, int swap_rate) {
		total_cost += swap_rate;
		swap_cnt[swap_cnt.size() - 1] += 1;
		if (dy == -1) swap_operator[swap_operator.size() - 1] += "U";
		if (dx == -1) swap_operator[swap_operator.size() - 1] += "L";
		if (dy == 1) swap_operator[swap_operator.size() - 1] += "D";
		if (dx == 1) swap_operator[swap_operator.size() - 1] += "R";
		swap(spin_info[place], spin_info[place + dx + dy * w]);
	}


	//回転操作
	void spin(int place) {
		spin_info[place] = (spin_info[place] - '0' + 1) % 4 + '0';
	}

	//表示
	void show() {
		cout << "回転情報: " << spin_info << endl;
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
} status;