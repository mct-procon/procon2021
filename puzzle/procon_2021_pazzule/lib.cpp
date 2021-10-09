#include <iostream>
#include <vector>
#include <time.h>
#include<assert.h>
#include<algorithm>
#include <fstream>
#include "status.h"
#include "lib.h"
using namespace std;

extern Status answer;
extern vector<vector<unsigned char> > complete;
extern vector<int> goal_place;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir, move_style;
extern vector<int> spin;

void input(vector<vector<int> > &table) {
	int target_i;

	//���͂��󂯎��
	cout << "�p�Y����@(0:�蓮   1:IDA*(2nd)   5:��M�����@8:ShortCut  9:��M����another)>";
	cin >> move_style;
	cout << "�ړ�����(0: �S���� 1:�E�Ɖ�)>";
	cin >> search_dir;
	cout << "�ڕW�摜(0: �w��   1: �����_��)>";
	cin >> target_i;

	cout << "�c�̕�����(2�`16)>";
	cin >> h;
	cout << "���̕�����(2�`16)>";
	cin >> w;
	cout << "�I���񐔐���(2�`128)>";
	cin >> sel_lim;
	cout << "�I���R�X�g�ϊ����[�g(1�`500)>";
	cin >> sel_rate;
	cout << "�����R�X�g�ϊ����[�g(1�`100)>";
	cin >> swap_rate;
	spin.resize(h * w, 0);

	table.resize(h);
	for (int i = 0; i < h; i++)
		table[i].resize(w);
	complete.resize(h);
	for (int i = 0; i < h; i++)
		complete[i].resize(w);

	switch (target_i) {
	case 0:
		cout << "�ڕW�摜>";
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int num;
				cin >> num;
				complete[y][x] = num;
			}
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
				printf("%d ", complete[y][x]);
			cout << endl;
		}
	}
	cout << "��]���(�Ȃ���� -1)>";
	for (int i = 0; i < h * w; i++) {
		cin >> spin[i];
		if (spin[i] == -1) {
			spin[i] = 0;
			break;
		}
	}

	//�}���n�b�^�������v�Z�p���W�̈ʒu
	goal_place.resize(h * w);
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			goal_place[complete[y][x]] = x + y * w;


	//�������ꂽ�摜�ɔԍ�������
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			table[i][j] = i * w + j;
}

void show_table(vector<vector<int> > table) {

	for (int s = 0; s < answer.sel_cnt; s++) {
		int sel_place = answer.sel_place[s];
		//cout << "�I���ʒu: " << sel_place << endl;
		for (int t = 0; t < answer.swap_cnt[s]; t++) {
			int past_place = sel_place;
			//cout << "�ړ�����: " << answer.swap_operator[s][t] << endl;
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
      if (s == answer.sel_cnt - 1 && t == answer.swap_cnt[s] - 1) // ���ʂ̔Ֆʂ����\��
      {
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

				//������\��
				for (int y = 0; y < h; y++) {
					for (int x = 0; x < w; x++) {
						int dist = 0;
						for (int yy = 0; yy < h; yy++) {
							for (int xx = 0; xx < w; xx++) {
								if (table[y][x] == complete[yy][xx])
									dist = (yy != y) + (xx != x);
							}
						}
						printf("%2x ", dist);
					}
					cout << endl;
				}
			}
		}
	}
}

//������
void status_init(Status &sta, int _x, int _y) {
	sta.place.resize(h);
	for (int i = 0; i < h; i++)
		sta.place[i].resize(w);
	sta.replace.resize(h * w);

	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++) {
			sta.place[i][j] = i * w + j;
			sta.replace[i * w + j] = i * w + j;
		}

	sta.x = _x; sta.y = _y;
	sta.array_size = h * w;
	status_calc_val(sta);
}

//�I��
void status_sellect(Status &sta, int _x, int _y) {
	sta.x = _x; sta.y = _y;
	sta.sel_cnt += 1;
	sta.sellect_cost += sel_rate;
	sta.total_cost += sel_rate;
	sta.sel_place.push_back(sta.x + sta.y * w);
  sta.swap_cnt.push_back(0);
  sta.swap_operator.push_back("");

	sta.compare_cost += sel_rate;
}

void status_unsellect(Status& sta, int pre_x, int pre_y) {
	sta.sel_cnt -= 1;
	sta.sellect_cost -= sel_rate;
	sta.total_cost -= sel_rate;
	sta.compare_cost -= sel_rate;
	sta.sel_place.pop_back();
	assert(sta.swap_cnt[sta.swap_cnt.size() - 1] == 0);
	sta.swap_cnt.pop_back();
	assert(sta.swap_operator[sta.sel_cnt] == "");
	sta.swap_operator.pop_back();
	sta.x = pre_x;
	sta.y = pre_y;
}


