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
#include "graph.h"

class Vertex {
public:
	Vertex(int x, int y) : x(x), y(y), visited(false), articulation(false) { }
	void dfs_visit();
	int x, y;
	set<Vertex *> adjacents;
	bool visited, articulation;
	unsigned num, low;
	Vertex *parent;
};

void
Vertex::dfs_visit()
{
	visited = true;
	/* don't step OUT of articulations */
	if (articulation)
		return;
	for (set<Vertex *>::iterator v = adjacents.begin();
	    v != adjacents.end(); ++v)
		if (!(*v)->visited)
			(*v)->dfs_visit();
}

Graph::Graph(const Board &b) : counter(0)
{
	for (int y = 0; y < b.getHeight(); ++y) {
		for (int x = 0; x < b.getWidth(); ++x) {
			if (!b.read(x, y)) {
				Vertex *v = new Vertex(x, y);
				vertices.insert(make_pair(
				    make_pair(x, y), v));
				if (!b.read(x, y - 1))
					addEdge(v, vertices[
					    make_pair(x, y - 1)]);
				if (!b.read(x - 1, y))
					addEdge(v, vertices[
					    make_pair(x - 1, y)]);
			}
		}
	}
}

Graph::~Graph()
{
	for (map<pair<int, int>, Vertex *>::iterator v =
	    vertices.begin(); v != vertices.end(); ++v)
		delete v->second;
}

void
Graph::findArticulations(pair<int, int> p) {
	map<pair<int, int>, Vertex *>::iterator v = vertices.find(p);
	if (v == vertices.end()) {
		return;
	}
	findArt(v->second);
}

set<pair<int, int> >
Graph::findAllArticulations(pair<int, int> p)
{
	findArticulations(p);
	set<pair<int, int> > s;
	for (set<Vertex *>::const_iterator v = articulations.begin();
	    v != articulations.end(); ++v)
		s.insert(make_pair((*v)->x, (*v)->y));
	return s;
}

void
Graph::addEdge(Vertex *a, Vertex *b) {
	a->adjacents.insert(b);
	b->adjacents.insert(a);
}

void
Graph::findArt(Vertex *v)
{
	v->visited = true;
	v->low = v->num = counter++;
	for (set<Vertex *>::iterator w = v->adjacents.begin();
	    w != v->adjacents.end(); ++w) {
		if (!(*w)->visited) {
			(*w)->parent = v;
			findArt(*w);
			if ((*w)->low >= v->num) {
				v->articulation = true;
				articulations.insert(v);
			}
			if ((*w)->low < v->low)
				v->low = (*w)->low;
		} else {
			if (v->parent != *w)
				if ((*w)->num < v->low)
					v->low = (*w)->num;
		}
	}
}
