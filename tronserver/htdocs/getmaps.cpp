#include <stdio.h>
#include <string.h>
#include <time.h>
#include <utility>
#include <vector>
#include <map>
#include <string>

extern "C" {
#include "cgi.h"
}

using namespace std;

const char *fn = "results.pgn";

/*
[White "hebbie"]
[Black "ebrahim-2"]
[Result "1/2-1/2"]
1267133152
maps/ring.txt
15 15
###############
#    2        #
# ##### ##### #
# #         # #
# #         # #
# #         # #
# #         # #
#             #
# #         # #
# #         # #
# #         # #
# #         # #
# ##### ##### #
#        1    #
###############
WEWENSNSNWWSNSNSESEEEEEENE
*/

struct Game {
	string p1, p2;
	string result;
	string id;
	string map;
};

int main()
{
	vector<Game> games;
	map<string, pair<unsigned, unsigned> > maps;
	struct query *q = get_query();
	FILE *f;
	char s[8192], p1[256], p2[256], r[256];
	int w, h;

	printf("HTTP/1.0 200 OK\n");
	printf("Content-Type: text/html\n");
	printf("Connection: close\n");
	printf("\n");
	printf("<html><body>\n");
	printf("<h2>Map statistics</h2>\n");
	f = fopen(fn, "r");
	while (fgets(s, sizeof(s), f)) {
		if (!strncmp(s, "[White \"", 8)) {
			Game g;
			memset(p1, 0, sizeof(p1));
			sscanf(s + 8, "%255[^\"]", p1);
			g.p1 = p1;
			fgets(s, sizeof(s), f);
			if (!strncmp(s, "[Black \"", 8)) {
				memset(p2, 0, sizeof(p2));
				sscanf(s + 8, "%255[^\"]", p2);
				g.p2 = p2;
				fgets(s, sizeof(s), f);
				if (!strncmp(s, "[Result \"", 9)) {
					memset(r, 0, sizeof(r));
					sscanf(s + 9, "%255[^\"]", r);
					s[strlen(s) - 1] = 0;
					if (!strcmp(r, "1/2-1/2"))
						g.result = "DRAW";
					else
						g.result = "WIN";
					fgets(s, sizeof(s), f);
					g.id = s;
					fgets(s, sizeof(s), f);
					g.map = s;
					games.insert(games.end(), g);
				}
			}
		}
	}
	fclose(f);
	for (unsigned i = 0; i < games.size(); ++i) {
		map<string, pair<unsigned, unsigned> >::iterator m =
		    maps.find(games[i].map);
		if (m == maps.end())
			if (games[i].result == "DRAW")
				maps.insert(make_pair(games[i].map, make_pair((unsigned)1, (unsigned)0)));
			else
				maps.insert(make_pair(games[i].map, make_pair((unsigned)0, (unsigned)1)));
		else {
			if (games[i].result == "DRAW")
				m->second.first++;
			else
				m->second.second++;
		}
	}
	printf("<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\">\n");
	printf("<tr><th align=\"left\">Name</th><th align=\"left\">Games</th><th align=\"left\">Draws (%%)</th></tr>\n");
	unsigned tg = 0, td = 0;
	unsigned i = 0;
	for (map<string, pair<unsigned, unsigned> >::iterator m = maps.begin();
	    m != maps.end(); ++m) {
		const char *bg = i++ % 2 ? "#FFFFFF" : "#DDDDFF";
		unsigned g = m->second.first + m->second.second;
		unsigned d = m->second.first;
		tg += g;
		td += d;
		printf("<tr bgcolor=\"%s\"><td><a href=\"%s\">%s</a></td><td align=\"right\">%u</td><td align=\"right\">%u (%u%%)</td></tr>\n", bg, m->first.c_str(), m->first.c_str(), g, d, g > 0 ? d * 100 / g : 0);
	}
	const char *bg = i++ % 2 ? "#FFFFFF" : "#DDDDFF";
	printf("<tr bgcolor=\"%s\"><td>Total</td><td align=\"right\">%u</td><td align=\"right\">%u (%u%%)</td></tr>\n", bg, tg, td, tg > 0 ? td * 100 / tg : 0);
	printf("</table>\n");
	printf("</body><html>\n");
        fflush(stdout);
	return (0);
}
