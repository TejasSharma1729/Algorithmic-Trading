#include "receiver.h"
#include <vector>
#include "../dict.h"
#include "../heap.h"
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

struct stockLowHigh {
	int price = 0;
	int buy = 0;
	int sell = INT_MAX;
};

string stringSplit(string message, vector<string>& splits) {
	int i = 0;
	string temp;
	string order = "";
	bool flags = 1;
	while (flags)
	{
		temp = "";
		while (message[i] != ' ' && message[i] != '#') {
			temp += message[i];
			order += message[i];
			i++;
		}
		if (temp != "") splits.push_back(temp);
		if (message[i] == '#') {
			order += '#';
			flags = 0;
		}
		else {
			order += ' ';
		}
		i++;
	}
	if (order[order.length() - 2] == 'b') order[order.length() - 2] = 's';
	else order[order.length() - 2] = 'b';
	return order;
}

void buyLowSellHigh(string order, dict<string, stockLowHigh>& stocks) {
	int i = 0;
	string stockName = ""; 
	//cerr << order << "\n";
	while (order[i] != ' ') stockName += order[i++];
	i++;
	int price = 0;
	while (order[i] != ' ') price = 10*price + (int)order[i++] - 48; 
	char bs = order[++i];
	while (order[i] != '\n' && i != order.length()) i++; i++;

	if (!stocks.find(stockName).isNull()) {
		stockLowHigh& j = stocks[stockName];

		if (bs == 'b') {
			if (j.price >= price) {
				cout << "No Trade\n";
				if (j.buy < price) {
					j.buy = price;
				}
			}
			else if (j.sell == price) {
				cout << "No Trade\n";
				j.sell = INT_MAX;
			}
			else if (j.buy > price) cout << "No Trade\n";
			else {
				cout << stockName << " " << price << " s\n";
				j.price = price;
			}
		}
		else {
			if (j.price <= price) {
				cout << "No Trade\n";
				if (j.sell > price) {
					j.sell = price;
				}
			}
			else if (j.buy == price) {
				cout << "No Trade\n";
				j.buy = 0;
			}
			else if (j.sell < price) cout << "No Trade\n";
			else {
				cout << stockName << " " << price << " b\n";
				j.price = price;
			}
		}
	}   
	else {
		stocks[stockName].price = price;
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

void arbitrage(string message, vector<stock>& stocks, ptr<vector<int>>& people, ptr<string>& orders, int& FinalProfit, int& peps) {
	
	vector<string> splits;
	string order = stringSplit(message, splits);
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
		bool cancel = 1;
		for (int j = 0; j < stocks.size(); j++) {
			if (j != 0 && T->val[j] != people->val[j]) flags = 0;
			if (T->val[j] + people->val[j] != 0) cancel = 0;
		}
		if (cancel == 1) {
			cerr << "No Trade\n";
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
			return;
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
					cout << V->val << "\n";
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
	}
}

void orderBook(string order) {
	// @YashJonjale, your code should be here. Add any parameters as per your choice, preferably pass by reference
	return;
}

int main(int argc, char* argv[]) {
	// For all parts
	Receiver rcv;
	int lineCount = 0;
	int i = 0;
	string message = rcv.readIML();
	string order = "";
	bool flags = 1;

	// For part 1 -- Buy Low, Sell High
	dict<string, stockLowHigh> stocksLowHigh;

	// For part 2 -- Arbitrage
	vector<stock> stocksArbitrage(1, {"", 0});
	ptr<vector<int>> people = nullptr;
	ptr<string> orders = nullptr;
	int FinalProfit = 0;
	int peps = 0;
	
	// For part 3 -- Order Book Processing
	int x = 0;

	while (flags) {
		if (message == "") {
			message = rcv.readIML();
			i = 0;
			continue;
		}
		
		while (i < message.length() && (message[i] != 13 && message[i] != 0 && message[i] != 10)) order += message[i++];
		if (message[i] != 13 && message[i] != 10 && message[i] != 0) {
			message = "";
			continue;
		}
		if (message[i-1] == '$') {
			flags = 0;
			if (message[i-2] == 13 || message[i-2] == 0 || message[i-2] == 10) break;
		}
		if (message[i] == 13 || message[i] == 0 || message[i] == 10) 
		{
			if (order != "") {
				if (argv[1][0] == '1') buyLowSellHigh(order, stocksLowHigh);
				else if (argv[1][0] == '2') arbitrage(order, stocksArbitrage, people, orders, FinalProfit, peps);
				else if (argv[1][0] == '3') orderBook(order); // You can add parameters
				else cout << order << "\n";
			}
			order = "";
			while (i < message.length() && (message[i] == 13 || message[i] == 0 || message[i] == 10)) i++;
			if (i == message.length()) {
				message = "";
				continue;
			}
			x++;
			continue;
		}
		message = rcv.readIML();
		i = 0;
	}
	if (argv[1][0] == '2') cout << FinalProfit << "\n";
	rcv.terminate();
}
