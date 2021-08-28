#define _CRT_SECURE_NO_WARNINGS
#include <Siv3D.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <queue>

using namespace std;
int PSIZE = 20;

struct picture {
	int div_x;
	int div_y;
	int select_lim;
	int select_cost;
	int swap_cost;
	int size_x;
	int size_y;
	int color_max;
	vector<vector<vector<unsigned char>>> pix;
};

struct piece {
	int piece_id;
	vector<vector<vector<unsigned char>>> edge;
	DynamicTexture tex;
};

picture inputpic() {
	picture res;
	FILE* fp = fopen("pictures/NGC_2336.ppm", "rb");
	if (fp == NULL) {
		//読み込みエラー
		assert(0);
	}
	char magic_number[10];
	fscanf(fp, "%s\n", magic_number);
	if (strcmp(magic_number, "P6") != 0) {
		//フォーマットがP6でない
		fclose(fp);
		assert(0);
	}
	fscanf(fp, "# %d %d\n", &res.div_x, &res.div_y);
	fscanf(fp, "# %d\n", &res.select_lim);
	fscanf(fp, "# %d %d\n", &res.select_cost, &res.swap_cost);
	fscanf(fp, "%d %d\n", &res.size_x, &res.size_y);
	fscanf(fp, "%d\n", &res.color_max);
	res.pix = vector<vector<vector<unsigned char>>>(res.size_y, vector<vector<unsigned char>>(res.size_x, vector<unsigned char>(3)));
	for (int i = 0; i < res.size_y; i++) {
		for (int j = 0; j < res.size_x; j++) {
			for (int k = 0; k < 3; k++) {
				fread(&res.pix[i][j][k], 1, 1, fp);
			}
		}
	}
	fclose(fp);
	if (res.size_x / res.div_x != res.size_y / res.div_y) {
		//ピースが正方形でない
		assert(0);
	}
	return res;
}

vector<piece> picture_2_pieces(const picture& pic) {
	const int piece_num = pic.div_x * pic.div_y;
	const int piece_size = pic.size_x / pic.div_x;
	Image init_img(piece_size, piece_size, Palette::White);
	vector<piece> res(piece_num, piece{ 0,vector<vector<vector<unsigned char>>>(0),DynamicTexture(init_img)});
	for (int i = 0; i < piece_num; i++) {
		res[i].piece_id = i;
	}
	int idx;
	//上辺
	idx = 0;
	for (int i = 0; i < pic.size_y; i+= piece_size) {
		for (int j = 0; j < pic.size_x; j+= piece_size) {
			vector<vector<unsigned char>> tmp;
			for (int k = 0; k < piece_size; k++) {
				tmp.emplace_back(pic.pix[i][j + k]);
			}
			res[idx++].edge.emplace_back(tmp);
		}
	}
	//右辺
	idx = 0;
	for (int i = 0; i < pic.size_y; i += piece_size) {
		for (int j = piece_size - 1; j < pic.size_x; j += piece_size) {
			vector<vector<unsigned char>> tmp;
			for (int k = 0; k < piece_size; k++) {
				tmp.emplace_back(pic.pix[i + k][j]);
			}
			res[idx++].edge.emplace_back(tmp);
		}
	}
	//下辺
	idx = 0;
	for (int i = piece_size - 1; i < pic.size_y; i += piece_size) {
		for (int j = piece_size - 1; j < pic.size_x; j += piece_size) {
			vector<vector<unsigned char>> tmp;
			for (int k = 0; k < piece_size; k++) {
				tmp.emplace_back(pic.pix[i][j - k]);
			}
			res[idx++].edge.emplace_back(tmp);
		}
	}
	//左辺
	idx = 0;
	for (int i = piece_size - 1; i < pic.size_y; i += piece_size) {
		for (int j = 0; j < pic.size_x; j += piece_size) {
			vector<vector<unsigned char>> tmp;
			for (int k = 0; k < piece_size; k++) {
				tmp.emplace_back(pic.pix[i - k][j]);
			}
			res[idx++].edge.emplace_back(tmp);
		}
	}
	//テクスチャ
	for (int piece_idx = 0; piece_idx < piece_num; piece_idx++) {
		Image pieceimg(piece_size, piece_size, Palette::White);
		int origin_x = (piece_idx % pic.div_x) * piece_size;
		int origin_y = (piece_idx / pic.div_x) * piece_size;
		for (int y = 0; y < piece_size; y++) {
			for (int x = 0; x < piece_size; x++) {
				pieceimg[y][x] = Color(pic.pix[origin_y + y][origin_x + x][0], pic.pix[origin_y + y][origin_x + x][1], pic.pix[origin_y + y][origin_x + x][2]);
			}
		}
		DynamicTexture piecetex(pieceimg);
		res[piece_idx].tex = piecetex;
	}
	return res;
}

