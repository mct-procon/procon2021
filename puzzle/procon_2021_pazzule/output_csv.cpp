#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "status.h"
#include "lib.h"
using namespace std;
extern int h, w, sel_rate, swap_rate, sel_lim;

void output_csv(Status sta, Status res, string name) {
	ofstream csvfile(name);
	//csvfile << sta.compare_cost << "," << endl;
	csvfile << sta.total_dis << "," << endl;
	for (int i = 0; i < res.sel_cnt; i++) {
		status_sellect(sta, res.sel_place[i] % w, res.sel_place[i] / w);
		//csvfile << sta.compare_cost << "," << endl;
		csvfile << sta.total_dis << "," << endl;
		for (int j = 0; j < res.swap_cnt[i]; j++) {
			if (res.swap_operator[i][j] == 'U')
				status_move(sta, 0, -1);
			if (res.swap_operator[i][j] == 'D')
				status_move(sta, 0, 1);
			if (res.swap_operator[i][j] == 'L')
				status_move(sta, -1, 0);
			if (res.swap_operator[i][j] == 'R')
				status_move(sta, 1, 0);
			//csvfile << sta.compare_cost << "," << endl;
			csvfile << sta.total_dis << "," << endl;
		}
	}
	csvfile.close();
}