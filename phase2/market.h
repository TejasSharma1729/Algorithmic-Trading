#ifndef MARKET_H
#define MARKET_H
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <climits>
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

		bool operator == (const order& B) const {
			return (line == B.line && timeIn == B.timeIn && timeExp == B.timeExp && 
                    price == B.price && person == B.person && qty == B.qty);
		}

        std::strong_ordering operator <=> (const order& B) const
        {
            if (price != B.price) return (price <=> B.price);
            if (timeIn != B.timeIn) return (timeIn <=> B.timeIn);
            if (person != B.person) return (person <=> B.person);
            if (timeExp != B.timeExp) return (timeExp <=> B.timeExp);
            if (line != B.line) return (line <=> B.line);
            return (qty <=> B.qty);
        }
	};

    /*
    bool operator == (const order& A, const order& B) {
		return (A.line == B.line && A.timeIn == B.timeIn && A.timeExp == B.timeExp && 
                    A.price == B.price && A.person == B.person && A.qty == B.qty);
    }

    auto operator <=> (const order& A, const order& B) {
            if (A.price != B.price) return (A.price <=> B.price);
            if (A.timeIn != B.timeIn) return (A.timeIn <=> B.timeIn);
            if (A.person != B.person) return (A.person <=> B.person);
            if (A.timeExp != B.timeExp) return (A.timeExp <=> B.timeExp);
            if (A.line != B.line) return (A.line <=> B.line);
            return (A.qty <=> B.qty);
    }
    */

	struct personOrders {
		int bought = 0;
		int sold = 0;
		long obtained = 0;
	};
	dict<string, personOrders> peoples;
	long netExchange;
	int numTrades;
	long numSharedTrades;

	struct stock {
		vector<string> name;
		vector<int> quant;
		int numTransacted;
		dict<order, int> bookBuy;
		dict<int, dict<order, bool>> timeExpBuy;
		dict<order, int> bookSell;
		dict<int, dict<order, bool>> timeExpSell;

		void printname(int qty) {
			int n = (int)name.size() - 1;
			for (int i = 0; i < n; i++) {
				cout << qty*quant[i] << " share of " << name[i] << " and ";
			}
			cout << qty*quant[n] << " share of " << name[n];
		}

		void expire(int t) {
			dict<order, bool> outB = timeExpBuy[t];
			for (auto itr = outB.begin(); !itr.isNull(); itr++) {
				bookBuy.remove(itr.key());
				timeExpBuy[t].remove(itr.key());
			}
			dict<order, bool> outS = timeExpSell[t];
			for (auto itr = outS.begin(); !itr.isNull(); itr++) {
				bookSell.remove(itr.key());
				timeExpSell[t].remove(itr.key());
			}
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
				int exp = data.timeExp;
				timeExpBuy[exp].remove(data);
				bookBuy.remove(data);
			}
			else {
				int exp = data.timeExp;
				timeExpSell[exp].remove(data);
				bookSell.remove(data);
			}
		}

		void transact(dict<string, personOrders>& peoples_parent, 
                long& netExchange_parent, int& numTrades_parent, long& numSharedTrades_parent) {
			vector<order> bought;
			vector<order> sold;
			auto itr = bookBuy.end();
			auto jtr = bookSell.end();
			while (!itr.isNull() && !jtr.isNull()) {
				if (itr.key().price >= jtr.key().price) 
				{
					int q1 = itr.key().qty;
					int q2 = jtr.key().qty;
					int price;
					if (jtr.key().line > itr.key().line) 
                        price = itr.key().price; else price = jtr.key().price;

					if (q2 > q1) {
						cout << itr.key().person << " purchased ";
						printname(q1);
						cout << " from " << jtr.key().person << 
                            " for $" << 1.0*price/numTransacted << "/share\n";
						bought.push_back(itr.key());

						auto& A = peoples_parent[itr.key().person];
						A.bought += numTransacted*q1;
						A.obtained -= price*q1;
						auto& B = peoples_parent[jtr.key().person];
						B.sold += numTransacted*q1;
						B.obtained += price*q1;
						netExchange_parent += price*q1;
						numTrades_parent++;
						numSharedTrades_parent += numTransacted*q1;

						auto ktr = timeExpSell.find(jtr.val());
						
						
						int x = jtr.key().qty - q1;
						ktr.val().find(jtr.key()).key().qty = x;
						jtr.key().qty = x;
						
						--itr;
					}
					
					else if (q1 > q2) {
						cout << itr.key().person << " purchased ";
						printname(q2);
						cout << " from " << jtr.key().person << 
                            " for $" << 1.0*price/numTransacted << "/share\n";
						sold.push_back(jtr.key());

						auto& A = peoples_parent[itr.key().person];
						A.bought += numTransacted*q2;
						A.obtained -= price*q2;
						auto& B = peoples_parent[jtr.key().person];
						B.sold += numTransacted*q2;
						B.obtained += price*q2;
						netExchange_parent += price*q2;
						numTrades_parent++;
						numSharedTrades_parent += numTransacted*q2;

						auto ktr = timeExpBuy.find(itr.val());
						int x = itr.key().qty - q2;
						ktr.val().find(itr.key()).key().qty = x;
						itr.key().qty = x;
						
						--jtr;
					}
					else {
						cout << itr.key().person << " purchased ";
						printname(q1);
						cout << " from " << jtr.key().person << 
                            " for $" << 1.0*price/numTransacted << "/share\n";

						auto& A = peoples_parent[itr.key().person];
						A.bought += numTransacted*q1;
						A.obtained -= price*q1;
						auto& B = peoples_parent[jtr.key().person];
						B.sold += numTransacted*q1;
						B.obtained += price*q1;
						netExchange_parent += price*q1;
						numTrades_parent++;
						numSharedTrades_parent += numTransacted*q1;

						bought.push_back(itr.key());
						sold.push_back(jtr.key());
						--itr;
						--jtr;
					}
				}
				else --jtr;
			}
			int n = (int)bought.size();
			for (int i = 0; i < n; i++) {
				remove(bought[i], 0);
			}
			n = (int)sold.size();
			for (int i = 0; i < n; i++) {
				remove(sold[i], 1);
			}
		}

		bool operator == (const stock& B) const {
			if (name.size() != B.name.size()) return false;
			for (int i = 0; i < (int)name.size(); i++) 
                if (name[i] != B.name[i] || quant[i] != B.quant[i]) 
                    return false;
			return true;
		}

        std::strong_ordering operator <=> (const stock& B) const {
            int i = 0;
            while (true) {
                if (i == (int)name.size() || i == (int)B.name.size()) 
                    return (name.size() <=> B.name.size());
                if (name[i] != B.name[i]) return (name[i] <=> B.name[i]);
                if (quant[i] != B.quant[i]) return (quant[i] <=> B.quant[i]);
                i++;
            }
        }
	};
	dict<stock, bool> stocks;

	int split(string message, int prevT, int line);

public:
	market(int argc, char** argv);
	void start();
private:
};
#endif
