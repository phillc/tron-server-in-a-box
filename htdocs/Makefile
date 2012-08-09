all: getgame getplayer getratings getmaps

getgame: getgame.c cgi.o
	gcc -static -o getgame getgame.c cgi.o

getplayer: getplayer.cpp cgi.o
	g++ -static -o getplayer getplayer.cpp cgi.o

getmaps: getmaps.cpp cgi.o
	g++ -static -o getmaps getmaps.cpp cgi.o

getratings: getratings.cpp cgi.o
	g++ -static -o getratings getratings.cpp cgi.o

cgi.o: cgi.c cgi.h
	gcc -static -c -o cgi.o cgi.c

clean:
	rm -f getgame getplayer getratings getmaps *.o *.core

install:
	cp getgame getplayer getratings getmaps /var/www/htdocs/benzedrine.cx/tron/
	cp visualizer.html visualizer.js /var/www/htdocs/benzedrine.cx/tron/
	cp -R viz /var/www/htdocs/benzedrine.cx/tron/

