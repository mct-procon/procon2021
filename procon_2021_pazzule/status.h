//�񓚃f�[�^
using namespace std;
struct Status {
	vector<vector<unsigned _int8> > place; // �ꏊ�̈ʒu
	short int x = -1, y = -1; //�I���ꏊ
	unsigned char sel_cnt = 0;  //�I����(���C����)(�ő�128��)
	vector<unsigned char> sel_place;  //�I���摜�ʒu
	vector<int> swap_cnt;  //������
	vector<string> swap_operator; // ��������
	int eval_cost = 0; //�]���l
	int total_cost = 0;  //���R�X�g
	int sellect_cost = 0;  //�I���R�X�g

	//������
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

	//�I��
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


	//��������
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

	//�\��
	void show() {
		printf("�I����: %d\n", sel_cnt);
		for (int i = 0; i < sel_cnt; i++) {
			printf("�I���ʒu: %x\n", sel_place[i]);
			printf("������: %d\n", swap_cnt[i]);
			cout << "��������: " << swap_operator[i] << endl;
		}
	}

	void show_cost() {
		printf("���R�X�g  : %d\n", total_cost);
		printf("�I���R�X�g: %d\n", sellect_cost);
	}

	// �œK���̉��E
	// ���ׂẴ}�X�̃}���n�b�^�������̍��v / 2
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