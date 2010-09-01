PLATFORM=_BOOST
CXX=g++
CXXFLAGS=-D$(PLATFORM) -Wall -pedantic -O3

all: XBee.o

XBee.o: XBee.cpp XBee.h Platform.h
	$(CXX) $(CXXFLAGS) -c XBee.cpp

clean:
	rm -f XBee.o
