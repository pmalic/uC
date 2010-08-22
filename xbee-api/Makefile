CC=g++

all: XBee.o

XBee.o: XBee.cpp XBee.h Platform.h
	$(CC) -Wall -pedantic -O3 -c XBee.cpp

clean:
	rm -f XBee.o
