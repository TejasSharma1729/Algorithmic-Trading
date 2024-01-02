// This is your trader. Place your orders from here

#include <iostream>
#include <fstream>
#include <climits>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include "../dict.h"
using namespace std;

extern std::mutex printMutex;
extern std::atomic<int> commonTimer;
extern int NUM_THREADS;

// THIS IS THE PUBLIC NAMESPACE -- FOR COMMON VARIABLES.
namespace Trader {
    struct dynamicMedian {
        vector<int> left;
        vector<int> right;
        int mid = INT_MIN;

        void swap(int& a, int& b) {
            int temp = a;
            a = b;
            b = temp;
        }

        bool empty() {
            if (mid != INT_MIN) return 0;
            return (left.empty());
        }
        int median() {
            if (mid == INT_MIN) {
                if (left.size() == 0) return INT_MIN;
                return (left[0] + right[0])/2;
            }
            else return mid;
        }
        void insert(int x) {
            if (mid == INT_MIN) {mid = x; return;}
            int s = left.size();
            int t = right.size();
            if (x >= mid) {
                left.push_back(mid);
                right.push_back(x);
            } else {
                left.push_back(x);
                right.push_back(mid);
            }
            mid = INT_MIN;

            while (s > 0) {
                if (left[s] > left[(s-1)/2]) {
                    swap(left[s], left[(s-1)/2]);
                    s = (s-1)/2;
                } else break;
            }
            while (t > 0) {
                if (right[t] < right[(t-1)/2]) {
                    swap(right[t], right[(t-1)/2]);
                    t = (t-1)/2;
                } else break;
            }
        }
    };
    struct stockStruct {
        vector<string> name;
        vector<int> quant;
        bool operator == (const stockStruct& B) const {
			if (name.size() != B.name.size()) return false;
			for (int i = 0; i < (int)name.size(); i++) if (name[i] != B.name[i] || quant[i] != B.quant[i]) return false;
			return true;
		}
		bool operator < (const stockStruct& B) const {
			int i = 0;
			while (true) {
				if (i == (int)name.size()) return (B.name.size() > name.size());
				if (i == (int)B.name.size()) return false;
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
    dict<stockStruct, dynamicMedian> medianPrices;
    dict<string, int> withMe;
    dict<stockStruct, Pair<dict<int, dict<int, int>>, dict<int, dict<int, int>>>> combinations;
    dict<stockStruct, dict<int, Pair<dict<int, int>, dict<int, int>>>> timeWise;
    // Notation: first == what WE BUY. second == what WE SELL

    long seekPos = 0;
};

// BELOW FUNCTION IS TO ACT ON ONE ORDER AT A TIME AND TO GENERATE OUTPUT APT-LY.
int trader(std::string *message)
{
    string order = message[0];
    int currentTime = commonTimer.load();
    string outPut = to_string(currentTime) + " 22B0909_22B0990 ";
    string otherPut = outPut;
    std::lock_guard<std::mutex> lock(printMutex);
    int i = 0;
    while (true) {
        if (order[i] >= 48 && order[i] < 58) i++;
        else {i++; break;}
    }
	string name = "";
    while (order[i] != ' ') name += order[i++]; i++;
	if (name == "22B0909_22B0990") return 0;

    int8_t buyOrd = 0;

    if (order[i] == 'S') {
        outPut += "BUY "; i += 5;
        otherPut += "SELL ";
        buyOrd = 1;
    } 
    else if (order[i] == 'B') {
        outPut += "SELL "; i += 4;
        otherPut += "BUY ";
        buyOrd = -1;
    } 
    else throw invalid_argument("[" + to_string(i) + "] == " + order[i]);

    int j = i;
    while (order[j] != '$') j++;
    j++;

    int price;
    string num = "";
    while (order[j] != ' ') num += order[j++];
    price = stoi(num);
    j += 2;

    int quantity;
    num = "";
    while (order[j] != ' ') num += order[j++];
    quantity = stoi(num);
    num = ""; 
    j++;

    int retTime;
    while (j < (int)order.length() && order[j] != ' ' && order[j] != 13 && order[j] != 10 && order[j] != 0) num += order[j++];
    retTime = stoi(num);
    int timeExp = retTime + currentTime;

    unsigned long squareNow = 0;
    unsigned long squareDiff = 0;

    Trader::stockStruct current;
    while (order[i] != '$') {
        string stockName = "";
        while (order[i] != ' ') stockName += order[i++];
        i++;
        current.name.push_back(stockName);
        if (order[i] == '$' || (order[i] != ' ' && (order[i] < 48 || order[i] >= 58))) {
            current.quant.push_back(1);
            outPut += stockName + " ";
            otherPut += stockName + " ";
            squareNow += (long)Trader::withMe[stockName] * Trader::withMe[stockName];
            squareDiff += ((long)Trader::withMe[stockName] + buyOrd) *((long)Trader::withMe[stockName] + buyOrd);
        }
        else {
            string num = "";
            while (order[i] != ' ') num += order[i++];
            i++;
            int n = stoi(num);
            current.quant.push_back(n);
            squareNow += (long)Trader::withMe[stockName] * Trader::withMe[stockName];
            squareDiff += ((long)Trader::withMe[stockName] + buyOrd*n) *((long)Trader::withMe[stockName] + buyOrd*n);
            outPut += stockName + " " + num + " ";
            otherPut += stockName + " " + num + " ";
        }
    }

    auto& Dict_C = Trader::combinations[current];
    auto& Dict_D = Trader::timeWise[current];
    // Reminder: First == What WE buy. Second == What WE sell.

    Trader::medianPrices[current].insert(price);

    if (buyOrd == 1) {
        if (price < Trader::medianPrices[current].median()) {
            for (int i = 0; i < (int)current.name.size(); i++) {
                squareNow -= (long)Trader::withMe[current.name[i]] * Trader::withMe[current.name[i]];
                squareDiff -= ((long)Trader::withMe[current.name[i]] + buyOrd*current.quant[i]) *((long)Trader::withMe[current.name[i]] + buyOrd*current.quant[i]);
                Trader::withMe[current.name[i]] += current.quant[i]*quantity;
                squareNow += (long)Trader::withMe[current.name[i]] * Trader::withMe[current.name[i]];
                squareDiff += ((long)Trader::withMe[current.name[i]] + buyOrd*current.quant[i]) *((long)Trader::withMe[current.name[i]] + buyOrd*current.quant[i]);
            }
			string print = outPut + "$" + to_string(price) + " #" + to_string(quantity) + " " + to_string(retTime) + "\r";
			if (quantity > 0) std::cout << print << "\n"; 
			//Trader::seekPos += print.length() + 1;
        }
        else {
            Trader::combinations[current].first[price][timeExp] += quantity;
            Trader::timeWise[current][timeExp].first[price] += quantity;
        }
    }
    else {
        if (Trader::medianPrices[current].median() != INT_MIN && price > Trader::medianPrices[current].median()) {
            for (int i = 0; i < (int)current.name.size(); i++) {
                squareNow += (long)Trader::withMe[current.name[i]] * Trader::withMe[current.name[i]];
                squareDiff += ((long)Trader::withMe[current.name[i]] + buyOrd*current.quant[i]) *((long)Trader::withMe[current.name[i]] + buyOrd*current.quant[i]);
                Trader::withMe[current.name[i]] -= current.quant[i]*quantity;
                squareNow -= (long)Trader::withMe[current.name[i]] * Trader::withMe[current.name[i]];
                squareDiff -= ((long)Trader::withMe[current.name[i]] + buyOrd*current.quant[i]) *((long)Trader::withMe[current.name[i]] + buyOrd*current.quant[i]);
            }
            string print = outPut + "$" + to_string(price) + " #" + to_string(quantity) + " " + to_string(retTime) + "\r";
			if (quantity > 0) std::cout << print << "\n"; 
			//Trader::seekPos += print.length() + 1;
        }
        else {
            Trader::combinations[current].second[price][timeExp] += quantity;
            Trader::timeWise[current][timeExp].second[price] += quantity;
        }
    }
	// Median-based trader or insert.
    
    auto itr = Dict_C.first.begin();
    auto jtr = Dict_C.second.end();
    unsigned long madeTrans = 0;
	int minExp = INT_MAX;

    if (squareDiff < squareNow) {
        if (buyOrd == 1) {
            while (!itr.isNull() && squareDiff < squareNow) {
	            auto ktr = itr.val().begin();
				if (ktr.isNull()) {
					itr++; continue;
				}
				if (ktr.key() > currentTime) minExp = std::min(minExp, ktr.key() - currentTime);
                ktr.val()--;
                Dict_D[ktr.key()].first[itr.key()]--;
                if (ktr.val() == 0) {
                    auto ltr = ktr;
                    ktr++;
                    Dict_D[ltr.key()].first.remove(itr.key());
                    itr.val().remove(ltr.key());
                }
                madeTrans++;
                if (ktr.isNull()) {
					if (minExp == INT_MAX) minExp = -1;
					string print = outPut + "$" + to_string(itr.key()) + " #" + to_string(madeTrans) + " " + to_string(minExp) + "\r";
					//Trader::seekPos += print.length() + 1;
                    if (madeTrans > 0) std::cout << print << "\n";
                    madeTrans = 0;
					minExp = INT_MAX;
                    auto kt = itr;
                    itr++;
                    Dict_C.first.remove(kt.key());
                }
                squareNow = squareDiff;
                squareDiff = 0;
                for (int i = 0; i < (int)current.name.size(); i++) {
                    int x = Trader::withMe[current.name[i]] + current.quant[i];
                    squareDiff += (x + current.quant[i]) * (x + current.quant[i]);
                    Trader::withMe[current.name[i]] = x;
                }
            }
            if (!itr.isNull()) {
				if (minExp == INT_MAX) minExp = -1;
				string print = outPut + "$" + to_string(itr.key()) + " #" + to_string(madeTrans) + " " + to_string(minExp) + "\r";
				//Trader::seekPos += print.length() + 1;
				if (madeTrans > 0) std::cout << print << "\n";
			}
        }
        else {
            while (!jtr.isNull() && squareDiff < squareNow) {
                auto ktr = jtr.val().begin();
				if (ktr.isNull()) {
					jtr--; continue;
				}
				if (ktr.key() > currentTime) minExp = std::min(minExp, ktr.key() - currentTime);
                ktr.val()--;
                Dict_D[ktr.key()].second[jtr.key()]--;
                if (ktr.val() == 0) {
                    Dict_D[ktr.key()].second.remove(jtr.key());
                    auto ltr = ktr;
                    ktr++;
                    jtr.val().remove(ltr.key());
                }
                madeTrans++;
                if (ktr.isNull()) {
					if (minExp == INT_MAX) minExp = -1;
                    string print = outPut + "$" + to_string(jtr.key()) + " #" + to_string(madeTrans) + " " + to_string(minExp) + "\r";
					//Trader::seekPos += print.length() + 1;
                    if (madeTrans > 0) std::cout << print << "\n"; 
                    madeTrans = 0;
					minExp = INT_MAX;
                    auto kt = jtr;
                    jtr--;
                    Dict_C.second.remove(kt.key());
                }
                squareNow = squareDiff;
                squareDiff = 0;
                for (int i = 0; i < (int)current.name.size(); i++) {
                    int x = Trader::withMe[current.name[i]] - current.quant[i];
                    squareDiff += (x - current.quant[i]) * (x - current.quant[i]);
                    Trader::withMe[current.name[i]] = x;
                }
            }
            if (!jtr.isNull()) {
				if (minExp == INT_MAX) minExp = -1;
				string print = outPut + "$" + to_string(jtr.key()) + " #" + to_string(madeTrans) + " " + to_string(minExp) + "\r";
				//Trader::seekPos += print.length() + 1;
				if (madeTrans > 0) std::cout << print << "\n";
			}
        }
    }
    else {
        squareDiff = 0;
        for (int i = 0; i < (int)current.name.size(); i++) {
            int x = Trader::withMe[current.name[i]] - current.quant[i];
            squareDiff += (x - buyOrd*current.quant[i]) * (x - buyOrd*current.quant[i]);
            Trader::withMe[current.name[i]] = x;
        }
        if (buyOrd == 1) {
            while (!jtr.isNull() && squareDiff < squareNow) {
                auto ktr = jtr.val().begin();
				if (ktr.isNull()) {
					jtr--; continue;
				}
				if (ktr.key() > currentTime) minExp = std::min(minExp, ktr.key() - currentTime);
                ktr.val()--;
                Dict_D[ktr.key()].second[jtr.key()]--;
                if (ktr.val() == 0) {
                    Dict_D[ktr.key()].second.remove(jtr.key());
                    auto ltr = ktr;
                    ktr++;
                    jtr.val().remove(ltr.key());
                }
                madeTrans++;
                if (ktr.isNull()) {
					if (minExp == INT_MAX) minExp = -1;
                    string print = otherPut + "$" + to_string(jtr.key()) + " #" + to_string(madeTrans) + " " + to_string(minExp) + "\r";
					//Trader::seekPos += print.length() + 1;
                    if (madeTrans > 0) std::cout << print << "\n";
                    madeTrans = 0;
					minExp = INT_MAX;
                    auto kt = jtr;
                    jtr--;
                    Dict_C.second.remove(kt.key());
                }
                squareNow = squareDiff;
                squareDiff = 0;
                for (int i = 0; i < (int)current.name.size(); i++) {
                    int x = Trader::withMe[current.name[i]] - current.quant[i];
                    squareDiff += (x - current.quant[i]) * (x - current.quant[i]);
                    Trader::withMe[current.name[i]] = x;
                }
            }
            if (!jtr.isNull()) {
				if (minExp == INT_MAX) minExp = -1;
				string print = otherPut + "$" + to_string(jtr.key()) + " #" + to_string(madeTrans) + " " + to_string(minExp) + "\r";
				//Trader::seekPos += print.length() + 1;
				if (madeTrans > 0) std::cout << print << "\n";
			}
        }
        else {
            while (!itr.isNull() && squareDiff < squareNow) {
                auto ktr = itr.val().begin();
				if (ktr.isNull()) {
					itr++; continue;
				}
				if (ktr.key() > currentTime) minExp = std::min(minExp, ktr.key() - currentTime);
                ktr.val()--;
                Dict_D[ktr.key()].first[itr.key()]--;
                if (ktr.val() == 0) {
                    auto ltr = ktr;
                    ktr++;
                    Dict_D[ltr.key()].first.remove(itr.key());
                    itr.val().remove(ltr.key());
                }
                madeTrans++;
                if (ktr.isNull()) {
					if (minExp == INT_MAX) minExp = -1;
                    string print = otherPut + "$" + to_string(itr.key()) + " #" + to_string(madeTrans) + " " + to_string(minExp) + "\r";
					//Trader::seekPos += print.length() + 1;
                    if (madeTrans > 0) std::cout << print << "\n";
                    madeTrans = 0;
					minExp = INT_MAX;
                    auto kt = itr;
                    itr++;
                    Dict_C.first.remove(kt.key());
                }
                squareNow = squareDiff;
                squareDiff = 0;
                for (int i = 0; i < (int)current.name.size(); i++) {
                    int x = Trader::withMe[current.name[i]] + current.quant[i];
                    squareDiff += (x + current.quant[i]) * (x + current.quant[i]);
                    Trader::withMe[current.name[i]] = x;
                }
            }
            if (!itr.isNull()) {
				if (minExp == INT_MAX) minExp = -1;
				string print = otherPut + "$" + to_string(itr.key()) + " #" + to_string(madeTrans) + " " + to_string(minExp) + "\r";
				//Trader::seekPos += print.length() + 1;
				if (madeTrans > 0) std::cout << print << "\n";
			}
        }
    }
	// Clear out extra stocks with us.
	minExp = INT_MAX;

    while (!itr.isNull() && !jtr.isNull() && itr.key() < jtr.key()) {
        madeTrans = 0;
        auto ktr = itr.val().begin();
        auto ltr = jtr.val().begin();

        while (!ktr.isNull() && !ltr.isNull()) {
            madeTrans++;
			if (ktr.key() > currentTime) minExp = std::min(minExp, ktr.key() - currentTime);
			if (ltr.key() > currentTime) minExp = std::min(minExp, ltr.key() - currentTime);
            ktr.val()--;
            Dict_D[ktr.key()].first[itr.key()]--;
            if (ktr.val() <= 0) {
                auto mtr = ktr;
                ktr++;
                Dict_D[mtr.key()].first.remove(itr.key());
                itr.val().remove(mtr.key());
            }
            ltr.val()--;
            Dict_D[ltr.key()].second[jtr.key()]--;
            if (ltr.val() <= 0) {
                auto mtr = ltr;
                ltr++;
                Dict_D[mtr.key()].second.remove(jtr.key());
                jtr.val().remove(mtr.key());
            }
        }
		if (minExp == INT_MAX) minExp = -1;
        if (buyOrd == 1) {
			string print1 = outPut + "$" + to_string(itr.key()) + " #" + to_string(madeTrans) + " " + to_string(minExp) + "\r";
			string print2 = otherPut + "$" + to_string(jtr.key()) + " #" + to_string(madeTrans) + " " + to_string(minExp) + "\r";
			if (madeTrans > 0) std::cout << print1 << "\n" << print2 << "\n";
			//Trader::seekPos += print1.length() + print2.length() + 2;
        }
        else {
            string print1 = otherPut + "$" + to_string(itr.key()) + " #" + to_string(madeTrans) + " " + to_string(minExp) + "\r";
			string print2 = outPut + "$" + to_string(jtr.key()) + " #" + to_string(madeTrans) + " " + to_string(minExp) + "\r";
			if (madeTrans > 0) std::cout << print1 << "\n" << print2 << "\n";
			//Trader::seekPos += print1.length() + print2.length() + 2;
        }
        if (ktr.isNull()) {
            auto mtr = itr;
            itr++;
            Dict_C.first.remove(mtr.key());
        }
        if (ltr.isNull()) {
            auto mtr = jtr;
            jtr--;
            Dict_C.second.remove(mtr.key());
        }
		minExp = INT_MAX;
    }
	// Arbitrage
    return 0;
}

void* userTrader(void* arg)
{
	auto it0 = Trader::timeWise.begin();
	auto it1 = Trader::combinations.begin();

	while (!it0.isNull()) {
		auto& dict_C = it0.val()[*(int*)arg];
		auto kt0 = dict_C.first.begin();
		auto kt1 = dict_C.second.begin();

		while (!kt0.isNull()) {
			it1.val().first[kt0.key()].remove(*(int*)arg);
			kt0++;
		}
		while (!kt1.isNull()) {
			it1.val().second[kt1.key()].remove(*(int*)arg);
			kt1++;
		}
		it0.val().remove(*(int*)arg);
		it0++;
		it1++;
	}
    return nullptr;
}

// EXECUTE ORDERS BASED ON A GIVEN TIME;
int reader(int time)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    ifstream file("./output.txt");
    file.seekg(Trader::seekPos, ios::cur);
    string* message = new string [1];
    std::getline(file, message[0]);
    while (message[0] != "") 
	{
        Trader::seekPos += message[0].length() + 1;
        if (message[0][0] == '!') {
			delete [] message;
			return 0;
		}
        if (message[0].length() >= 10 && message[0][0] != '(') trader(message);
        message[0] = "";
        std::getline(file, message[0]);
    }
    file.close();
	void* timer = malloc(4);
	*(int*)timer = commonTimer.load();
	userTrader(timer);
	free(timer);
	delete [] message;
    return 1;
}


void* userThread(void* arg)
{
    int x = 1;
    while (x) {
        x = reader(0);
    }
    return nullptr;
}
