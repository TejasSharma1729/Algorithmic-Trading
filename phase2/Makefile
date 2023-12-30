CXX=g++ -std=c++20 -lpthread -fsanitize=address,undefined -static-libsan
TRADERFILES=market.cpp trader.cpp
BLANKFILES=market.cpp blank_trader.cpp
TWOFILES=main/main2.cpp worker/worker2.cpp
FIVEFILES=main/main5.cpp worker/worker5.cpp
TENFILES=main/main10.cpp worker/worker10.cpp
TWENTYFIVEFILES=main/main25.cpp worker/worker25.cpp
FIFTYFILES=main/main50.cpp worker/worker50.cpp

all: fifty

two:
	$(CXX) $(TWOFILES) $(TRADERFILES) -o trader
	$(CXX) $(TWOFILES) $(BLANKFILES) -o market

five:
	$(CXX) $(FIVEFILES) $(TRADERFILES) -o trader
	$(CXX) $(FIVEFILES) $(BLANKFILES) -o market

ten:
	$(CXX) $(TENFILES) $(TRADERFILES) -o trader
	$(CXX) $(TENFILES) $(BLANKFILES) -o market

twentyfive:
	$(CXX) $(TWENTYFIVEFILES) $(TRADERFILES) -o trader
	$(CXX) $(TWENTYFIVEFILES) $(BLANKFILES) -o market

fifty:
	$(CXX) $(FIFTYFILES) $(TRADERFILES) -o trader
	$(CXX) $(FIFTYFILES) $(BLANKFILES) -o market

market:
	./market > output.txt

trader:
	./trader > output.txt

clean:
	rm -rf market trader output.txt
	rm -rf *.dSYM
	rm -rf *~

.PHONY: market trader