//��������
void status_move(Status& sta, int dx, int dy) {
	sta.total_cost += swap_rate;
	sta.swap_cnt[sta.swap_cnt.size() - 1] += 1;
	if (dy == -1) sta.swap_operator[sta.swap_operator.size() - 1] += "U";
	if (dx == -1) sta.swap_operator[sta.swap_operator.size() - 1] += "L";
	if (dy == 1) sta.swap_operator[sta.swap_operator.size() - 1] += "D";
	if (dx == 1) sta.swap_operator[sta.swap_operator.size() - 1] += "R";
	swap(sta.place[(sta.y + dy + h) % h][(sta.x + dx + w) % w], sta.place[sta.y][sta.x]);
	swap(sta.replace[sta.place[(sta.y + dy + h) % h][(sta.x + dx + w) % w]], sta.replace[sta.place[sta.y][sta.x]]);
	sta.x = (sta.x + dx + w) % w; sta.y = (sta.y + dy + h) % h;
	status_update_val(sta, dx, dy);

}


//�߂�
void status_unmove(Status& sta) {
	sta.total_cost -= swap_rate;	//status_update_val�Ŏg���̂Ő��
	
	int swap_op_last = sta.sel_cnt - 1;

	if(sta.last_swap_dir & 1){	//�E�������ɓ�������
		int dx = (1 - (sta.last_swap_dir & 2));
		int prev_x = (sta.x - dx + w) % w;
		swap(sta.place[sta.y][prev_x], sta.place[sta.y][sta.x]);
		swap(sta.replace[sta.place[sta.y][prev_x]], sta.replace[sta.place[sta.y][sta.x]]);
		sta.x = prev_x;
		status_update_val(sta, -dx, 0);
	}
	else {	//�ォ�����ɓ�������
		int dy = (1 - (sta.last_swap_dir & 2));
		int prev_y = (sta.y - dy + h) % h;
		swap(sta.place[prev_y][sta.x], sta.place[sta.y][sta.x]);
		swap(sta.replace[sta.place[prev_y][sta.x]], sta.replace[sta.place[sta.y][sta.x]]);
		sta.y = prev_y;
		status_update_val(sta, 0, -dy);
	}

	//sta.swap_operator[swap_op_last].erase(sta.swap_operator[swap_op_last].begin() + sta.swap_operator[swap_op_last].size() - 1);
	sta.swap_operator[swap_op_last].pop_back();
	sta.swap_cnt[sta.swap_cnt.size() - 1]--;
}

//�\��
void status_show(Status &sta) {
	printf("�I����: %d\n", sta.sel_cnt);
	for (int i = 0; i < sta.sel_cnt; i++) {
		printf("�I���ʒu: %x\n", sta.sel_place[i]);
		printf("������: %d\n", sta.swap_cnt[i]);
		cout << "��������: " << sta.swap_operator[i] << endl;
	}
}

void status_show_cost(Status &sta) {
	printf("���R�X�g  : %d\n", sta.total_cost);
	printf("�I���R�X�g: %d\n", sta.sellect_cost);
}

void status_sub_for() {
	ofstream outfile("submit.txt");
  //cout << "cost:" << answer.total_cost << endl;
	//cout << uppercase;
	outfile << uppercase;
	for (int i = 0; i < h * w; i++) {
		//cout << spin[i];
		outfile << spin[i];
	}
	//cout << endl;
	outfile << endl;
	//cout << (int)answer.sel_cnt << endl;
	outfile << (int)answer.sel_cnt << endl;
	for (int i = 0; i < answer.sel_cnt; i++) {
		//cout << hex << answer.sel_place[i] % w << answer.sel_place[i] / w << endl;
		outfile << hex << answer.sel_place[i] % w << answer.sel_place[i] / w << endl;
		//cout << dec << answer.swap_cnt[i] << endl;
		outfile << dec << answer.swap_cnt[i] << endl;

		//cout << answer.swap_operator[i] << endl;
		outfile << answer.swap_operator[i] << endl;
	}
	outfile.close();
}

