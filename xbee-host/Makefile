PLATFORM=_BOOST
CXX=g++
CXXFLAGS=-D$(PLATFORM) -Wall -pedantic -O3 -I../xbee-api
LDFLAGS=-L/opt/local/lib

main: main.o
	$(CXX) $(LDFLAGS) -o main main.o ../xbee-api/XBee.o -lboost_system

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -f main main.o
