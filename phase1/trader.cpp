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

struct comb_t {
	vector<int> numbers;
	dict<int, int> orders;
	int8_t buy;
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

typedef dict<vector<int>, Pair<dict<int, int>, dict<int, int>>> DICT_T;
typedef vector<Pair<vector<int>, string>> VECT_ORD;
// NOTATION: first == sell orders. Second == buy orders.

int8_t combiSplit(string message, vector<int>& current, vector<string>& names, DICT_T& linComb, VECT_ORD& history, int& price, int& qty) {
	int i = 0;
	string order = "";
	string temp;
	string num;
	while (true)
	{
		temp = "";
		num = "";
		if (message[i] == '-' || (message[i] < 58 && message[i] >= 48)) break;
		while (message[i] != ' ') temp += message[i++]; i++;
		while (message[i] != ' ') num += message[i++]; i++;
		bool flags = 1;
		for (int j = 0; j < names.size(); j++) {
			if (names[j] == temp) {current[j] = stoi(num); flags = 0;}
		}
		if (flags) {
			names.push_back(temp);
			current.push_back(stoi(num));
			for (auto itr = linComb.begin(); !itr.isNull(); itr++) itr.key().push_back(0);
			for (int j = 0; j < history.size(); j++) history[j].first.push_back(0);
		}
		order += temp;
		order += ' ';
		order += num;
		order += ' ';
	}
	num = "";
	temp = "";
	while (message[i] != ' ') num += message[i++]; i++;
	while (message[i] != ' ') temp += message[i++]; i++;
	price = stoi(num);
	qty = stoi(temp);
	int8_t buy = (message[i] == 'b');
	history.push_back({current, order});
	return buy;
}

void buyLowSellHigh(string order, dict<string, stockLowHigh>& stocks) {
	int i = 0;
	string stockName = ""; 
	while (order[i] != ' ') stockName += order[i++];
	i++;
	int price = 0;
	while (order[i] != ' ') price = 10*price + (int)order[i++] - 48; 
	char bs = order[++i];

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
			cout << "No Trade\n";
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

void orderBook(string order, DICT_T& linComb, VECT_ORD& history, vector<string>& names,  int& FinalProfit) {
	vector<int> current(names.size(), 0);
	int price = 0;
	int qty = 0;
	int8_t buySell = combiSplit(order, current, names, linComb, history, price, qty);
	int8_t opposTrans = 1 - buySell;

	if (buySell) {
		auto itr = linComb[current].first.find(price);
		if (!itr.isNull()) {
			if (itr.val() < qty) {
				qty -= itr.val();
				linComb[current].first.remove(itr.key());
			}
			else {
				itr.val() -= qty;
				if (itr.val() == 0) linComb[current].first.remove(price);
				cout << "No Trade\n";
				return;
			}
		}
		linComb[current].second[price] += qty;
	}
	else {
		auto itr = linComb[current].second.find(price);
		if (!itr.isNull()) {
			if (itr.val() < qty) {
				qty -= itr.val();
				linComb[current].second.remove(itr.key());
			}
			else {
				itr.val() -= qty;
				if (itr.val() == 0) linComb[current].second.remove(price);
				cout << "No Trade\n";
				return;
			}
		}
		linComb[current].first[price] += qty;
	}

	vector<dict<int, int>> profits(linComb.size());
	vector<Pair<int, int>> limits(linComb.size());
	auto itr = linComb.begin();
	for (int i = 0; i < linComb.size(); i++)
	{
		auto Dict_J = itr.val().first;
		auto Dict_K = itr.val().second;
		int x = 0;
		// NOTATION: x < 0 means something we take buy orders. x > 0 means something we take sell orders.

		while (true) {
			profits[i][x] = 0;
			auto ktr = Dict_K.end();
			int y = 0;
			while (!ktr.isNull()) {
				if (y + ktr.val() <= -x) {
					y += ktr.val();
					profits[i][x] += ktr.val()*ktr.key();
					--ktr;
				} else break;
			}
			if (ktr.isNull()) {
				break;
			}
			auto jtr = Dict_J.begin();
			int kIn = y + ktr.val() + x;
			int jIn = 0;
			while (!jtr.isNull() && !ktr.isNull() && jtr.key() >= ktr.key()) {
				if (kIn == ktr.val()) {
					ktr--; kIn = 0;
				} else kIn++;
				if (jIn == jtr.val()) {
					jtr++; jIn = 0;
				} else jIn++;
				profits[i][x] += jtr.key() - ktr.key();
			}
			x--;
		}
		limits[i].first = x;
		x = 1;
		if (!Dict_J.empty()) {
			while (true) {
				profits[i][x] = 0;
				auto jtr = Dict_J.begin();
				int y = 0;
				while (!jtr.isNull()) {
					if (y + jtr.val() <= x) {
						y += jtr.val();
						profits[i][x] -= jtr.key()*jtr.val();
						++jtr;
					} else break;
				}
				if (jtr.isNull()) {
					break;
				}
				auto ktr = Dict_K.end();
				int jIn = y + jtr.val() - x;
				int kIn = 0;
				while ( jtr.key() >= ktr.key()) {
					if (kIn == ktr.val()) {
						ktr--; kIn = 0;
					} else kIn++;
					if (jIn == jtr.val()) {
						jtr++; jIn = 0;
					} else jIn++;
					profits[i][x] += jtr.key() - ktr.key();
				}
				x++;
			}
			limits[i].second = x;
		}
		else limits[i].second = 0;
		itr++;
	}

	// The Main Exponential Part begins here.
	vector<int> finale(linComb.size(), 0);
	vector<int> thisComb(linComb.size(), 0);
	for (int i = 0; i < linComb.size(); i++) thisComb[i] = limits[i].first;
	bool flags = 1;
	int mainProfit = 0;

	while (flags) {
		vector<int> implication(names.size(), 0);
		int impliedProfit = 0;

		int i = 0;
		auto itr = linComb.begin();
		while (i < linComb.size())
		{
			impliedProfit += profits[i][thisComb[i]];
			for (int j = 0; j < names.size(); j++) implication[j] += thisComb[i]*itr.key()[j];
			++itr; i++;
		}
		for (int j = 0; j < names.size(); j++) if (implication[j] != 0) flags = 0;
		if (flags && mainProfit < impliedProfit) {
			mainProfit = impliedProfit;
			finale = thisComb;
		}
		i = linComb.size() - 1;
		while (i >= 0 && thisComb[i] == limits[i].second) {
			thisComb[i--] = limits[i].first;
		}
		if (i < 0) flags = 0;
		else thisComb[i]++;
	}

	// Cleanup and printing
	if (mainProfit <= 0) {
		cout << "No Trade\n";
		return;
	}
	FinalProfit += mainProfit;
	for (int i = history.size() - 1; i >= 0; i--) {
		auto itr = linComb.begin();
		int j = 0;
		while (!itr.isNull()) 
		{
			if (itr.key() == history[i].first && finale[j] != INT_MIN) {
				auto Dict_J = itr.val().first;
				auto Dict_K = itr.val().second;

				if (finale[j] < 0) {
					auto ktr = Dict_K.end();
					int y = 0;
					while (!ktr.isNull()) {
						if (y + ktr.val() <= -finale[j]) {
							y += ktr.val();
							cout << history[i].second << ktr.key() << " " << ktr.val() << " s#\n";
							auto ltr = ktr;
							--ktr;
							Dict_K.remove(ltr.key());
						} else {
							cout << history[i].second << ktr.key() << " " << (-finale[j] - y) << " s#\n";
							Dict_K[ktr.key()] -= (-finale[j] - y);
							break;
						}
					}
					auto jtr = Dict_J.begin();
					if (!ktr.isNull()) {
						int kIn = 0;
						int jIn = 0;
						while (!jtr.isNull() && !ktr.isNull() && jtr.key() >= ktr.key()) {
							if (kIn == ktr.val()) {
								auto ltr = ktr;
								ktr--;
								cout << history[i].second << ltr.key() << " " << ltr.val() << " s#\n";
								Dict_K.remove(ltr.key());
							} else kIn++;
							if (jIn == jtr.val()) {
								auto ltr = jtr;
								jtr++;
								cout << history[i].second << ltr.key() << " " << ltr.val() << " b#\n";
								Dict_J.remove(ltr.key());
							} else jIn++;
						}
						if (!jtr.isNull()) {
							cout << history[i].second << jtr.key() << " " << jIn << " b#\n";
							jtr.val() -= jIn;
						}
						if (!ktr.isNull()) {
							cout << history[i].second << ktr.key() << " " << kIn << " b#\n";
							ktr.val() -= kIn;
						}
					}
				}
				else if (finale[j] > 0) {
					auto jtr = Dict_J.begin();
					int y = 0;
					while (!jtr.isNull()) {
						if (y + jtr.val() <= finale[j]) {
							y += jtr.val();
							cout << history[i].second << jtr.key() << " " << jtr.val() << " b#\n";
							auto ltr = jtr;
							++jtr;
							Dict_J.remove(ltr.key());
						} else {
							cout << history[i].second << jtr.key() << " " << (finale[j] - y) << " b#\n";
							Dict_J[jtr.key()] -= (finale[j] - y);
							break;
						}
					}
					auto ktr = Dict_K.end();
					if (!jtr.isNull()) {
						int kIn = 0;
						int jIn = 0;
						while (!jtr.isNull() && !ktr.isNull() && jtr.key() >= ktr.key()) {
							if (kIn == ktr.val()) {
								auto ltr = ktr;
								ktr--;
								cout << history[i].second << ltr.key() << " " << ltr.val() << " s#\n";
								Dict_K.remove(ltr.key());
							} else kIn++;
							if (jIn == jtr.val()) {
								auto ltr = jtr;
								jtr++;
								cout << history[i].second << ltr.key() << " " << ltr.val() << " b#\n";
								Dict_J.remove(ltr.key());
							} else jIn++;
						}
						if (!jtr.isNull()) {
							cout << history[i].second << jtr.key() << " " << jIn << " b#\n";
							jtr.val() -= jIn;
						}
						if (!ktr.isNull()) {
							cout << history[i].second << ktr.key() << " " << kIn << " b#\n";
							ktr.val() -= kIn;
						}
					}
				}
				else {
					auto jtr = Dict_J.begin();
					auto ktr = Dict_K.end();
					int kIn = 0;
					int jIn = 0;
					while (!jtr.isNull() && !ktr.isNull() && jtr.key() >= ktr.key()) {
						if (kIn == ktr.val()) {
							auto ltr = ktr;
							ktr--;
							cout << history[i].second << ltr.key() << " " << ltr.val() << " s#\n";
							Dict_K.remove(ltr.key());
						} else kIn++;
						if (jIn == jtr.val()) {
							auto ltr = jtr;
							jtr++;
							cout << history[i].second << ltr.key() << " " << ltr.val() << " b#\n";
							Dict_J.remove(ltr.key());
						} else jIn++;
					}
					if (!jtr.isNull()) {
						cout << history[i].second << jtr.key() << " " << jIn << " b#\n";
						jtr.val() -= jIn;
					}
					if (!ktr.isNull()) {
						cout << history[i].second << ktr.key() << " " << kIn << " b#\n";
						ktr.val() -= kIn;
					}
				}
				finale[j] = INT_MIN;
				break;
			}
			itr++;
			j++;
		}
	}
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
	VECT_ORD history;
	DICT_T linComb;
	vector<string> names;

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
				else if (argv[1][0] == '3') orderBook(order, linComb, history, names, FinalProfit); // You can add parameters
				else cout << order << "\n";
			}
			order = "";
			while (i < message.length() && (message[i] == 13 || message[i] == 0 || message[i] == 10)) i++;
			if (i == message.length()) {
				message = "";
				continue;
			}
			continue;
		}
		message = rcv.readIML();
		i = 0;
	}
	if (argv[1][0] == '2' || argv[1][0] == '3') cout << FinalProfit << "\n";
	rcv.terminate();
}
