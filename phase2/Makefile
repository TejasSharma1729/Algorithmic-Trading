CXX=clang++ -std=c++23 -fuse-ld=lld -lpthread -fsanitize=undefined,thread,bounds -g -O3
TRADERFILES=market.cpp trader.cpp main.cpp worker.cpp
BLANKFILES=market.cpp blank_trader.cpp main.cpp worker.cpp

all:
	$(CXX) $(TRADERFILES) -o trader
	$(CXX) $(BLANKFILES) -o market

clean:
	rm -rf market trader output.txt
	rm -rf *.dSYM
	rm -rf *~
