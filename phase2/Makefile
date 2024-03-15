# Uses Environment CXX and CXXFLAGS and LDFLAGS. Add variables as per choice.
TRADERFILES=market.cpp trader.cpp main.cpp worker.cpp
BLANKFILES=market.cpp blank_trader.cpp main.cpp worker.cpp

all:
	$(CXX) $(CXXFLAGS) $(BLANKFILES) $(LDFLAGS) -lpthread -o market &
	$(CXX) $(CXXFLAGS) $(TRADERFILES) $(LDFLAGS) -lpthread -o trader

clean:
	rm -rf market trader output.txt
	rm -rf *.dSYM
