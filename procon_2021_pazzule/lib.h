#pragma once

void show_table(vector<vector<int> > table);
void input(vector<vector<int> > *table);
void status_init(Status& sta, int _x, int _y);
void status_sellect(Status& sta, int _x, int _y);
void status_move(Status& sta, int dx, int dy);
void status_unmove(Status& sta, int pre_x, int pre_y);
void status_show(Status& sta);
void status_show_cost(Status& sta);
void status_sub_for(Status& sta);
void status_calc_val(Status& sta);
void status_update_val(Status& sta, int dx, int dy);
#define HAND 0
#define IDAS 1
#define ASTAR 2
#define FB 3
#define DB 4
