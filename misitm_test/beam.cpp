#define _CRT_SECURE_NO_WARNINGS
#include <Siv3D.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <set>

using namespace std;
int PSIZE = 64;

bool chmin(int& a, int b) {
	if (a > b) {
		a = b;
		return 1;
	}
	return 0;
}

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
	FILE* fp = fopen("problem.ppm", "rb");
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
	//fscanf(fp, "%d\n", &res.color_max);
	int tmp;
	fread(&tmp, 4, 1, fp); //読み込みバグ対策
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
	vector<piece> res(piece_num, piece{ 0,vector<vector<vector<unsigned char>>>(0),DynamicTexture(init_img) });
	for (int i = 0; i < piece_num; i++) {
		res[i].piece_id = i;
	}
	int idx;
	//上辺
	idx = 0;
	for (int i = 0; i < pic.size_y; i += piece_size) {
		for (int j = 0; j < pic.size_x; j += piece_size) {
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

int calc_difference_score(vector<vector<unsigned char>> edge_a, vector<vector<unsigned char>> edge_b, int edge_width) {
	int sum = 0;
	for (int i = 0; i < edge_width; i++) {
		for (int j = 0; j < 3; j++) {
			//モザイクにしようとしたけど、最小誤差が良いのでは
			//int mosaic = 0;
			//int cnt = 0;
			int mosa_width = 3;
			int mindiv = INT_MAX;
			for (int x = -mosa_width; x <= mosa_width; x++) {
				if (i + x >= 0 && i + x < edge_width) {
					//mosaic += edge_b[edge_width - 1 - (i + x)][j];
					//cnt++;
					chmin(mindiv, abs(edge_a[i][j] - edge_b[edge_width - 1 - (i + x)][j]));
				}
			}
			//sum += abs(edge_a[i][j] - mosaic / cnt);
			sum += mindiv;
		}
	}

	return sum;
}

struct state { //ビームサーチで持つ状態を改造した
	vector<vector<int>> board; //その場所に何番のピースがあるか
	vector<vector<int>> direction; //向き
	vector<int> pos;	//位置の逆引き(何番のピースかより)
};

//差が小さいものを貪欲に選択していく
state search(const vector<piece>& pieces, const int16& div_x, const int16& div_y, const int& piece_size) {
	const int piece_num = div_x * div_y;
	const int16 board_size_x = 520;
	const int16 board_size_y = 520;
	const int dy[4] = { -1,0,1,0 }, dx[4] = { 0,1,0,-1 };

	state ans = { vector<vector<int>>(board_size_y,vector<int>(board_size_x,-1)),vector<vector<int>>(board_size_y,vector<int>(board_size_x,0)),vector<int>(piece_num,-1) };
	ans.board[256][256] = 0;
	ans.pos[0] = ((256) << 16) + 256;

	vector<bool> used(piece_num, 0);
	used[0] = 1;

	//diffをメモ
	vector<vector<int>> diff_memo(piece_num * 4, vector<int>(piece_num * 4, -1));
	//先にやる(並列)
#pragma omp parallel for
	for (int i = 0; i < piece_num; i++) {
		int I = i * 4;
		for (int id = 0; id < 4; id++) {
			auto base_edge = pieces[i].edge[id];
			for (int j = 0; j < piece_num; j++) {
				int J = j * 4;
				for (int jd = 0; jd < 4; jd++) {
					diff_memo[I + id][J + jd] = calc_difference_score(base_edge, pieces[j].edge[jd], piece_size);
				}
			}
		}
	}


	set<int> next_pos;	//次に置く場所の候補
	int next_pos_num = 4;
	for (int i = 0; i < 4; i++) {
		next_pos.insert(((256 + dy[i]) << 16) + 256 + dx[i]);
	}

	for (int count = 1; count < piece_num; count++) {	//全部のピース置くまで
		int mi = INT_MAX;
		int diff = 0;
		int apos = 0;
		int apiece = 0;
		int aspin = 0;

		for (auto nxpos : next_pos) {	//置く場所の候補より探索
			for (int join_piece = 0; join_piece < piece_num; join_piece++) {
				if (used[join_piece]) continue;	//まだ置いてないやつしか見ない
				for (int join_spin = 0; join_spin < 4; join_spin++) {	//回転数
					diff = 0;

					int join_cnt = 0;
					for (int join_d = 0; join_d < 4; join_d++) {	//接する辺を見る
						int base_x = (nxpos & 65535) + dx[join_d];
						int base_y = ((nxpos >> 16) & 65535) + dy[join_d];
						assert(base_x < board_size_x);
						assert(base_x >= 0);
						assert(base_y < board_size_y);
						assert(base_y >= 0);
						int base_piece = ans.board[base_y][base_x];
						if (base_piece == -1) continue;
						join_cnt++;

						//(n & 3) == (n % 4)
						int b_direction = (join_d + 2 - ans.direction[base_y][base_x] + 4) & 3;
						int j_direction = (join_d - join_spin + 4) & 3;
						int bpd = base_piece * 4 + b_direction;	//base_piece_direction
						int jpd = join_piece * 4 + j_direction;	//join_piece_direction
						//if (diff_memo[bpd][jpd] == -1) {	//まだ差を見てないならメモする
						//	diff_memo[bpd][jpd] = calc_difference_score(pieces[base_piece].edge[b_direction], pieces[join_piece].edge[j_direction], piece_size);
						//}
						assert(bpd < piece_num * 4);
						assert(jpd < piece_num * 4);
						diff += diff_memo[bpd][jpd];	//メモより計算
					}

					diff -= join_cnt * (INT_MAX / 1000000);	//多くの辺に接する形の方が良い
					if (chmin(mi, diff)) {	//最も差が小さいものを選択
						apos = nxpos;
						apiece = join_piece;
						aspin = join_spin;
					}
				}
			}
		}

		//ピースを置く
		int ans_x = apos & 65535;
		int ans_y = (apos >> 16) & 65535;
		ans.board[ans_y][ans_x] = apiece;
		ans.direction[ans_y][ans_x] = aspin;
		ans.pos[apiece] = apos;
		used[apiece] = 1;

		//描画
		for (int x = 0; x < board_size_x; x++) {
			for (int y = 0; y < board_size_y; y++) {
				if (ans.board[y][x] == -1)
					continue;
				pieces[ans.board[y][x]].tex.resized(PSIZE).rotated(90_deg * ans.direction[y][x]).draw(x * PSIZE, y * PSIZE);
			}
		}
		System::Update();

		//接する辺が変わるので更新
		for (int d = 0; d < 4; d++) {
			int x = dx[d] + ans_x;
			int y = dy[d] + ans_y;
			if (ans.board[y][x] == -1) {
				next_pos.insert((y << 16) + x);
			}
		}
		next_pos.erase(apos);

	}

	/* そのまま置く
	for (int i = 0; i < div_x; i++) {
		for (int j = 0; j < div_y; j++) {
			if (i == 0 && j == 0) continue;
			ans.board[div_y + j][div_x + i] = j * div_x + i;
		}
	}
	*/

	return ans;
}

bool output(const state& ans, const int& board_size_x, const int& board_size_y, const int& div_x, const int& div_y, const int& select_lim, const int& select_cost, const int& swap_cost) {
	int ox = -1, oy = -1;
	for (int y = 0; y < board_size_y; y++) {
		for (int x = 0; x < board_size_x; x++) {
			if (ans.board[y][x] != -1) {
				ox = x, oy = y;
				break;
			}
		}
		if (ox != -1)
			break;
	}
	for (int y = oy; y < oy + div_y; y++) {
		for (int x = ox; x < ox + div_x; x++) {
			if (ans.board[y][x] == -1) {
				return 0;
			}
		}
	}
	vector<vector<int>> out_place(div_y, vector<int>(div_x));
	for (int y = oy; y < oy + div_y; y++) {
		for (int x = ox; x < ox + div_x; x++) {
			out_place[ans.board[y][x] / div_x][ans.board[y][x] % div_x] = (y - oy) * div_x + (x - ox);
		}
	}
	FILE* fp;
	/*
	fp = fopen("output.txt", "w");
	fprintf(fp, "%d %d\n", div_y, div_x);
	fprintf(fp, "%d\n", select_lim);
	fprintf(fp, "%d %d\n", select_cost, swap_cost);
	for (int y = 0; y < div_y; y++) {
		for (int x = 0; x < div_x; x++) {
			if (x > 0) fprintf(fp, " ");
			fprintf(fp, "%d", out_place[y][x]);
		}
		fprintf(fp, "\n");
	}
	for (int y = 0; y < div_y; y++) {
		for (int x = 0; x < div_x; x++) {
			if (x > 0) fprintf(fp, " ");
			fprintf(fp, "%d", ans.direction[oy + y][ox + x]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	*/
	fp = fopen("output2.txt", "w");
	fprintf(fp, "%d %d\n", div_y, div_x);
	fprintf(fp, "%d\n", select_lim);
	fprintf(fp, "%d %d\n", select_cost, swap_cost);
	for (int y = 0; y < div_y; y++) {
		for (int x = 0; x < div_x; x++) {
			if (x > 0) fprintf(fp, " ");
			fprintf(fp, "%d", ans.board[oy + y][ox + x]);
		}
		fprintf(fp, "\n");
	}
	for (int y = 0; y < div_y; y++) {
		for (int x = 0; x < div_x; x++) {
			if (x > 0) fprintf(fp, " ");
			fprintf(fp, "%d", ans.direction[oy + y][ox + x]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return 1;
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
		image[i][j] = Color(r, g, b);
	}
	DynamicTexture texture(image);
	texture.fill(image);
	vector<piece> pieces = picture_2_pieces(pic);

	state ans = search(pieces, pic.div_x, pic.div_y, pic.size_x / pic.div_x);

	Print << pic.div_y << U"x" << pic.div_x;
	const int board_size_x = 520;
	const int board_size_y = 520;
	int ax = -256 * PSIZE, ay = -256 * PSIZE;
	int selx = -1, sely = -1;
	int wheel, shift;
	set<pair<int, int>> selects;
	int select_dir = 0;
	int select_rect_x0, select_rect_y0;
	int select_rect_x1, select_rect_y1;
	double select_avg_x, select_avg_y;
	while (System::Update()) {
		if (KeyEscape.pressed())
			System::Exit();
		if (KeyA.pressed())
			ax += 2;
		if (KeyD.pressed())
			ax -= 2;
		if (KeyW.pressed())
			ay += 2;
		if (KeyS.pressed())
			ay -= 2;
		if (KeySpace.down()) {
			if (output(ans, board_size_x, board_size_y, pic.div_x, pic.div_y, pic.select_lim, pic.select_cost, pic.swap_cost))
				Print << U"Output succeeded";
			else
				Print << U"Output failed";
		}
		if (MouseL.down()) {
			selx = (Cursor::Pos().x - ax) / PSIZE;
			sely = (Cursor::Pos().y - ay) / PSIZE;
			if (selx <= 0 || selx >= board_size_x - 1 || sely <= 0 || sely >= board_size_y - 1) {
				selx = -1;
				sely = -1;
			}
			else if (ans.board[sely][selx] == -1) {
				selx = -1;
				sely = -1;
			}
			if (selx == -1) {
				selects.clear();
			}
			else {
				if (selects.empty()) {
					selects.insert({ sely,selx });
				}
				select_avg_y = 0;
				select_avg_x = 0;
				for (auto [y,x] : selects) {
					select_avg_y += (double)y;
					select_avg_x += (double)x;
				}
				select_avg_y /= (int)selects.size();
				select_avg_x /= (int)selects.size();
			}
		}
		if (MouseL.up()) {
			bool overlapping = 0;
			int cy = Cursor::Pos().y;
			int cx = Cursor::Pos().x;
			for (auto [y, x] : selects) {
				double draw_x = x - select_avg_x, draw_y = y - select_avg_y;
				for (int i = 0; i < select_dir; i++) {
					double nx = draw_x, ny = draw_y;
					draw_x = -ny;
					draw_y = nx;
				}
				draw_x -= 0.5;
				draw_y -= 0.5;
				int ty = (cy + draw_y * PSIZE + PSIZE / 2 - ay) / PSIZE;
				int tx = (cx + draw_x * PSIZE + PSIZE / 2 - ax) / PSIZE;
				if (ans.board[ty][tx] != -1 && selects.count({ ty,tx }) == 0) {
					overlapping = 1;
					break;
				}
			}
			if (!overlapping) {
				map<pair<int, int>, int> board_mp;
				map<pair<int, int>, int> direction_mp;
				for (auto [y, x] : selects) {
					board_mp[{y, x}] = ans.board[y][x];
					direction_mp[{y, x}] = ans.direction[y][x];
					ans.board[y][x] = -1;
					ans.direction[y][x] = -1;
				}
				for (auto [y, x] : selects) {
					double draw_x = x - select_avg_x, draw_y = y - select_avg_y;
					for (int i = 0; i < select_dir; i++) {
						double nx = draw_x, ny = draw_y;
						draw_x = -ny;
						draw_y = nx;
					}
					draw_x -= 0.5;
					draw_y -= 0.5;
					int ty = (cy + draw_y * PSIZE + PSIZE / 2 - ay) / PSIZE;
					int tx = (cx + draw_x * PSIZE + PSIZE / 2 - ax) / PSIZE;
					ans.board[ty][tx] = board_mp[{y, x}];
					ans.direction[ty][tx] = (direction_mp[{y, x}] + select_dir)%4;
				}
				select_dir = 0;
				selects.clear();
			}
		}
		if (MouseR.down()) {
			select_rect_x0 = Cursor::Pos().x - ax;
			select_rect_y0 = Cursor::Pos().y - ay;
		}
		if (MouseR.up()) {
			select_rect_x0 += ax;
			select_rect_y0 += ay;
			select_rect_x1 = Cursor::Pos().x;
			select_rect_y1 = Cursor::Pos().y;
			if (select_rect_x0 > select_rect_x1)
				swap(select_rect_x0, select_rect_x1);
			if (select_rect_y0 > select_rect_y1)
				swap(select_rect_y0, select_rect_y1);
			for (int y = 1; y < board_size_y - 1; y++) {
				for (int x = 1; x < board_size_x - 1; x++) {
					if (ans.board[y][x] == -1)
						continue;
					for (int i = 0; i <= 4; i++) {
						int tx = (x + (i & 1)) * PSIZE + ax;
						int ty = (y + ((i >> 1) & 1)) * PSIZE + ay;
						if (i == 4) {
							tx = x * PSIZE + PSIZE / 2 + ax;
							ty = y * PSIZE + PSIZE / 2 + ay;
						}
						if (select_rect_x0 < tx && tx < select_rect_x1 && select_rect_y0 < ty && ty < select_rect_y1) {
							selects.insert({ y,x });
							break;
						}
					}
					
				}
			}
		}
		if (KeyR.down() && !selects.empty()) {
			select_dir = (select_dir + 1) % 4;
		}
		wheel = Mouse::Wheel();
		if (wheel == 1) {
			int dec_psz = PSIZE/2;
			if (PSIZE-dec_psz>=1) {
				for (int i = 0; i < dec_psz; i++) {
					ax -= (ax - Cursor::Pos().x) / PSIZE;
					ay -= (ay - Cursor::Pos().y) / PSIZE;
					PSIZE--;
				}
			}
		}
		else if (wheel == -1) {
			int inc_psz = PSIZE;
			for (int i = 0; i < inc_psz; i++) {
				ax += (ax - Cursor::Pos().x) / PSIZE;
				ay += (ay - Cursor::Pos().y) / PSIZE;
				PSIZE++;
			}
		}
		shift = KeyLShift.pressed();
		Rect(board_size_x/2*PSIZE+ax, board_size_y / 2 * PSIZE+ay, pic.div_x* PSIZE, pic.div_y*PSIZE).drawFrame(5, ColorF(1.0, 0, 0, 0.2));
		for (int y = 1; y < board_size_y - 1; y++) {
			for (int x = 1; x < board_size_x - 1; x++) {
				if (ans.board[y][x] == -1) {
					Rect(x * PSIZE + ax, y * PSIZE + ay, PSIZE, PSIZE).draw(ColorF(1.0, 1.0, 1.0, 0.1));
				}
				else if (MouseL.pressed() && selects.count({ y,x })) {
					Rect(x* PSIZE + ax, y* PSIZE + ay, PSIZE, PSIZE).draw(ColorF(1.0, 1.0, 1.0, 0.1));
				}
				else {
					pieces[ans.board[y][x]].tex.resized(PSIZE).rotated(90_deg * ans.direction[y][x]).draw(x * PSIZE + ax, y * PSIZE + ay);
					if (shift) {
						font(ans.board[y][x]).draw(x * PSIZE + ax, y * PSIZE + ay, ColorF(0.5, 0.5, 0.5, 0.8));
					}
					if (selects.count({ y,x })) {
						Rect(x * PSIZE + ax, y * PSIZE + ay, PSIZE, PSIZE).drawFrame(5, ColorF(1.0, 1.0, 0, 0.5));
					}
				}
				if (ans.board[y][x] == 0) {
					font(U"dir of 0 : {}"_fmt(ans.direction[y][x])).draw(1100, 0, ColorF(0.5, 0.5, 0.5, 0.8));
				}
			}
		}
		if (MouseL.pressed()) {
			for (auto [y,x] : selects) {
				double draw_x = x - select_avg_x, draw_y = y - select_avg_y;
				for (int i = 0; i < select_dir; i++) {
					double nx = draw_x, ny = draw_y;
					draw_x = -ny;
					draw_y = nx;
				}
				draw_x -= 0.5;
				draw_y -= 0.5;
				pieces[ans.board[y][x]].tex.resized(PSIZE).rotated(90_deg * ((ans.direction[y][x] + select_dir)%4)).draw(Cursor::Pos().x + draw_x * PSIZE, Cursor::Pos().y + draw_y * PSIZE);
			}
		}
		if (MouseR.pressed()) {
			Rect(select_rect_x0 + ax, select_rect_y0 + ay, Cursor::Pos().x - (select_rect_x0 + ax), Cursor::Pos().y - (select_rect_y0 + ay)).drawFrame(3, 0);
		}
	}

}
