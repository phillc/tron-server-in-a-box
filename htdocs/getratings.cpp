#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>

extern "C" {
#include "cgi.h"
}

using namespace std;

const char *fn = "ratings.txt";

/*
version 0056, Copyright (C) 1997-2007 Remi Coulom.
compiled Feb 25 2010 09:38:06.
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under the terms and conditions of the GNU General Public License.
See http://www.gnu.org/copyleft/gpl.html for details.
ResultSet>off
1000 game(s) loaded, 0 game(s) with unknown result ignored.^M2000 game(s) loaded
, 0 game(s) with unknown result ignored.^M2017 game(s) loaded, 0 game(s) with un
known result ignored.
00:00:00,00
40 left    ^M30 left    ^M20 left    ^M10 left    ^M0 left    ^M00:00:01,00

Rank Name           Elo    +    - games score oppo. draws
   1 anoever        147  219  183     8   75%    -6   25%
   2 a1k0n          146   35   34   227   64%    58   61%
   3 txandi         126   63   62    70   56%    86   53%
   4 mightybyte3    125   26   26   408   61%    56   58%
*/

struct Player {
	char		 name[256];
	unsigned	 rank, games, score, draws;
	int		 elo, plus, minus, oppo;
};

int main()
{
	vector<Player> players;
	FILE *f;
	bool skip = true;

	unsigned rank;
	char s[8192];

	printf("HTTP/1.0 200 OK\n");
	printf("Content-Type: text/html\n");
	printf("Connection: close\n");
	printf("\n");
	printf("<html><body>\n");
	printf("<h2>Current ratings</h2>\n");

	f = fopen(fn, "r");
	while (fgets(s, sizeof(s), f)) {
		if (skip) {
			if (!strncmp(s, "Rank Name ", 10))
				skip = false;
			continue;
		}
		Player p;
		if (sscanf(s, "%u %255s %d %d %d %u %u%% %d %u%%",
		    &p.rank, p.name, &p.elo, &p.plus, &p.minus,
		    &p.games, &p.score, &p.oppo, &p.draws) != 9)
			continue;
		players.insert(players.end(), p);
	}
	fclose(f);

	printf("<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\">\n");
	printf("<tr><th>Rank</th><th align=\"left\">Name</th><th>Elo</th><th>+</th><th>-</th><th>games</th><th>score</th><th>oppo.</th><th>draws</th></tr>\n");
	for (unsigned i = 0; i < players.size(); ++i) {
		Player p = players[i];
		const char *bg = i % 2 ? "#FFFFFF" : "#DDDDFF";
		printf("<tr bgcolor=\"%s\"><td align=\"right\">%u</td><td><a href=\"getplayer?player=%s\">%s</a></td><td align=\"right\"><b>%d</b></td><td align=\"right\">%d</td><td align=\"right\">%d</td><td align=\"right\">%u</td><td align=\"right\">%u%%</td><td align=\"right\">%d</td><td align=\"right\">%u%%</td></tr>\n",
		    bg, p.rank, p.name, p.name, p.elo, p.plus, p.minus,
		    p.games, p.score, p.oppo, p.draws);
	}
	printf("</table>\n");

	printf("</body><html>\n");
        fflush(stdout);
	return (0);
}
