// This is your trader. Place your orders from here

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
using namespace std;

extern std::mutex printMutex;
extern std::atomic<int> commonTimer;
extern int NUM_THREADS;

template <typename T, typename U>
struct Pair {
	T first;
	U second;
	Pair() {}
	Pair(T a, U b): first(a), second(b) {}
	bool operator == (Pair& B) {
		return (first == B.first && second == B.second);
	}
	bool operator < (Pair& B) {
		if (first < B.first) return true;
		if (first == B.first) return (second < B.second);
		return false;
	}
};

template <typename T, typename U>
class dict {
private:
template <typename V>
	struct Tuple {
		V* first;
		U second;
		int8_t third;
	};

	struct rbNode {
		T key;
		U val;
		bool color; // 0{Black} 1{Red}
		rbNode* left;
		rbNode* right;
		rbNode* parent;

		rbNode(T k, U v): key(k), val(v) {
			left = nullptr;
			right = nullptr;
			parent = nullptr;
			color = 1;
		}

		rbNode* copy() {
			rbNode* temp = new rbNode(key, val);
			temp->color = color;
			if (left != nullptr) temp->left = left->copy();
			else temp->left = nullptr;
			if (right != nullptr) temp->right = right->copy();
			else temp->right = nullptr;
			return temp;
		}
		void destroy() {
			if (left != nullptr) left->destroy();
			if (right != nullptr) right->destroy();
			delete this;
		}

		void rotate(int side) {
			if (side == -1) { // left side goes to top
				if (left == nullptr) return;
				rbNode* temp = left;
				temp->parent = parent;
				if (parent != nullptr) {
					if (parent->left == this) parent->left = temp;
					else parent->right = temp;
				}
				left = temp->right;
				temp->right = this;
				parent = temp;
				if (left != nullptr) left->parent = this;
			}
			else if (side == 1) { // right side goes to top
				if (right == nullptr) return;
				rbNode* temp = right;
				temp->parent = parent;
				if (parent != nullptr) {
					if (parent->left == this) parent->left = temp;
					else parent->right = temp;
				}
				right = temp->left;
				temp->left = this;
				parent = temp;
				if (right != nullptr) right->parent = this;
			}
			else {
				throw invalid_argument("\nSide to Elevate in Rotation: -1(Left) or +1(Right)");
			}
		}

		rbNode* find(T k) {
			if (k == key) return this;
			if (key < k) {
				if (right == nullptr) return nullptr;
				return right->find(k);
			}
			else {
				if (left == nullptr) return nullptr;
				return left->find(k);
			}
		}

		rbNode* insert(Pair<T, U> data) {
			if (data.first == key) 
			throw invalid_argument("\nDuplicate Entries Error");
			if (data.first < key) {
				if (left == nullptr) {
					left = new rbNode(data.first, data.second);
					left->parent = this;
					return left;
				}
				else return left->insert(data);
			}
			else {
				if (right == nullptr) {
					right = new rbNode(data.first, data.second);
					right->parent = this;
					return right;
				}
				else return right->insert(data);
			}
		}

		Tuple<rbNode> remove(T k) {
			U v = val;
			int8_t side; // side in which something was removed.
			if (k == key) {
				if (right == nullptr) {
					rbNode* temp = parent;
					if (left != nullptr) left->parent = temp;
					if (temp != nullptr) 
					{
						if (temp->right == this) {
							temp->right = left;
							side = 1;
						} else {
							temp->left = left;
							side = -1;
						}
						if (color == 1) side = 0;
					}
					if (temp == nullptr) {
						temp = left;
						if (temp != nullptr) temp->color = 0;
						delete this;
						return {temp, v, 2};
					}
					delete this;
					return {temp, v, side};
				}
				else if (left == nullptr) {
					rbNode* temp = parent;
					right->parent = temp;
					if (temp != nullptr)
					{
						if (temp->right == this) {
							temp->right = right;
							side = 1;
						} else {
							temp->left = right;
							side = -1;
						}
						if (color == 1) side = 0;
					}
					if (temp == nullptr) {
						temp = right;
						temp->color = 0;
						delete this;
						return {temp, v, 2};
					}
					delete this;
					return {temp, v, side};
				}
				else {
					rbNode* temp = right;
					while (temp->left != nullptr) temp = temp->left;
					if (temp == right) side = 1; else side = -1;

					rbNode* rit = temp->right;
					if (parent != nullptr) {
						if (parent->left == this) parent->left = temp; else parent->right = temp;
					}
					else side *= 3;
					if (side > 0) 
					{
						temp->parent = parent;
						left->parent = temp;
						temp->left = left;
						if (temp->color == 1) {
							if (side == 3) side = 6;
							else side = 0;
						}
						temp->color = color; 
						delete this;
						return {temp, v, side};
					}

					rbNode* par = temp->parent;
					temp->parent = parent;
					right->parent = temp;
					left->parent = temp;
					temp->right = right;
					temp->left = left;
					if (temp->color == 1) {
						if (side == -3) side = 6;
						else side = 0;
					}
					temp->color = color;
					par->left = rit;
					if (rit != nullptr) rit->parent = par;
					delete this;
					return {par, v, side};
				}
			}
			else if (k < key) {
				if (left != nullptr) return left->remove(k);
				else throw invalid_argument("\nDelete Nonexistant Key Error");
			}
			else {
				if (right != nullptr) return right->remove(k);
				else throw invalid_argument("\nDelete Nonexistant Key Error");
			}
		}

	};
	
