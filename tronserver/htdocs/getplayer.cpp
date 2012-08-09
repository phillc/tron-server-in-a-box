#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
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
	string opponent;
	string result;
	string id;
	string map;
	string moves;
};

static char *
timestr(time_t t)
{
	static char s[128];
	struct tm *tm;

	tm = gmtime(&t);
	snprintf(s, sizeof(s), "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d",
	    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	    tm->tm_hour, tm->tm_min, tm->tm_sec);
	return s;
}

int main()
{
	vector<Game> games;
	struct query *q = get_query();
	const char *player = get_query_param(q, "player");
	FILE *f;
	char s[8192], p1[256], p2[256], r[256];
	int w, h;

	printf("HTTP/1.0 200 OK\n");
	printf("Content-Type: text/html\n");
	printf("Connection: close\n");
	printf("\n");
	printf("<html><body>\n");
	if (!player || !player[0]) {
		printf("<b>Error: player not specified</b><br>\n");
		goto done;
	}
	printf("<h2>Latest games of %s</h2>\n", player);
	f = fopen(fn, "r");
	while (fgets(s, sizeof(s), f)) {
		if (!strncmp(s, "[White \"", 8)) {
			memset(p1, 0, sizeof(p1));
			sscanf(s + 8, "%255[^\"]", p1);
			fgets(s, sizeof(s), f);
			if (!strncmp(s, "[Black \"", 8)) {
				memset(p2, 0, sizeof(p2));
				sscanf(s + 8, "%255[^\"]", p2);
				fgets(s, sizeof(s), f);
				if (!strncmp(s, "[Result \"", 9)) {
					memset(r, 0, sizeof(r));
					sscanf(s + 9, "%255[^\"]", r);
					fgets(s, sizeof(s), f);
					s[strlen(s) - 1] = 0;
					Game game;
					game.p1 = p1;
					game.p2 = p2;
					if (!strcmp(p1, player)) {
						game.opponent = p2;
						if (!strcmp(r, "0-1"))
							game.result = "Loss";
						else if (!strcmp(r, "1-0"))
							game.result = "Win";
						else
							game.result = "Draw";
					} else if (!strcmp(p2, player)) {
						game.opponent = p1;
						if (!strcmp(r, "0-1"))
							game.result = "Win";
						else if (!strcmp(r, "1-0"))
							game.result = "Loss";
						else
							game.result = "Draw";
					} else
						continue;
					game.id = s;
					fgets(s, sizeof(s), f);
					game.map = s;
					fgets(s, sizeof(s), f);
					int w, h;
					sscanf(s, "%d %d", &w, &h);
					while (h-- >= 0)
						fgets(s, sizeof(s), f);
					game.moves = s;
					games.insert(games.end(), game);
				}
			}
		}
	}
	fclose(f);
	if (!games.size()) {
		printf("No games found.<p>\n");
		goto done;
	}

	printf("Found %u games.<p>\n", (unsigned)games.size());
	printf("<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\">\n");
	printf("<tr><th align=\"left\">Date Time (UTC)</th><th align=\"left\">Opponent</th><th align=\"left\">Outcome</th></tr>\n");
	for (unsigned i = 0; i < 32 && i < games.size(); ++i) {
		const char *bg = i % 2 ? "#FFFFFF" : "#DDDDFF";
		Game g = games[games.size() - i - 1];
		const char *fg = g.result == "Draw" ? "#000000" : (g.result == "Win" ? "#00AA00" : "#EE0000");
		string remark;
		if (g.moves[g.moves.size() - 2] == 'T' ||
		    g.moves[g.moves.size() - 3] == 'T')
			remark = "<sup><small>T</small></sup>";
		if (g.moves[g.moves.size() - 2] == 'D' ||
		    g.moves[g.moves.size() - 3] == 'D')
			remark = "<sup><small>D</small></sup>";
		printf("<tr bgcolor=\"%s\"><td>%s</td><td><a href=\"getplayer?player=%s\">%s</a></td><td><font color=\"%s\">%s%s</font></td><td><a href=\"visualizer.html?game_id=%s|%s|%s\">View Game &gt;&gt;</a></td></tr>\n", bg, timestr(atol(g.id.c_str())), g.opponent.c_str(), g.opponent.c_str(), fg, g.result.c_str(), remark.c_str(), g.id.c_str(), g.p1.c_str(), g.p2.c_str());
	}
	printf("</table>\n");
	printf("<p><small><sup>T</sup>=timed out, <sup>D</sup>=disconnected</small>\n");
done:
	printf("</body><html>\n");
        fflush(stdout);
	return (0);
}