/*
Image piece_2_image(piece p, int piece_size) {
	Image pieceimg(piece_size, piece_size, Palette::White);
	for (int i = 0; i < piece_size; i++) {
		pieceimg[0][i] = Color(p.edge[0][i][0], p.edge[0][i][1], p.edge[0][i][2]);
	}
	for (int i = 0; i < piece_size; i++) {
		pieceimg[i][piece_size - 1] = Color(p.edge[1][i][0], p.edge[1][i][1], p.edge[1][i][2]);
	}
	for (int i = 0; i < piece_size; i++) {
		pieceimg[piece_size - 1][piece_size - 1 - i] = Color(p.edge[2][i][0], p.edge[2][i][1], p.edge[2][i][2]);
	}
	for (int i = 0; i < piece_size; i++) {
		pieceimg[piece_size - 1 - i][0] = Color(p.edge[3][i][0], p.edge[3][i][1], p.edge[3][i][2]);
	}
	return pieceimg;
}
*/

double calc_difference_score(vector<vector<unsigned char>> edge_a, vector<vector<unsigned char>> edge_b, int edge_width) {
	double sum = 0;
	for (int i = 0; i < edge_width; i++) {
		for (int j = 0; j < 3; j++) {
			sum += abs(edge_a[i][j] - edge_b[edge_width-1-i][j]);
		}
	}
	return sum;
}

struct state { //ビームサーチで持つ状態
	vector<vector<int>> board; //その場所に何番のピースがあるか
	vector<vector<int>> direction; //向き
	vector<int> used; //使用済みピース (使用済みなら1)
	int adjacent_num; //ピースが接する部分の数
	double difference_score_sum; //ピースが接する部分の差異を数値化したものの総和
	bool operator<(const state& another) const { //比較
		if (adjacent_num == 0 || another.adjacent_num == 0)
			return 0;
		else
			return -(difference_score_sum / pow(adjacent_num,1.5)) > -(another.difference_score_sum / pow(another.adjacent_num,1.5));
	};
};

state beam_search(const vector<piece>& pieces, const int& div_x, const int& div_y, const int& piece_size) {
	const int beam_width = 5; //ビーム幅
	const int piece_num = div_x * div_y;
	const int board_size_x = 4 * div_x;
	const int board_size_y = 4 * div_y;
	const int dy[4] = { -1,0,1,0 }, dx[4] = { 0,1,0,-1 };
	
	state init = { vector<vector<int>>(board_size_y,vector<int>(board_size_x,-1)),vector<vector<int>>(board_size_y,vector<int>(board_size_x,0)),vector<int>(piece_num,0),0,0 };
	init.board[2*div_y][2*div_x] = 0;
	init.used[0] = 1;
	set<state> que;
	que.insert(init);
	for (int setup_piece_num = 1; setup_piece_num < piece_num; setup_piece_num++) {
		for (int y = 1; y < board_size_y - 1; y++) {
			for (int x = 1; x < board_size_x - 1; x++) {
				if ((*que.begin()).board[y][x] == -1)
					continue;
				pieces[(*que.begin()).board[y][x]].tex.resized(PSIZE).rotated(90_deg*(*que.begin()).direction[y][x]).draw(x* PSIZE, y* PSIZE);
			}
		}
		//Print << que.top().difference_score_sum;
		System::Update();
		set<state> next_que;
		auto it = que.begin();
		for (int i = 0; i < beam_width && it!=que.end(); i++) {
			state now_state = *it; it++;
			for (int y = 1; y < board_size_y-1; y++) {
				for (int x = 1; x < board_size_x - 1; x++) {
					if (now_state.board[y][x] != -1)
						continue;
					bool can_set = false;
					for (int j = 0; j < 4; j++) {
						if (now_state.board[y + dy[j]][x + dx[j]] != -1) {
							can_set = true;
							break;
						}
					}
					if (can_set == false)
						continue;
					for (int pidx_a = 0; pidx_a < piece_num; pidx_a++) {
						if (now_state.used[pidx_a])
							continue;
						now_state.board[y][x] = pidx_a;
						now_state.used[pidx_a] = 1;
						for (int dir_a = 0; dir_a < 4; dir_a++) {
							int new_adjacent_num = 0;
							double new_difference_score_sum = 0;
							for (int j = 0; j < 4; j++) {
								if (now_state.board[y + dy[j]][x + dx[j]] == -1)
									continue;
								int pidx_b = now_state.board[y + dy[j]][x + dx[j]];
								int dir_b = now_state.direction[y + dy[j]][x + dx[j]];
								new_adjacent_num++;
								new_difference_score_sum += calc_difference_score(pieces[pidx_a].edge[(j - dir_a + 8) % 4], pieces[pidx_b].edge[(j - dir_b + 6) % 4], piece_size);
							}
							now_state.direction[y][x] = dir_a;
							now_state.adjacent_num += new_adjacent_num;
							now_state.difference_score_sum += new_difference_score_sum;
							next_que.insert(now_state);
							now_state.direction[y][x] = 0;
							now_state.adjacent_num -= new_adjacent_num;
							now_state.difference_score_sum -= new_difference_score_sum;
						}
						now_state.used[pidx_a] = 0;
					}
					now_state.board[y][x] = -1;
				}
			}
		}
		que = next_que;
	}
	return *que.begin();
}

