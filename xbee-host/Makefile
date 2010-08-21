all:	main

main: main.o
	g++ -L/opt/local/lib -o main main.o -lboost_system

main.o: main.cpp
	g++ -Wall -I../arduino2boost -I../xbee-api -c main.cpp

clean:
	rm -f main main.o
