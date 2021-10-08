#pragma once

void show_table(vector<vector<int> > table);
void input(vector<vector<int> > &table);
void status_init(Status& sta, int _x, int _y);
void status_sellect(Status& sta, int _x, int _y);
void status_unsellect(Status& sta, int pre_x, int pre_y);
void status_move(Status& sta, int dx, int dy);
void status_unmove(Status& sta);
void status_show(Status& sta);
void status_show_cost(Status& sta);
void status_sub_for();
void status_calc_val(Status& sta);
void status_update_val(Status& sta, int dx, int dy);
template<typename T> bool chmax(T& a, T b) {
	if (a < b) { a = b; return 1; }
	return 0;
}
template<typename T> bool chmin(T& a, T b) {
	if (a > b) { a = b; return 1; }
	return 0;
}
#define HAND 0
#define IDAS 1
#define ASTAR 2
#define FB 3
#define DB 4
#define OW 5
#define IDASD 6
#define DFS_OW 7
#define SHORTCUT 8
#define OW_B 10
#define OWA 9