	void balanceInsert(rbNode* temp) {
		if (temp->parent == nullptr) {temp->color = 0; return;}
		if (temp->parent->color == 0) return;

		rbNode* other;
		rbNode* firstPar = temp->parent;
		rbNode* blackPar = firstPar->parent;
		if (blackPar->left == firstPar) other = blackPar->right; else other = blackPar->left;
		
		if (other != nullptr && other->color == 1) {
			other->color = 0;
			firstPar->color = 0;
			blackPar->color = 1;
			balanceInsert(blackPar);
			return;
		}

		if (blackPar->right == firstPar) {
			if (firstPar->left == temp) 
			{
				firstPar->rotate(-1);
				blackPar->rotate(+1);
				blackPar->color = 1;
				temp->color = 0;
				if (root == blackPar) root = temp;
			}
			else {
				blackPar->rotate(+1);
				blackPar->color = 1;
				firstPar->color = 0;
				if (root == blackPar) root = firstPar;
			}
		}
		else {
			if (firstPar->right == temp) {
				firstPar->rotate(+1);
				blackPar->rotate(-1);
				blackPar->color = 1;
				temp->color = 0;
				if (root == blackPar) root = temp;
			}
			else {
				blackPar->rotate(-1);
				blackPar->color = 1;
				firstPar->color = 0;
				if (root == blackPar) root = firstPar;
			}
		}
	}

	void balanceDelete(rbNode* temp, int side) {
		if (temp == nullptr || side == 0) return; // Root deleted -- only one left node OR red node deleted.
		if (side == -1) {
			if (temp->left != nullptr && temp->left->color == 1) {
				temp->left->color = 0;
				return;
			}
			rbNode* other = temp->right;
			if (other == nullptr) return;
			// other node cannot be a nullptr actually. Unless deleted node was terminal red.

			if (other->color == 1) {
				temp->rotate(1);
				if (root == temp) root = other;
				temp->color = 1;
				other->color = 0;
				balanceDelete(temp, -1);
				return;
			}

			if (other->left == nullptr || other->left->color == 0) {
				if (other->right == nullptr || other->right->color == 0) {
					other->color = 1;
					if (temp->color == 1) {temp->color = 0; return;}
					if (temp->parent != nullptr) {
						rbNode* y = temp->parent;
						if (y->left == temp) balanceDelete(y, -1); else balanceDelete(y, +1);
					}
					return;
				}
				else {
					temp->rotate(1);
					if (root == temp) root = other;
					other->color = temp->color;
					temp->color = 0;
					if (other->right == nullptr) return;
					other->right->color = 0;
				}
			}
			else {
				if (other->right == nullptr || other->right->color == 0) {
					other->rotate(-1);
					other = temp->right;
					other->color = 0;
					other->right->color = 1;
				}
				temp->rotate(1);
				if (root == temp) root = other;
				other->color = temp->color;
				temp->color = 0;
				if (other->right == nullptr) return;
				other->right->color = 0;
			}
		}
		else if (side == 1) {
			if (temp->right != nullptr && temp->right->color == 1) {
				temp->right->color = 0;
				return;
			}
			rbNode* other = temp->left;
			if (other == nullptr) return;
			// other node cannot be a nullptr actually. Unless deleted node was terminal red.

			if (other->color == 1) {
				temp->rotate(-1);
				if (root == temp) root = other;
				temp->color = 1;
				other->color = 0;
				balanceDelete(temp, +1);
				return;
			}

			if (other->right == nullptr || other->right->color == 0) {
				if (other->left == nullptr || other->left->color == 0) {
					other->color = 1;
					if (temp->color == 1) {temp->color = 0; return;}
					if (temp->parent != nullptr) {
						rbNode* y = temp->parent;
						if (y->right == temp) balanceDelete(y, +1); else balanceDelete(y, -1);
					}
					return;
				}
				else {
					temp->rotate(-1);
					if (root == temp) root = other;
					other->color = temp->color;
					temp->color = 0;
					if (other->left == nullptr) return;
					other->left->color = 0;
				}
			}
			else {
				if (other->left == nullptr || other->left->color == 0) {
					other->rotate(1);
					other = temp->left;
					other->color = 0;
					other->left->color = 1;
				}
				temp->rotate(-1);
				if (root == temp) root = other;
				other->color = temp->color;
				temp->color = 0;
				if (other->left == nullptr) return;
				other->left->color = 0;
			}
		}
	}

