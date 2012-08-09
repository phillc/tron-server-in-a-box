#include <stdio.h>

#include "cgi.h"

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

/*
+OK|15 15|###############
#1            #
#             #
#             #
#             #
#             #
#             #
#             #
#             #
#             #
#             #
#             #
#             #
#            2#
###############
|dhartmei|gillesjac|EWENENSNSWSWSWEWENSNSWSW|+OK
*/

int main()
{
	struct query *q = get_query();
	const char *game_id = get_query_param(q, "game_id");
	FILE *f;
	char s[8192], p1[256], p2[256], q1[256], q2[256], id[256], r[256];
	int w, h;

	memset(id, 0, sizeof(p1));
	memset(q1, 0, sizeof(p1));
	memset(q2, 0, sizeof(p1));
	printf("HTTP/1.0 200 OK\n");
	printf("Content-Type: text/plain\n");
	printf("Connection: close\n");
	printf("\n");
	if (strchr(game_id, '|')) {
		if (sscanf(game_id, "%255[^|]|%255[^|]|%255[^|]",
		    id, q1, q2) != 3) {
			printf("Error: invalid game_id %s\n", game_id);
			goto done;
		}
	} else
		strlcpy(id, game_id, sizeof(id));
	f = fopen(fn, "r");
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
		if (!strncmp(s, id, strlen(id)) &&
		    (!q1[0] || !strcmp(p1, q1)) &&
		    (!q2[0] || !strcmp(p2, q2))) {
			fgets(s, sizeof(s), f);
			fgets(s, sizeof(s), f);
			sscanf(s, "%d %d", &w, &h);
			s[strlen(s) - 1] = 0;
			printf("+OK|%d %d|", w, h);
			for (; h > 0; --h) {
				fgets(s, sizeof(s), f);
				printf("%s", s);
			}
			printf("|%s|%s|", p1, p2);
			fgets(s, sizeof(s), f);
			s[strlen(s) - 1] = 0;
			printf("%s|+OK\n", s);
			break;
		}
	}
done:
	fclose(f);
        fflush(stdout);
	return (0);
}
