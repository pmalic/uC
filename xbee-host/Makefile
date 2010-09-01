PLATFORM=_BOOST
CXX=g++
CXXFLAGS=-D$(PLATFORM) -Wall -pedantic -O3 -I..
LDFLAGS=-L/opt/local/lib -lboost_system -lboost_thread-mt

all: at sensor collector

at: at.o
	$(CXX) $(LDFLAGS) -o at at.o ../xbee/XBee.o

at.o: at.cpp
	$(CXX) $(CXXFLAGS) -c at.cpp

sensor: sensor.o
	$(CXX) $(LDFLAGS) -o sensor sensor.o ../xbee/XBee.o

sensor.o: sensor.cpp
	$(CXX) $(CXXFLAGS) -c sensor.cpp

collector: collector.o
	$(CXX) $(LDFLAGS) -o collector collector.o ../xbee/XBee.o

collector.o: collector.cpp
	$(CXX) $(CXXFLAGS) -c collector.cpp

clean:
	rm -f at at.o sensor sensor.o collector collector.o

