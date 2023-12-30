CXX=g++ -std=c++20 -lpthread -fsanitize=address,undefined -static-libsan
TRADERFILES=market.cpp trader.cpp main.cpp worker.cpp
BLANKFILES=market.cpp blank_trader.cpp main.cpp worker.cpp

all:
    $(CXX) $(TRADERFILES) -o trader
    $(CXX) $(BLANKFILES) -o market

clean:
    rm -rf market trader output.txt
    rm -rf *.dSYM
    rm -rf *~
