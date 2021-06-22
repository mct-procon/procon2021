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

picture inputpic() {
	picture res;
	FILE* fp = fopen("prob01.ppm", "r");
	if (fp==NULL) {
		//error
		assert(0);
	}
	char magic_number[10];
	fscanf(fp, "%s\n", magic_number);
	if (strcmp(magic_number,"P6")!=0) {
		//error
		assert(0);
	}
	fscanf(fp, "# %d %d\n", &res.div_x, &res.div_y);
	fscanf(fp, "# %d\n", &res.select_lim);
	fscanf(fp, "# %d %d\n", &res.select_cost, &res.swap_cost);
	fscanf(fp, "%d %d\n", &res.size_x, &res.size_y);
	fscanf(fp, "%d\n", &res.color_max);
	vector<vector<vector<unsigned char>>> tmppix(res.size_y, vector<vector<unsigned char>>(res.size_x, vector<unsigned char>(3)));
	for (int i = 0; i < res.size_y; i++)for (int j = 0; j < res.size_x; j++)for (int k = 0; k < 3; k++) {
		fread(&tmppix[i][j][k], 1, 1, fp);
	}
	res.pix = tmppix;
	fclose(fp);
	return res;
}
