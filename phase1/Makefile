# This uses environment CXX, CXXFLAGS and LDFLAGS variables. Add them if needed.
all: trader iml

trader: trader.cpp receiver.h
	$(CXX) $(CXXFLAGS) trader.cpp $(LDFLAGS) -o trader

iml: iml.cpp
	$(CXX) $(CXXFLAGS) iml.cpp $(LDFLAGS) -o iml

runpart1: iml trader
	./trader 1 > output.txt &
	sleep 1
	./iml inputs/data1.txt
	sleep 1
	diff -Bw outputs/out1.txt output.txt

runpart2: iml trader
	./trader 2 > output.txt &
	sleep 1
	./iml inputs/data2.txt
	sleep 1
	diff -Bw outputs/out2.txt output.txt

runpart3: iml trader
	./trader 3 > output.txt &
	sleep 1
	./iml inputs/data3.txt
	sleep 1
	diff -Bw outputs/out3.txt output.txt

clean:
	rm -rf output.txt trader iml
	rm -rf *.dSYM
	rm -rf *~
