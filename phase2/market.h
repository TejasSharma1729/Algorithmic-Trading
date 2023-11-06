#ifndef MARKET_H
#define MARKET_H
#include <string>
#include <vector>
#include <iostream>
#include "../heap.h"
#include "../dict.h"
using namespace std;
class market
{
	struct order {
		int line;
		int timeIn;
		int timeExp;
		int price;
		string person;
		int qty;
		order(): line(0), timeIn(0), price(0), person(""), qty(0) {}

		bool operator < (const order& B) const {
			if (price < B.price) return true;
			else if (price > B.price) return false;
			if (timeIn < B.timeIn) return true;
			else if (timeIn > B.timeIn) return false;
			if (person < B.person) return true;
			else return false;
		}
		bool operator == (const order& B) const {
			return (line == B.line && timeIn == B.timeIn && timeExp == B.timeExp && price == B.price && person == B.person && qty == B.qty);
		}
	};

	struct stock {
		vector<string> name;
		vector<int> quant;
		dict<order, int> bookBuy;
		dict<int, dict<order, bool>> timeExpBuy;
		dict<order, int> bookSell;
		dict<int, dict<order, bool>> timeExpSell;

		void printname(int qty) {
			int n = name.size() - 1;
			for (int i = 0; i < n; i++) {
				cout << qty*quant[i] << " shares of " << name[i] << " and ";
			}
			cout << qty*quant[n] << " shares of " << name[n];
		}

		void expire(int t) {
			dict<order, bool> out = timeExpBuy[t];
			for (auto itr = out.begin(); !itr.isNull(); itr++) {
				bookBuy.remove(itr.key());
			}
			out.obliterate();
			out = timeExpSell[t];
			for (auto itr = out.begin(); !itr.isNull(); itr++) {
				bookSell.remove(itr.key());
			}
			out.obliterate();
			timeExpBuy.remove(t);
			timeExpSell.remove(t);
			return;
		}

		void insert(order data, int exp, bool sell) {
			exp += data.timeIn;
			data.timeExp = exp;
			if (!sell) {
				bookBuy[data] = exp;
				timeExpBuy[exp][data] = false;
			}
			else {
				bookSell[data] = exp;
				timeExpSell[exp][data] = false;
			}
		}

		void remove(order data, bool sell) {
			if (!sell) {
				int exp = bookBuy[data];
				timeExpBuy[exp].remove(data);
				bookBuy.remove(data);
			}
			else {
				int exp = bookSell[data];
				timeExpSell[exp].remove(data);
				bookSell.remove(data);
			}
		}

		void transact() {
			vector<order> bought;
			vector<order> sold;
			auto itr = bookBuy.end();
			auto jtr = bookSell.end();
			while (!itr.isNull() && !jtr.isNull()) {
				if (itr.key().price >= jtr.key().price) {
					int q1 = itr.key().qty;
					int q2 = jtr.key().qty;
					int price;
					if (jtr.key().line > itr.key().line) price = itr.key().price; else price = jtr.key().price;
					if (q2 > q1) {
						cout << itr.key().person << " purchased ";
						printname(q1);
						cout << " from " << jtr.key().person << " for $" << price << "/share\n";
						bought.push_back(itr.key());
						auto ktr = timeExpSell.find(jtr.val());
						int x = jtr.key().qty - q1;
						ktr.val().find(jtr.key()).key().qty = x;
						jtr.key().qty = x;
						--itr;
					}
					else if (q1 > q2) {
						cout << itr.key().person << " purchased ";
						printname(q2);
						cout << " from " << jtr.key().person << " for $" << price << "/share\n";
						sold.push_back(jtr.key());
						auto ktr = timeExpBuy.find(itr.val());
						int x = itr.key().qty - q2;
						ktr.val().find(itr.key()).key().qty = x;
						itr.key().qty = x;
						--jtr;
					}
					else {
						cout << itr.key().person << " purchased ";
						printname(q1);
						cout << " from " << jtr.key().person << " for $" << price << "/share\n";
						bought.push_back(itr.key());
						sold.push_back(jtr.key());
						--itr;
						--jtr;
					}
				}
				else --jtr;
			}
			int n = bought.size();
			for (int i = 0; i < n; i++) {
				remove(bought[i], 0);
			}
			n = sold.size();
			for (int i = 0; i < n; i++) {
				remove(sold[i], 1);
			}
		}

		bool operator == (const stock& B) const {
			if (name.size() != B.name.size()) return false;
			for (int i = 0; i < name.size(); i++) if (name[i] != B.name[i] || quant[i] != B.quant[i]) return false;
			return true;
		}
		bool operator < (const stock& B) const {
			int i = 0;
			while (true) {
				if (i == name.size()) return (B.name.size() > name.size());
				if (i == B.name.size()) return false;
				if (name[i] < B.name[i]) return true;
				else if (name[i] == B.name[i]) {
					if (quant[i] == B.quant[i]) {
						i++;
						continue;
					}
					return (quant[i] < B.quant[i]);
				}
				else return false;
			}
		}
	};
	dict<stock, bool> stocks;

	int split(string message, int prevT);

public:
	market(int argc, char** argv);
	void start();
private:
};
#endif