	rbNode* root;
	int length;

public:
	dict() {root = nullptr; length = 0;}
	dict(dict& Oth) {
		if (Oth.root == root) return;
		length = Oth.length;
		if (Oth.root != nullptr) root = Oth.root->copy();
		else root = nullptr;
	}
	void operator = (dict& Oth) {
		if (Oth.root == root) return;
		length = Oth.length;
		if (Oth.root != nullptr) root = Oth.root->copy();
		else root = nullptr;
	}
	~dict() {
		if (root != nullptr) root->destroy();
		root = nullptr;
		length = 0;
	}
	int size() {return length;}
	bool empty() {return (length == 0);}

	class iterator {
		rbNode* node;
	public:
		bool isNull() {return node == nullptr;}
		T& key() {return node->key;}
		U& val() {return node->val;}

		bool isBegin() {
			if (node == nullptr) return 1;
			if (node->left != nullptr) return 0;
			auto s = node;
			while (s->parent != nullptr) {
				if (s->parent->right == s) return 0;
				s = s->parent;
			}
			return 1;
		}

		bool isEnd() {
			if (node == nullptr) return 1;
			if (node->right != nullptr) return 0;
			auto s = node;
			while (s->parent != nullptr) {
				if (s->parent->left == s) return 0;
				s = s->parent;
			}
			return 1;
		}
		
		iterator(rbNode* s) {
			node = s;
		}
		~iterator() {}
		
		iterator operator ++() {
			if (node == nullptr) return iterator(nullptr);
			else if (node->right != nullptr) {
				node = node->right;
				while (node->left != nullptr) node = node->left;
			}
			else {
				while (node->parent != nullptr && node->parent->right == node) {
					node = node->parent;
				}
				node = node->parent;
			}
			return iterator(node);
		}

		iterator operator ++(int) {
			iterator temp(node);
			if (node == nullptr) return iterator(nullptr);
			else if (node->right != nullptr) {
				node = node->right;
				while (node->left != nullptr) node = node->left;
			}
			else {
				while (node->parent != nullptr && node->parent->right == node) {
					node = node->parent;
				}
				node = node->parent;
			}
			return temp;
		}

		iterator operator --() {
			if (node == nullptr) return iterator(nullptr);
			else if (node->left != nullptr) {
				node = node->left;
				while (node->right != nullptr) node = node->right;
			}
			else {
				while (node->parent != nullptr && node->parent->left == node) {
					node = node->parent;
				}
				node = node->parent;
			}
			return iterator(node);
		}

		iterator operator --(int) {
			iterator temp(node);
			if (node == nullptr) return iterator(nullptr);
			else if (node->left != nullptr) {
				node = node->left;
				while (node->right != nullptr) node = node->right;
			}
			else {
				while (node->parent != nullptr && node->parent->left == node) {
					node = node->parent;
				}
				node = node->parent;
			}
			return temp;
		}

