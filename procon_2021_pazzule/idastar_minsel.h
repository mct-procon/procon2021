#pragma once
using namespace std;
int idastar_minsel(int depth, int count, int sel_lim, vector<vector<int> > table, status ans, int px, int py);
void idastar_ms_solve(vector<vector<int> >* table, status* ans);
