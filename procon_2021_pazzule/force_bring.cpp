#include <iostream>
#include <map>
#include <vector>
#include "status.h"

using namespace std;
extern vector<vector<unsigned char> > complete;
extern vector<int> goal_place;
extern Status answer;
extern int h, w, sel_rate, swap_rate, sel_lim, search_dir;

// ¶ã‚©‚ç‚Ğ‚½‚·‚ç‘µ‚¦‚Ä‚¢‚­
void fb_solve(Status *status) {
	vector<int> target(sel_lim); // index‚ğ–Ú•WêŠ‚É‚·‚é”š‚ÌŒ»İ’n
	int n = min(h * w, sel_lim);
	for (int i = 0; i < n; i++) {
		target[i] = complete[i / w][i % w];
	}

	// ‚Ğ‚½‚·‚ç¶ã‚©‚ç‹l‚ß‚é
	for (int i = 0; i < n;  i++) {
		if (i == 6) {
			int sum;
			sum = i + i + i;
		}
		status->sellect(target[i] % w, target[i] / w, sel_rate);
		while (status->x + status->y * w != i) {
			int dx = 0, dy = 0;
			if (status->x < i % w) dx = 1;
			else if (status->x > i % w) dx = -1;
			else if (status->y < i / w) dy = 1;
			else if (status->y > i / w) dy = -1;
			for (int j = 0; j < n; j++) {
				if (target[j] == status->x + dx + (status->y + dy) * w) {
					target[j] -= dx + dy * w;
					break;
				}
			}
			target[i] += dx + dy * w;
			status->move(dx,dy, h, w, swap_rate, goal_place);
		}
	}
	answer = *status;

	answer.show();
	answer.show_cost();
}