void Main() {
	Window::Resize(Size(1280, 720));
	picture pic = inputpic();
	Image image(pic.size_x, pic.size_y, Palette::White);
	const Font font(30);
	for (int i = 0; i < pic.size_y; i++)for (int j = 0; j < pic.size_x; j++) {
		int r = pic.pix[i][j][0];
		int g = pic.pix[i][j][1];
		int b = pic.pix[i][j][2];
		image[i][j] = Color(r,g,b);
	}
	DynamicTexture texture(image);
	texture.fill(image);
	vector<piece> pieces = picture_2_pieces(pic);
	state ans=beam_search(pieces, pic.div_x, pic.div_y, pic.size_x / pic.div_x);
	const int board_size_x = 4 * pic.div_x;
	const int board_size_y = 4 * pic.div_y;
	int ax = 0, ay = 0;
	int selx = -1, sely = -1;
	int wheel, shift;
	while (1) {
		if (KeyA.pressed())
			ax+=2;
		if (KeyD.pressed())
			ax-=2;
		if (KeyW.pressed())
			ay+=2;
		if (KeyS.pressed())
			ay-=2;
		if (MouseL.up()) {
			if (selx != -1) {
				int putx = (Cursor::Pos().x - ax) / PSIZE;
				int puty = (Cursor::Pos().y - ay) / PSIZE;
				if (putx <= 0 || putx >= board_size_x - 1 || puty <= 0 || puty >= board_size_y - 1) {
					selx = -1;
					sely = -1;
				}
				else {
					swap(ans.board[sely][selx], ans.board[puty][putx]);
					swap(ans.direction[sely][selx], ans.direction[puty][putx]);
					selx = -1;
					sely = -1;
				}
			}
		}
		if (MouseL.down()) {
			selx = (Cursor::Pos().x - ax) / PSIZE;
			sely = (Cursor::Pos().y - ay) / PSIZE;
			if (selx <= 0 || selx >= board_size_x - 1 || sely <= 0 || sely >= board_size_y - 1) {
				selx = -1;
				sely = -1;
			}
		}
		if (KeyR.down() && selx != -1) {
			ans.direction[sely][selx] = (ans.direction[sely][selx] + 1) % 4;
		}
		wheel = Mouse::Wheel();
		if (wheel == 1) {
			PSIZE--;
		}
		else if (wheel == -1) {
			if(PSIZE!=1)
				PSIZE++;
		}
		shift = KeyLShift.pressed();
		for (int y = 1; y < board_size_y - 1; y++) {
			for (int x = 1; x < board_size_x - 1; x++) {
				if (y == sely && x == selx)
					continue;
				if (ans.board[y][x] == -1)
					continue;
					//Rect(x * 50 + ax, y * 50 + ay, x * 50 + ax + 50, y * 50 + ay + 50).draw(ColorF(1.0, 1.0, 1.0, 0.5));
				else {
					pieces[ans.board[y][x]].tex.resized(PSIZE).rotated(90_deg * ans.direction[y][x]).draw(x * PSIZE + ax, y * PSIZE + ay);
					if(shift)
						font(ans.board[y][x]).draw(x * PSIZE + ax, y * PSIZE + ay, ColorF(0.5, 0.5, 0.5, 0.8));
				}
			}
		}
		if (selx != -1) {
			if (ans.board[sely][selx] != -1) {
				pieces[ans.board[sely][selx]].tex.resized(PSIZE).rotated(90_deg * ans.direction[sely][selx]).draw(Cursor::Pos().x - PSIZE / 2, Cursor::Pos().y - PSIZE / 2);
				if (shift)
					font(ans.board[sely][selx]).draw(Cursor::Pos().x - PSIZE / 2, Cursor::Pos().y - PSIZE / 2, ColorF(0.5, 0.5, 0.5, 0.8));
			}
		}
		System::Update();
	}
}
