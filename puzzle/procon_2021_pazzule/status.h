//回答データ
#define min(x, y) (x < y)? x : y
using namespace std;
struct Status {
	vector<vector<unsigned char> > place; // 場所の位置
	vector<unsigned char> replace;	// 場所の位置逆引き
	short int x = -1, y = -1; //選択場所
	unsigned char sel_cnt = 0;  //選択回数(ライン数)(最大128回)
	vector<unsigned char> sel_place;  //選択画像位置
	vector<int> swap_cnt;  //交換回数
	vector<string> swap_operator; // 交換操作
	int eval_cost = 0; //最短swap数
	int total_cost = 0;  //総コスト
	int sellect_cost = 0;  //選択コスト

	int total_dis = 0;
	vector<unsigned char> dis_array;
	int array_size = 0;
	unsigned char dis_top1, dis_top2;
	unsigned char dis_not_zero = 0;
	bool sellected = 0;
	int8_t last_swap_dir;	//{0123:DRUL}
	int compare_cost = 0;	//astar用のコスト
};