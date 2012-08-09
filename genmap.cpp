#include <stdio.h>
#include <utility>
#include <map>
#include <set>

#include "board.h"
#include "graph.h"

using namespace std;

static void
mirror(int w, int h, int &x, int &y)
{
/* line symmetry
	if (w % 2)
		y = h - y - 1;
	else
		x = w - x - 1;
*/
	/* point symmetry */
	x = w - x - 1;
	y = h - y - 1;
}

int main(int argc, char *argv[])
{
	multimap<unsigned, Board> boards;
	int w, h, x, y, i, j;
	char s[8192], fn[8192];
	const char *prefix;
	int count, c, max;
	FILE *f;

	if (argc != 4) {
		printf("usage: %s prefix count max\n", argv[0]);
		return 1;
	}
	prefix = argv[1];
	count = atoi(argv[2]);
	max = atoi(argv[3]);

	c = 0;
	while (c < max) {
		w = 15 + (arc4random() % 36); /* 15 <= w <= 50 */
		if (w % 2)
			h = 16 + ((arc4random() % 18) * 2);
		else
			h = 15 + ((arc4random() % 18) * 2);
		/* (w even && h odd) || (w odd && h even) */
		Board b(w, h);
		b.mx = 1 + (arc4random() % (w - 2));
		b.my = 1 + (arc4random() % (h - 2));
		b.ox = b.mx;
		b.oy = b.my;
		mirror(b.w, b.h, b.ox, b.oy);
		for (x = 0; x < w; ++x) {
			b.write(x, 0);
			b.write(x, h - 1);
		}
		for (y = 0; y < h; ++y) {
			b.write(0, y);
			b.write(w - 1, y);
		}
		j = w * h / (8 + (arc4random() % 23));
		for (i = 0; i < j; ++i) {
			x = 1 + (arc4random() % (b.w - 2));
			y = 1 + (arc4random() % (b.h - 2));
			if ((x == b.mx && y == b.my) ||
			    (x == b.ox && y == b.oy))
				continue;
			b.write(x, y);
			mirror(b.w, b.h, x, y);
			b.write(x, y);
		}
		if (b.rate() != UNDECIDED || b.isolated())
			continue;
		Graph g(b);
		set<pair<int, int> > aps = g.findAllArticulations(b.getMyPos());
		boards.insert(make_pair((unsigned)(j / aps.size()), b));
		c++;
	}

	multimap<unsigned, Board>::iterator b = boards.begin();
	while (count > 0) {
		b->second.print(false, s);
		snprintf(fn, sizeof(fn), "%s%3.3d.txt", prefix, count);
		printf("writing %s\n", fn);
		f = fopen(fn, "w");
		if (f) {
			fprintf(f, "%s", s);
			fflush(f);
			fclose(f);
		}
		count--;
		b++;
	}
	return 0;
}
