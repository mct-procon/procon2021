//�񓚃f�[�^
#define min(x, y) (x < y)? x : y
using namespace std;
struct Status {
	vector<vector<unsigned char> > place; // �ꏊ�̈ʒu
	short int x = -1, y = -1; //�I���ꏊ
	unsigned char sel_cnt = 0;  //�I����(���C����)(�ő�128��)
	vector<unsigned char> sel_place;  //�I���摜�ʒu
	vector<int> swap_cnt;  //������
	vector<string> swap_operator; // ��������
	int eval_cost = 0; //�]���l
	int total_cost = 0;  //���R�X�g
	int sellect_cost = 0;  //�I���R�X�g

	/*astar_2nd�p*/
	bool sellected = 0;
};