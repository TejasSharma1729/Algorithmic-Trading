#include "market.h"
#include <fstream>
#include <string>
#include <vector>
using namespace std;

int market::split(string message, int prevT, int line) {
    vector<string> splits;
    string ord = "";
    int i = 0;
    while (true) {
        if (message[i] == ' ' || message[i] == 13 || message[i] == 10 || message[i] == 0) {
            if (ord != "") splits.push_back(ord);
            ord = "";
            if (message[i] == 13 || message[i] == 10 || message[i] == 0) break;
            i++;
        } 
        else {
            if (message[i] == '#' || message[i] == '$') i++;
            else ord += message[i++];
        }
    }

    int n = splits.size();
    order now;
    now.person = splits[1];
    now.timeIn = stoi(splits[0]);
    now.timeExp = now.timeIn + stoi(splits[n-1]);
    now.price = stoi(splits[n-3]);
    now.qty = stoi(splits[n-2]);
    now.line = line;
    stock current;
    for (int i = prevT; i < stoi(splits[0]); i++) for (auto itr = stocks.begin(); !itr.isNull(); ++itr) itr.key().expire(i);

    if (n == 7) {
        current.name.push_back(splits[3]);
        current.quant.push_back(1);
        current.numTransacted = 1;
    }
    else {
        for (int i = 3; i < n-4; i += 2) {
            current.name.push_back(splits[i]);
            current.quant.push_back(stoi(splits[i+1]));
            current.numTransacted += stoi(splits[i+1]);
        }
    }
    auto itr = stocks.find(current);
    if (!itr.isNull()) {
        if (splits[2][0] == 'B') {
            auto jtr = itr.key().bookBuy.find(now);
            if (!jtr.isNull()) {
                jtr.key().qty += now.qty;
            }
            else {
                itr.key().insert(now, stoi(splits[n-1]), 0);
            }
        }
        else {
            auto jtr = itr.key().bookSell.find(now);
            if (!jtr.isNull()) {
                jtr.key().qty += now.qty;
            }
            else {
                itr.key().insert(now, stoi(splits[n-1]), 1);
            }
        }
    }
    else {
        stocks[current] = false;
        auto itr = stocks.find(current);
        if (splits[2][0] == 'B') itr.key().insert(now, stoi(splits[n-1]), 0);
        else itr.key().insert(now, stoi(splits[n-1]), 1);
    }

    for (auto itr = stocks.begin(); !itr.isNull(); ++itr) itr.key().transact(peoples, netExchange, numTrades, numSharedTrades);
    return stoi(splits[0]);
}

market::market(int argc, char** argv)
{
	netExchange = 0;
    numTrades = 0;
    numSharedTrades = 0;
}

void market::start()
{
    int lastTime = 0;
    int line = 0;
	string message;
    ifstream readFile("./output.txt");
    getline(readFile, message);
    getline(readFile, message);
    while (message != "" && message[0] != '!') {
        lastTime = split(message, lastTime, line);
        line++;
        getline(readFile, message);
    }

    cout << "\n---End of Day---\n";
    cout << "Total Amount of Money Transferred: $" << netExchange << "\n";
    cout << "Number of Completed Trades: " << numTrades << "\n";
    cout << "Number of Shares Traded: " << numSharedTrades << "\n";
    for (auto itr = peoples.begin(); !itr.isNull(); itr++) {
        cout << itr.key() << " bought " << itr.val().bought << " and sold " << itr.val().sold << " for a net transfer of $" << itr.val().obtained << "\n";
    }
}
