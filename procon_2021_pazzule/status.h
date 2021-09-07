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

	/*astar_2nd用*/
	bool sellected = 0;
};