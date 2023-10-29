#include "receiver.h"
#include <vector>
using namespace std;

template <typename T>
struct node {
	T val;
	node* next;
};
template <typename T>
using ptr = node<T>*;

struct stock {
	string name = "";
	int price = 0;
};

string stringSplit(string& message, vector<string>& splits, int &i) {
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
	return order;
}

int main(int argc, char* argv[]) {
	Receiver rcv;
	sleep(5);
	string message;
	string m = rcv.readIML();
	while (m != "") {
		message += m;
		m = "";
		m = rcv.readIML();
	}
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
		ptr<vector<int>> people = nullptr;
		int peps = 0;
		ptr<string> orders = nullptr;
		int FinalProfit = 0;

		while (k < message.length()) {
			vector<string> splits;
			string order = stringSplit(message, splits, k);
			int n = splits.size();
			auto T = new node<vector<int>>;
			auto U = new node<string>;
			T->val.resize(stocks.size(), 0);
			T->next = people;
			U->val = order;
			U->next = orders;
			people = T;
			orders = U;
			peps++;

			people->val[0] = stoi(splits[n-2]);
			for (int i = 0; i < n-2; i += 2) {
				bool flags = 0;
				for (int j = 1; j < stocks.size(); j++) if (splits[i] == stocks[j].name) {
					flags = 1;
					people->val[j] = stoi(splits[i+1]);
					break;
				}
				if (!flags) {
					stocks.push_back({splits[i], i/2+1});
					T = people;
					while (T != nullptr) { 
						T->val.push_back(0);
						T = T->next;
					}
					people->val[stocks.size()-1] = stoi(splits[i+1]);
				}
			}
			if (splits[n-1][0] == 's') {
				for (int j = 0; j < stocks.size(); j++) people->val[j] *= -1;
			}
			// ADDED ENTRY FOR NEW PERSON

			T = people->next;
			auto prev = people;
			U = orders;
			auto V = U->next;

			while (T != nullptr) { 
				bool flags = 1;
				for (int j = 1; j < stocks.size(); j++) {
					if (T->val[j] != people->val[j]) flags = 0;
				}
				if (flags == 1 && T->val[0] < people->val[0]) {
					prev->next = T->next;
					U->next = V->next;
					delete T;
					delete V;
					T = prev->next;
					peps--;
				}
				else {
					prev = T;
					T = T->next;
					U = V;
				}
				V = U->next;
			}
			// DELETING OUTSTANDING ORDERS OF NET GAIN LESS THAN THIS BUT SAME TRADE.
			
			bool flags = 0;
			int m = stocks.size();
			vector<uint8_t> included(peps, 0); 
			// The person new will always be there. If not then this arbitrage would have been caught earlier.
			included[0] = 1;
			vector<uint8_t> maxIncl(peps, 0);
			int maxProfit = 0;
			
			while (flags == 0) {
				int i = 0;
				vector<int> resultant(m, 0);
				T = people;
				while (T != nullptr) {
					for (int j = 0; j < m; j++) resultant[j] += included[i]*T->val[j];
					T = T->next;
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
				if (i == -1) flags = 1;
				else included[i] = 1;
			}
			// EXPONENTIAL COMPLEXITY PART ABOVE

			if (maxProfit > 0) {
				int i = 0;
				T = people;
				ptr<vector<int>> prev = nullptr;
				U = nullptr;
				V = orders;

				while (T != nullptr) {
					if (maxIncl[i] == 1) {
						peps--;
						if (prev == nullptr) {
							people = people->next;
							orders = orders->next;
							delete T;
							cout << V->val << "#\n";
							delete V;
							T = people;
							V = orders;
						}
						else {
							U->next = V->next;
							prev->next = T->next;
							cout << V->val << "\n";
							delete V;
							delete T;
							T = prev->next;
							V = U->next;
						}
					}
					else {
						prev = T;
						U = V;
						T = T->next;
						V = V->next;
					}
					i++;
				}
				FinalProfit += maxProfit;
			}

			else {
				cout << "No Trade\n";
				T = people->next;
				U = orders;
				V = orders->next;
				auto prev = people;

				while (T != nullptr) { 
					bool flags = 1;
					for (int j = 0; j < stocks.size(); j++) {
						if (T->val[j] + people->val[j] != 0) flags = 0;
					}
					if (flags == 1) {
						prev->next = T->next;
						U->next = V->next;
						delete T;
						delete V;
						prev = people;
						U = orders;
						orders = orders->next;
						people = people->next;
						delete prev;
						delete U;
						peps -= 2;
						break;
					}
					else {
						prev = T;
						T = T->next;
						U = V;
						V = V->next;
					}
				}
				// DELETING OUTSTANDING ORDERS THAT CANCEL EACH OTHER.
			}
		}
		cout << FinalProfit << "\n";
	}

	if (argv[1][0] == '3') {
		// Enter your code here, @YashJonjole!
	}
}
