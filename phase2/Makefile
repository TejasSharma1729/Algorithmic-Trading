# Uses Environment CXX and CXXFLAGS and LDFLAGS. Add variables as per choice.
TRADERFILES=market.cpp trader.cpp main.cpp worker.cpp
BLANKFILES=market.cpp blank_trader.cpp main.cpp worker.cpp

all:
	$(CXX) $(CXXFLAGS) $(BLANKFILES) $(LDFLAGS) -lpthread -o market &
	$(CXX) $(CXXFLAGS) $(TRADERFILES) $(LDFLAGS) -lpthread -o trader

trader:
	$(CXX) $(CXXFLAGS) $(TRADERFILES) $(LDFLAGS) -lpthread -o trader

market:
	$(CXX) $(CXXFLAGS) $(BLANKFILES) $(LDFLAGS) -lpthread -o market

trader1: trader
	./trader 1 > output.txt
trader2: trader
	./trader 2 > output.txt
trader3: trader
	./trader 3 > output.txt
trader4: trader
	./trader 4 > output.txt
trader5: trader
	./trader 5 > output.txt

market1: market
	./market 1 > output.txt
market2: market
	./market 2 > output.txt
market3: market
	./market 3 > output.txt
market4: market
	./market 4 > output.txt
market5: market
	./market 5 > output.txt

clean:
	rm -rf market trader output.txt
	rm -rf *.dSYM
