#include "receiver.h"
#include <vector>
using namespace std;

struct stock {
	string name = "";
	int price = 0;
};

int stringSplit(string& message, vector<string>& splits, vector<string>& orders, int i) {
	string temp;
	string order;
	bool flags = 1;
	while (flags)
	{
		temp = "";
		while (message[i] != ' ' && message[i] != '\n' && message[i] != '$') {
			temp += message[i];
			if (message[i] != '#') order += message[i];
			i++;
		}
		if (temp != "") splits.push_back(temp);
		if (message[i] == '\n' || message[i] == '$') flags = 0; else order += ' ';
		i++;
	}
	if (order[order.length() - 1] == 'b') order[order.length() - 1] = 's';
	else order[order.length() - 1] = 'b';
	orders.push_back(order);
	return i;
}

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
	
	if (argv[1][0] == '2') {
		// Exponential Algorithm
		vector<stock> stocks(1, {"", 0}); // 1-indexed
		int k = 0;
		vector<vector<int>> people;
		vector<string> orders;

		while (k < message.length()) {
			vector<string> splits;
			k = stringSplit(message, splits, orders, k);
			int n = splits.size();
			people.push_back(vector<int>(stocks.size(), 0));
			people.back()[0] = stoi(splits[n-2]);
			for (int i = 0; i < n-2; i += 2) {
				bool flags = 0;
				for (int j = 1; j < stocks.size(); j++) if (splits[i] == stocks[j].name) {
					flags = 1;
					people[people.size()-1][j] = stoi(splits[i+1]);
					break;
				}
				if (!flags) {
					stocks.push_back({splits[i], i/2+1});
					for (int j = 0; j < people.size(); j++) people[j].push_back(0);
					people[people.size()-1][stocks.size()-1] = stoi(splits[i+1]);
				}
			}
			if (splits[n-1][0] == 's') {
				for (int j = 0; j < stocks.size(); j++) people[people.size()-1][j] *= -1;
			}
		}
		int n = people.size();
		int m = stocks.size();
		vector<uint8_t> included(n, 0);
		bool flags = 1;
		vector<uint8_t> maxIncl;
		int maxProfit = 0;

		while (flags) {
			int i = 0;
			vector<int> resultant(m, 0);
			while (i < n) {
				for (int j = 0; j < m; j++) resultant[j] += included[i]*people[i][j];
				i++;
			}
			bool acceptable = 1;
			for (int j = 1; j < m; j++) if (resultant[j] != 0) acceptable = 0;
			if (acceptable) {
				if (resultant[0] > maxProfit) {
					maxIncl = included;
					maxProfit = resultant[0];
				}
			}
			i--;
			while (i >= 0 && included[i] != 0) included[i--] = 0;
			if (i == -1) flags = 0;
			else included[i] = 1;
		}

		vector<int> resultant(m, 0);
		for (int i = 0; i < n; i++)
		{
			if (maxIncl[i] == 0) {cout << "No Trade\n"; continue;}
			bool flags = 1;
			for (int j = 1; j < m; j++) {resultant[j] += people[i][j]; if (resultant[j] != 0) flags = 0;}
			if (flags) {
				for (int j = i; j >= 0; j--) if (maxIncl[j] == 1) {
					cout << orders[j] << "#\n";
					maxIncl[j] = 0;
				}
			}
			else cout << "No Trade\n";
		}
		cout << maxProfit << "\n";
	}

	if (argv[1][0] == '3') {
		// Enter your code here, @YashJonjole!
	}
}
