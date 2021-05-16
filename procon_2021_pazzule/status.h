//�񓚃f�[�^
using namespace std;
struct Status {
	vector<vector<int> > place; // �ꏊ�̈ʒu
	int x, y; //�I���ꏊ
	string spin_info;  //��]���
	int sel_cnt = 0;  //�I����(���C����)
	vector<int> sel_place;  //�I���摜�ʒu
	vector<int> swap_cnt;  //������
	vector<string> swap_operator; // ��������
	int eval_cost; //�]���l
	int total_cost = 0;  //���R�X�g
	int sellect_cost = 0;  //�I���R�X�g

	//������
	void init(int _x, int _y, int h, int w, vector<int> goal_place) {
		for (int i = 0; i < h * w; i++)
			spin_info += "0";
	    
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
		sel_place[sel_place.size() - 1] = x + y * place[0].size();
		swap_cnt.resize(swap_cnt.size() + 1);
		swap_cnt[swap_cnt.size() - 1] = 0;
		swap_operator.resize(swap_operator.size() + 1);
		swap_operator[swap_operator.size() - 1] = "";
	}


	//��������
	void move(int dx, int dy, int w, int swap_rate, vector<int> goal_place) {
		total_cost += swap_rate;
		swap_cnt[swap_cnt.size() - 1] += 1;
		if (dy == -1) swap_operator[swap_operator.size() - 1] += "U";
		if (dx == -1) swap_operator[swap_operator.size() - 1] += "L";
		if (dy == 1) swap_operator[swap_operator.size() - 1] += "D";
		if (dx == 1) swap_operator[swap_operator.size() - 1] += "R";
		swap(spin_info[x + y * w], spin_info[x + y * w + dx + dy * w]);
		swap(place[y + dy][x + dx], place[y][x]);
		x += dx; y += dy;
		calc_val(goal_place);
	}


	//��]����
	void spin(int place) {
		spin_info[place] = (spin_info[place] - '0' + 1) % 4 + '0';
	}

	//�\��
	void show() {
		cout << "��]���: " << spin_info << endl;
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