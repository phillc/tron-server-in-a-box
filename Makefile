all: server expire genmap animate

server: server.o board.o
	g++ -o server server.o board.o

server.o: server.cpp board.h
	g++ -c -o server.o server.cpp

board.o: board.cpp board.h
	g++ -c -o board.o board.cpp

expire: expire.c
	gcc -o expire expire.c

genmap: genmap.o board.o graph.o
	g++ -o genmap genmap.o board.o graph.o

genmap.o: genmap.cpp board.h
	g++ -c -o genmap.o genmap.cpp

graph.o: graph.cpp graph.h
	g++ -c -o graph.o graph.cpp

animate: animate.o board.o
	g++ -o animate animate.o board.o

animate.o: animate.cpp board.h
	g++ -c -o animate.o animate.cpp

clean:
	rm -f server expire genmap animate *.o *.core

release:
	make clean
	cd htdocs; make clean
	tar zcvf tronserver.tar.gz README Makefile server.cpp board.h board.cpp graph.h graph.cpp expire.c tcptron.c genmap.cpp animate.cpp animate.sh maps htdocs
	mv tronserver.tar.gz /var/www/htdocs/benzedrine.cx/
	cp tcptron.c /var/www/htdocs/benzedrine.cx/
