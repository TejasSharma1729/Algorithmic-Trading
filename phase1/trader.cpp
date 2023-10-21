#include "receiver.h"
#include <vector>
using namespace std;

struct stock {
	string name = "";
	int price = 0;
};

int main(int argc, char* argv[]) {
	Receiver rcv;
	sleep(5);
	string message;
	message = rcv.readIML();
	for (int i = 0; i < message.length(); i++) if (message[i] < 32) message[i] = '\n';

	if (argv[1][0] == '1') {
		string stockName;
		int price;
		char bs;
		vector<stock> stocks;
		int i = 0;

		while (i < message.length()) {
    	    stockName = ""; 
    	    while (message[i] != ' ') stockName += message[i++];
    	    i++;
    	    price = 0;
    	    while (message[i] != ' ') price = 10*price + (int)message[i++] - 48; 
    	    bs = message[++i];
			while (message[i] != '\n' && i != message.length()) i++; i++;
    	    int n = stocks.size();

    	    for (int j = 0; j < stocks.size(); j++) {
    	        if (stocks[j].name == stockName) {
    	            if (stocks[j].price < price && bs == 'b') {
    	                stocks[j].price = price;
    	                cout << stockName << " " << price << " s\n";
    	            }   
    	            else if (stocks[j].price > price && bs == 's') {
    	                stocks[j].price = price;
    	                cout << stockName << " " << price << " b\n";
    	            }   
    	            else {
						cout << "No Trade\n";
					}
    	            n = j;
    	            break;
    	        }   
    	    }   
    	    if (n == stocks.size()) {
    	        stocks.push_back({stockName, price});
    	        cout << stockName << " ";
    	        cout << price << " ";
				if (bs == 'b') {
					cout << "s\n";
				}
				else {
					cout << "b\n";
				}
    	    }   
    	}
	}
}