		void operator += (int n) {
			if (n == 0) return;
			if (n > 0) {
				for (int i = 0; i < n; i++) {
					if (node == nullptr) return;
					else if (node->right != nullptr) {
						node = node->right;
						while (node->left != nullptr) node = node->left;
					}
					else {
						while (node->parent != nullptr && node->parent->right == node) {
							node = node->parent;
						}
						node = node->parent;
					}
				}
			}
			else {
				n *= -1;
				for (int i = 0; i < n; i++) {
					if (node == nullptr) return;
					else if (node->left != nullptr) {
						node = node->left;
						while (node->right != nullptr) node = node->right;
					}
					else {
						while (node->parent != nullptr && node->parent->left == node) {
							node = node->parent;
						}
						node = node->parent;
					}
				}
			}
		}

		void operator -= (int n) {
			*this += (-n);
		}
	};

	U at(T key) {
		if (root == nullptr) throw invalid_argument("\nNot Found Error");
		rbNode* val = (root->find(key));
		if (val == nullptr) throw invalid_argument("\nNot Found Error");
		return *val;
	}

	iterator find(T key) {
		if (root != nullptr) return iterator(root->find(key)); 
		return iterator(nullptr);
	}

	iterator begin() {
		if (root == nullptr) return iterator(nullptr);
		rbNode* s = root;
		while (s->left != nullptr) s = s->left;
		return iterator(s);
	}

	iterator end() {
		if (root == nullptr) return iterator(nullptr);
		rbNode* s = root;
		while (s->right != nullptr) s = s->right;
		return iterator(s);
	}

	U& operator [] (T key) {
		if (root == nullptr) {
			root = new rbNode(key, U());
			root->color = 0;
			length++;
			return root->val;
		}
		rbNode* n = (root->find(key));
		if (n == nullptr) {
			rbNode* temp = root->insert({key, U()});
			length++;
			balanceInsert(temp);
			return temp->val;
		}
		return n->val;
	}

	void insert(T k, U v) {
		if (root == nullptr) {
			length++;
			root = new rbNode(k, v);
			root->color = 0;
		}
		else {
			rbNode* temp = root->insert({k, v});
			length++;
			balanceInsert(temp);
		}
	}

