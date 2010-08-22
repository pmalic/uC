CC=g++

main: main.o
	$(CC) -L/opt/local/lib -o main main.o ../xbee-api/XBee.o -lboost_system

main.o: main.cpp
	$(CC) -Wall -pedantic -I../xbee-api -O3 -c main.cpp

clean:
	rm -f main main.o
