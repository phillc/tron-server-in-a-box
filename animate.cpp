#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "board.h"

int main(int argc, char *argv[])
{
	const char *rfn = "results.pgn";
	const char *game_id;
	FILE *f;
	char s[8192], p1[256], p2[256], q1[256], q2[256], id[256], r[256];
	int w, h;
	int x, y;
	int i, d;

	if (argc != 2) {
		fprintf(stderr, "usage: %s game_id\n", argv[0]);
		return 1;
	}
	game_id = argv[1];

	memset(id, 0, sizeof(p1));
	memset(q1, 0, sizeof(p1));
	memset(q2, 0, sizeof(p1));

	if (!(f = fopen(rfn, "r"))) {
		fprintf(stderr, "fopen: %s: %s\n", rfn, strerror(errno));
		return 1;
	}
	while (fgets(s, sizeof(s), f)) {
		if (!strncmp(s, "[White \"", 8)) {
			memset(p1, 0, sizeof(p1));
			sscanf(s + 8, "%255[^\"]", p1);
		}
		if (!strncmp(s, "[Black \"", 8)) {
			memset(p2, 0, sizeof(p2));
			sscanf(s + 8, "%255[^\"]", p2);
		}
		if (!strncmp(s, "[Result \"", 9)) {
			memset(r, 0, sizeof(r));
			sscanf(s + 9, "%255[^\"]", r);
		}
		if (!strncmp(s, game_id, strlen(game_id))) {
			fgets(s, sizeof(s), f);
			fgets(s, sizeof(s), f);
			sscanf(s, "%d %d", &w, &h);
			Board b(w, h);
			for (y = 0; y < h; ++y) {
//			for (; h > 0; --h) {
				fgets(s, sizeof(s), f);
				for (x = 0; x < w; ++x) {
					switch (s[x]) {
					case '1':
						b.mx = x; b.my = y;
						break;
					case '2':
						b.ox = x; b.oy = y;
						break;
					case '#':
						b.write(x, y);
						break;
					}
				}
				printf("%s", s);
			}
			fgets(s, sizeof(s), f);
			s[strlen(s) - 1] = 0;
			printf("%s\n", s);
			printf("there are %d steps\n", strlen(s) / 2);
			int step = 0;
			b.write(b.mx, b.my, 2);
			b.write(b.ox, b.oy, 3);
			char fn[8192];
			snprintf(fn, sizeof(fn), "animate%3.3d.bmp", step);
			printf("writing %s\n", fn);
			b.screenshot(fn);

			for (i = 0; s[i] && s[i] != '\n'; ++i) {
				char dirs[4] = { 'N', 'E', 'S', 'W' };
				for (d = 0; d < 4; ++d)
					if (s[i] == dirs[d])
						break;
				if (d == 4)
					break;
				b.move(i % 2, (DIR)d);
				if (i % 2)
					b.write(b.ox, b.oy, 3);
				else
					b.write(b.mx, b.my, 2);
				if (i % 2) {
					step++;
					snprintf(fn, sizeof(fn),
					    "animate%3.3d.bmp", step);
					printf("writing %s\n", fn);
					b.screenshot(fn);
				}
			}
			b.print(false, s);
			printf("%s", s);
			b.screenshot("test.bmp");

			break;
		}
	}
	fclose(f);

	return 0;
}