	U remove(T key) {
		if (root == nullptr) 
			throw invalid_argument("\nDelete Nonexistant Key Error");
		Tuple<rbNode> temp = root->remove(key);
		length--;
		if (temp.third == 2) root = temp.first;
		else if (temp.third == 3 || temp.third == -3 || temp.third == 6) 
		{
			auto s = temp.first;
			while (s->parent != nullptr) s = s->parent;
			root = s;
			temp.third /= 3;
			if (temp.third == 2) temp.third = 0;
			balanceDelete(temp.first, temp.third);
		}
		else balanceDelete(temp.first, temp.third);
		return temp.second;
	}
};

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
    string outPut = to_string(currentTime) + " tejassharma_yashjonjale ";
    string otherPut = outPut;
    std::lock_guard<std::mutex> lock(printMutex);
    int i = 0;
    while (true) {
        if (order[i] >= 48 && order[i] < 58) i++;
        else {i++; break;}
    }
	string name = "";
    while (order[i] != ' ') name += order[i++]; i++;
	if (name == "tejassharma_yashjonjale") return 0;

    int8_t buyOrd = 0;
    int medianWeighted = 0;

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

    auto it = Dict_D.begin();
    while (!it.isNull() && it.key() < currentTime) {
        auto jt = it.val().first.begin();
        auto kt = it.val().second.begin();

        while (!jt.isNull()) {
            Dict_C.first[jt.key()].remove(it.key());
            jt++;
        }
        while (!kt.isNull()) {
            Dict_C.second[kt.key()].remove(it.key());
			kt++;
        }

        auto lt = it;
        it++;
        Dict_D.remove(lt.key());
    }
	// Wash out earlier orders.

    Trader::medianPrices[current].insert(price);

    if (buyOrd == 1) {
        if (price < Trader::medianPrices[current].median()) {
            for (int i = 0; i < (int)current.name.size(); i++) {
                Trader::withMe[current.name[i]] += current.quant[i]*quantity;
            }
			string print = outPut + "$" + to_string(price) + " #" + to_string(quantity) + " 2\r";
			if (quantity > 0) std::cout << print << "\n"; 
			//Trader::seekPos += print.length() + 1;
        }
        else {
            Trader::combinations[current].first[price][timeExp] += quantity;
            Trader::timeWise[current][timeExp].first[price] += quantity;
        }
    }
    else {
        if (price > Trader::medianPrices[current].median()) {
            for (int i = 0; i < (int)current.name.size(); i++) {
                Trader::withMe[current.name[i]] -= current.quant[i]*quantity;
            }
            string print = outPut + "$" + to_string(price) + " #" + to_string(quantity) + " 2\r";
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
    int madeTrans = 0;

    if (squareDiff < squareNow) {
        if (buyOrd == 1) {
            while (!itr.isNull() && squareDiff < squareNow) {
	            auto ktr = itr.val().begin();
				if (ktr.isNull()) {
					itr++; continue;
				}
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
					string print = outPut + "$" + to_string(itr.key()) + " #" + to_string(madeTrans) + " 2\r";
					//Trader::seekPos += print.length() + 1;
                    if (madeTrans > 0) std::cout << print << "\n";
                    madeTrans = 0;
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
				string print = outPut + "$" + to_string(itr.key()) + " #" + to_string(madeTrans) + " 2\r";
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
                    string print = outPut + "$" + to_string(jtr.key()) + " #" + to_string(madeTrans) + " 2\r";
					//Trader::seekPos += print.length() + 1;
                    if (madeTrans > 0) std::cout << print << "\n"; 
                    madeTrans = 0; 
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
				string print = outPut + "$" + to_string(jtr.key()) + " #" + to_string(madeTrans) + " 2\r";
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
                    string print = otherPut + "$" + to_string(jtr.key()) + " #" + to_string(madeTrans) + " 2\r";
					//Trader::seekPos += print.length() + 1;
                    if (madeTrans > 0) std::cout << print << "\n";
                    madeTrans = 0; 
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
				string print = otherPut + "$" + to_string(jtr.key()) + " #" + to_string(madeTrans) + " 2\r";
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
                    string print = otherPut + "$" + to_string(itr.key()) + " #" + to_string(madeTrans) + " 2\r";
					//Trader::seekPos += print.length() + 1;
                    if (madeTrans > 0) std::cout << print << "\n";
                    madeTrans = 0;
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
				string print = otherPut + "$" + to_string(itr.key()) + " #" + to_string(madeTrans) + " 2\r";
				//Trader::seekPos += print.length() + 1;
				if (madeTrans > 0) std::cout << print << "\n";
			}
        }
    }
	// Clear out extra stocks with us.

    while (!itr.isNull() && !jtr.isNull() && itr.key() < jtr.key()) {
        madeTrans = 0;
        auto ktr = itr.val().begin();
        auto ltr = jtr.val().begin();

        while (!ktr.isNull() && !ltr.isNull()) {
            madeTrans++;
            ktr.val()--;
            Dict_D[ktr.key()].first[itr.key()]--;
            if (ktr.val() == 0) {
                auto mtr = ktr;
                ktr++;
                Dict_D[mtr.key()].first.remove(itr.key());
                itr.val().remove(mtr.key());
            }
            ltr.val()--;
            Dict_D[ltr.key()].second[jtr.key()]--;
            if (ltr.val() == 0) {
                auto mtr = ltr;
                ltr++;
                Dict_D[mtr.key()].second.remove(jtr.key());
                jtr.val().remove(mtr.key());
            }
        }
        if (buyOrd == 1) {
			string print1 = outPut + "$" + to_string(itr.key()) + " #" + to_string(madeTrans) + " 2\r";
			string print2 = otherPut + "$" + to_string(jtr.key()) + " #" + to_string(madeTrans) + " 2\r";
			if (madeTrans > 0) std::cout << print1 << "\n" << print2 << "\n";
			//Trader::seekPos += print1.length() + print2.length() + 2;
        }
        else {
            string print1 = otherPut + "$" + to_string(itr.key()) + " #" + to_string(madeTrans) + " 2\r";
			string print2 = outPut + "$" + to_string(jtr.key()) + " #" + to_string(madeTrans) + " 2\r";
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
    }
	// Arbitrage
    return 0;
}

// EXECUTE ORDERS BASED ON A GIVEN TIME;
int reader(int time)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    ifstream file("./output.txt");
    file.seekg(Trader::seekPos, ios::cur);
    string* message = new string [1];
    std::getline(file, message[0]);
    while (message[0] != "") {
        Trader::seekPos += message[0].length() + 1;
        if (message[0][0] == '!') return 0;
        if (message[0].length() >= 10 && message[0][0] != '(') trader(message);
        message[0] = "";
        std::getline(file, message[0]);
    }
    file.close();
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

void* userTrader(void* arg)
{
    return nullptr;
}