// �œK���̉��E
// �e�}�X�̍ŏ��ړ������̍��v / 2
void status_calc_val(Status &sta) {
	int sum = 0;
	sta.dis_array.resize(sta.array_size);
	sta.dis_top1 = 0;
	sta.dis_top2 = 0;
	sta.dis_not_zero = 0;

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int goal_x = goal_place[sta.place[y][x]] % w;
			int goal_y = goal_place[sta.place[y][x]] / w;
			int dis_x = min(abs(goal_x - x), w - abs(goal_x - x));
			int dis_y = min(abs(goal_y - y), w - abs(goal_y - y));
			int dis = dis_x + dis_y;
			sum += dis;
			sta.dis_array[y * w + x] = dis;
			if (dis != 0)
				sta.dis_not_zero++;
		}
	}
	//for (int i = 0; i < sta.array_size; i++) {
		//int num = sta.dis_array[i];
	for(unsigned char num : sta.dis_array){
		if (chmax(sta.dis_top2, num)) {	//���傫�����̂ɍX�V������
			if (sta.dis_top1 < sta.dis_top2) {
				swap(sta.dis_top1, sta.dis_top2);
			}
		}
	}

	sta.total_dis = sum;
	sta.eval_cost = max({sum / 2 + (sum & 1), sta.dis_top1 + sta.dis_top2 - 1 });
	//sta.eval_cost = sum / 2 + (sum & 1);

	int eval2 = (sta.dis_not_zero - 1) * swap_rate;
	if (swap_rate > sel_rate)
		eval2 = (sta.dis_not_zero - 1) / 2 * (swap_rate + sel_rate);

	sta.compare_cost = sta.total_cost + max(sta.eval_cost * swap_rate, eval2);
	//sta.compare_cost = sta.total_cost + sta.eval_cost * swap_rate;
}

// �����ɂ��]���l�X�V
void status_update_val(Status& sta, int dx, int dy) {
	int prev_x = (sta.x - dx + w) % w, prev_y = (sta.y - dy + h) % h;	//�����Ă�s�[�X�̓���ւ���O�̏ꏊ

	//�����Ă�s�[�X��
	int goal_x = goal_place[sta.place[sta.y][sta.x]] % w, goal_y = goal_place[sta.place[sta.y][sta.x]] / w;	//�ړI�n
	//�]���l����
	int pas_x = min(abs(goal_x - prev_x), w - abs(goal_x - prev_x));
	int pas_y = min(abs(goal_y - prev_y), w - abs(goal_y - prev_y));
	int pas_dis = pas_x + pas_y;
	int new_x = min(abs(goal_x - sta.x), w - abs(goal_x - sta.x));
	int new_y = min(abs(goal_y - sta.y), w - abs(goal_y - sta.y));
	int new_dis = new_x + new_y;

	if (pas_dis == 0 && new_dis != 0)
		sta.dis_not_zero++;
	if (pas_dis != 0 && new_dis == 0)
		sta.dis_not_zero--;
	sta.dis_array[sta.y * w + sta.x] = new_dis;
	sta.total_dis += (new_dis - pas_dis);

	//�����ĂȂ��s�[�X��
	goal_x = goal_place[sta.place[prev_y][prev_x]] % w; goal_y = goal_place[sta.place[prev_y][prev_x]] / w;	//�ړI�n
	//�]���l����
	pas_x = min(abs(goal_x - sta.x), w - abs(goal_x - sta.x));
	pas_y = min(abs(goal_y - sta.y), w - abs(goal_y - sta.y));
	pas_dis = pas_x + pas_y;
	new_x = min(abs(goal_x - prev_x), w - abs(goal_x - prev_x));
	new_y = min(abs(goal_y - prev_y), w - abs(goal_y - prev_y));
	new_dis = new_x + new_y;
	
	if (pas_dis == 0 && new_dis != 0)
		sta.dis_not_zero++;
	if (pas_dis != 0 && new_dis == 0)
		sta.dis_not_zero--;
	sta.dis_array[prev_y * w + prev_x] = new_dis;
	sta.total_dis += (new_dis - pas_dis);

	sta.dis_top1 = 0;
	sta.dis_top2 = 0;

	for(int i = 0; i < sta.array_size; i++){
		unsigned char num = sta.dis_array[i];
	//for(int num : sta.dis_array){
		if (chmax(sta.dis_top2, num)) {	//���傫�����̂ɍX�V������
			if (sta.dis_top1 < sta.dis_top2)
				swap(sta.dis_top1, sta.dis_top2);
		}
	}

	sta.eval_cost = max({ sta.total_dis / 2 + (sta.total_dis & 1), sta.dis_top1 + sta.dis_top2 - 1});
	//sta.eval_cost = sta.total_dis / 2 + (sta.total_dis & 1);

	int eval2 = (sta.dis_not_zero - 1) * swap_rate;
	if (swap_rate > sel_rate)
		eval2 = (sta.dis_not_zero - 1) / 2 * (swap_rate + sel_rate);


	sta.compare_cost = sta.total_cost + max(sta.eval_cost * swap_rate, eval2);
	//sta.compare_cost = sta.total_cost + sta.eval_cost * swap_rate;
}

