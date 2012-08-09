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

#ifndef __BOARD_H__
#define __BOARD_H__

#include <sys/types.h>
#include <utility>
#include <string>

using namespace std;

enum DIR { NORTH=0, EAST=1, SOUTH=2, WEST=3 };
enum RESULT { LOSS=0, UNDECIDED=1, DRAW=2, WIN=3 };

extern const char *dname[];
extern const char *rname[];

inline pair<int, int>
step(pair<int, int> s, DIR d)
{
	switch (d) {
	case NORTH: s.second--; break;
	case EAST:  s.first++;  break;
	case SOUTH: s.second++; break;
	case WEST:  s.first--;  break;
	}
	return s;
}

class Board {
public:
	Board(int w, int h) : w(w), h(h) { memset(b, 0, sizeof(b)); }
	Board(const char *fn);
	bool isValid() const { return valid; }
	bool operator==(const Board &o) const {
		return mx == o.mx && my == o.my && ox == o.ox && oy == o.oy &&
		    !memcmp(b, o.b, sizeof(b));
	}
	u_int8_t inline read(int x, int y) const {
		return b[y * w + x];
	}
	void inline write(int x, int y, u_int8_t v = 1) {
		b[y * w + x] = v;
	}
	void inline clear(int x, int y) {
		b[y * w + x] = 0;
	}
	void inline move(bool op, DIR dir) {
		int &x = op ? ox : mx, &y = op ? oy : my;
		write(x, y, op ? 3 : 2);
		switch (dir) {
		case NORTH: y--; break;
		case EAST:  x++; break;
		case SOUTH: y++; break;
		case WEST:  x--; break;
		}
	}
	unsigned inline options(bool op) const {
		pair<int, int> m = op ? getOpPos() : getMyPos();
		pair<int, int> o = op ? getMyPos() : getOpPos();
		unsigned count = 0;
		for (int d = 0; d < 4; ++d) {
			pair<int, int> n = step(m, (DIR)d);
			if (n != o && !read(n.first, n.second))
				count++;
		}
		return count;
	}
	RESULT rate() const {
		/* if both players are in the same space */
		if (mx == ox && my == oy)
			return DRAW;
		/* is either player in a wall? */
		bool m = read(mx, my), o = read(ox, oy);
		if (m && o)
			return DRAW;
		if (m)
			return LOSS;
		if (o)
			return WIN;
		/* !m && !o */
		/* is either player stuck? */
		m = !options(false);
		o = !options(true);
		if (m && o)
			return DRAW;
		if (m)
			return LOSS;
		if (o)
			return WIN;
		return UNDECIDED;
	}
	bool isolated() const;
	int getWidth() const { return w; }
	int getHeight() const { return h; }
	pair<int, int> getMyPos() const { return pair<int, int>(mx, my); }
	pair<int, int> getOpPos() const { return pair<int, int>(ox, oy); }
	void print(bool op, char *s) const;
	void screenshot(const char *fn) const;
	string getFn() const { return fn; }
//private:
	string fn;
	bool valid;
	u_int8_t b[2500];
	int w, h;
	int mx, my, ox, oy;
};

#endif
