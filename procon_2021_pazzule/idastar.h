#pragma once
using namespace std;
int idastar(int depth, int count, vector<vector<int> > table, status ans, int px, int py);
void idastar_solve(vector<vector<int> >* table, status* ans);