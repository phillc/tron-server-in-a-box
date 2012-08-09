/*
 * Copyright (c) 2010 Daniel Hartmeier <daniel@benzedrine.cx>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <list>
#include <map>
#include <deque>
#include <queue>
#include <stack>
#include <set>
#include <string>

#include "board.h"

#ifdef DEBUG
#define debug(format, ...)	do { fprintf(stderr, format, ## __VA_ARGS__); \
				} while (0)
#else
#define debug(format, ...)	
#endif

const char *dname[] = { "NORTH", "EAST", "SOUTH", "WEST" };
const char *rname[] = { "LOSS", "UNDECIDED", "DRAW", "WIN" };

Board::Board(const char *fn) : fn(fn), valid(false)
{
	FILE *f;
	char s[8192];

	if (!(f = fopen(fn, "r"))) {
		debug("Board::Board: fopen: '%s': %s\n", fn, strerror(errno));
		goto done;
	}
	if (!fgets(s, sizeof(s), f)) {
		debug("Board::Board: %s: empty\n", fn);
		goto done;
	}
	if (sscanf(s, "%d %d", &w, &h) != 2) {
		debug("Board::Board: %s: unparsable dimensions %s", fn, s);
		goto done;
	}
	if (w < 4 || w > 50 || h < 4 || h > 50) {
		debug("Board::Board: %s: invalid dimensions %d %d\n", fn, w, h);
		goto done;
	}
	memset(b, 0, sizeof(b));
	mx = my = ox = oy = -1;
	for (int y = 0; y < h; ++y) {
		if (!fgets(s, sizeof(s), f)) {
			debug("Board::Board: %s: unexpected EOF\n", fn);
			goto done;
		}
		for (int x = 0; x < w; ++x) {
			switch (s[x]) {
			case ' ':
				break;
			case '#':
				write(x, y);
				break;
			case '1':
				if (mx != -1 || my != -1) {
					debug("Board::Board: %s: '1' "
					    "doubly defined\n", fn);
					goto done;
				}
				mx = x; my = y;
				break;
			case '2':
				if (ox != -1 || oy != -1) {
					debug("Board::Board: %s: '2' "
					    "doubly defined\n", fn);
					goto done;
				}
				ox = x; oy = y;
				break;
			default:
				debug("Board::Board: %s: invalid char '%c'\n",
				    fn, s[x]);
				goto done;
			}
		}
	}
	if (mx == -1 || my == -1) {
		debug("Board::Board: %s: '1' not defined\n", fn);
		goto done;
	}
	if (ox == -1 || oy == -1) {
		debug("Board::Board: %s: '2' not defined\n", fn);
		goto done;
	}
	valid = true;
//	debug("Board::Board: %s: w %d, h %d, 1 (%d,%d), 2 (%d,%d)\n",
//	    fn, w, h, mx, my, ox, oy);
//	print();
	
done:
	if (f)
		fclose(f);
}

void
Board::print(bool op, char *s) const
{
	sprintf(s, "%d %d\n", w, h);
	s += strlen(s);
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			if (y == my && x == mx)
				*s++ = op ? '2' : '1';
			else if (y == oy && x == ox)
				*s++ = op ? '1' : '2';
			else
				*s++ = read(x, y) ? '#' : ' ';
		}
		*s++ = '\n';
	}
	*s = 0;
}

static void
putRGB(FILE *f, u_int8_t r, u_int8_t g, u_int8_t b)
{
	fwrite(&b, 1, 1, f);
	fwrite(&g, 1, 1, f);
	fwrite(&r, 1, 1, f);
}

static void
put16(FILE *f, u_int16_t d)
{
//	d = htons(d);
	fwrite(&d, 1, 2, f);
}

static void
put32s(FILE *f, int32_t d)
{
//	d = htonl(d);
	fwrite(&d, 1, 4, f);
}

static void
put32(FILE *f, u_int32_t d)
{
//	d = htonl(d);
	fwrite(&d, 1, 4, f);
}

void
Board::screenshot(const char *fn) const
{
	FILE *f;
	char magic[2] = { 'B', 'M' };
	unsigned rw, rh;
	int i, rx, ry, x, y;
	u_int8_t left, right, pixel;

	if ((f = fopen(fn, "w")) == NULL) {
		fprintf(stderr, "fopen: %s: %s\n", fn, strerror(errno));
		return;
	}
//	rw = w % 2 ? (w + 1) / 2 : w / 2;
	rw = w * 2;
	if (rw % 4)
		rw += 4 - (rw % 4);
	rh = h * 4;
printf("Board::screenshot: rw %u, rh %u\n", rw, rh);

	fwrite(magic, 1, 2, f);
	put32(f, 14 + 12 + 16 * 3 + rw * rh);	/* file size */
	put16(f, 0);				/* reserved */
	put16(f, 0);				/* reserved */
	put32(f, 14 + 12 + 16 * 3);		/* offset */

	put32(f, 12);				/* header */
	put16(f, w * 4);
	put16(f, h * 4);
	put16(f, 1);				/* color planes */
	put16(f, 4);				/* bits per pixel */

	putRGB(f, 255, 255, 255);
	putRGB(f, 0, 0, 0);
	putRGB(f, 255, 0, 0);
	putRGB(f, 0, 0, 255);
	for (i = 0; i < 12; ++i)
		putRGB(f, 0, 0, 0);

	for (ry = rh - 1; ry >= 0; --ry) {
		y = ry / 4;
		pixel = 0;
		for (rx = 0; rx < rw; ++rx) {
			x = rx / 2;
			if (x < w)
				right = read(x, y);
			else
				right = 0;
			pixel = (pixel << 4) | right;
			fwrite(&pixel, 1, 1, f);
		}
	}

	fclose(f);
}

bool
Board::isolated() const
{
	Board b(*this);
	queue<pair<int, int> > q;
	pair<int, int> o(ox, oy), n;
	bool found = false;

	q.push(pair<int, int>(mx, my));
	do {
		n = q.front();
		q.pop();
		if (n == o) {
			found = true;
			break;
		}
		if (!b.read(n.first, n.second)) {
			b.write(n.first, n.second);
			q.push(pair<int, int>(n.first, n.second - 1));
			q.push(pair<int, int>(n.first, n.second + 1));
			q.push(pair<int, int>(n.first - 1, n.second));
			q.push(pair<int, int>(n.first + 1, n.second));
		}
	} while (!q.empty());
	return !found;
}
