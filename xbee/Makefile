CXX=g++
CXXFLAGS=-DBOOST_GCC -Wall -pedantic -O2

all: XBee.o

XBee.o: XBee.cpp XBee.h Platform.h
	$(CXX) $(CXXFLAGS) -c -o XBee.o XBee.cpp

clean:
	rm -f XBee.o
