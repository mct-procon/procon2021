//�񓚃f�[�^
#define min(x, y) (x < y)? x : y
using namespace std;
struct Status {
	vector<vector<unsigned char> > place; // �ꏊ�̈ʒu
	vector<unsigned char> replace;	// �ꏊ�̈ʒu�t����
	short int x = -1, y = -1; //�I���ꏊ
	unsigned char sel_cnt = 0;  //�I����(���C����)(�ő�128��)
	vector<unsigned char> sel_place;  //�I���摜�ʒu
	vector<int> swap_cnt;  //������
	vector<string> swap_operator; // ��������
	int eval_cost = 0; //�ŒZswap��
	int total_cost = 0;  //���R�X�g
	int sellect_cost = 0;  //�I���R�X�g

	int total_dis = 0;
	vector<unsigned char> dis_array;
	int array_size = 0;
	unsigned char dis_top1, dis_top2;
	unsigned char dis_not_zero = 0;
	bool sellected = 0;
	int8_t last_swap_dir;	//{0123:DRUL}
	int compare_cost = 0;	//astar�p�̃R�X�g
};