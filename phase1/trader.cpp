#include "receiver.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;

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
		for (int j = 0; j < (int)names.size(); j++) {
			if (names[j] == temp) {current[j] = stoi(num); flags = 0;}
		}
		if (flags) {
			names.push_back(temp);
			current.push_back(stoi(num));
			for (auto itr = linComb.begin(); !itr.isNull(); itr++) itr.key().push_back(0);
			for (int j = 0; j < (int)history.size(); j++) history[j].first.push_back(0);
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
	auto itr = stocks.find(stockName);

	if (!itr.isNull()) {
		if (bs == 'b') {
			if (itr.val().buy >= price) {
				cout << "No Trade\n";
			}
			else if (itr.val().sell == price) {
				cout << "No Trade\n";
				itr.val().sell = INT_MAX;
			}
			else {
				itr.val().buy = price;
				if (price > itr.val().price) {
					itr.val().price = price;
					cout << stockName << " " << price << " s\n";
				}
				else cout << "No Trade\n";
			}
		}
		else {
			if (itr.val().sell <= price) {
				cout << "No Trade\n";
			}
			else if (itr.val().buy == price) {
				cout << "No Trade\n";
				itr.val().buy = INT_MIN;
			}
			else {
				itr.val().sell = price;
				if (price < itr.val().price) {
					itr.val().price = price;
					cout << stockName << " " << price << " b\n";
				}
				else cout << "No Trade\n";
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

void arbitrage(string message, vector<stock>& stocks, ptr<Pair<vector<int>, int8_t>>& people, ptr<string>& orders, int& FinalProfit, int& peps) {
	vector<string> splits;
	string order = stringSplit(message, splits);
	int n = splits.size();
	auto T = new node<Pair<vector<int>, int8_t>>;
	auto U = new node<string>;
	T->val.first.resize(stocks.size(), 0);
	// Notation: 0 == Buy Order, 1 == Sell Order.

	T->val.second = 0;
	T->next = people;
	U->val = order;
	U->next = orders;
	people = T;
	orders = U;
	peps++;

	people->val.first[0] = stoi(splits[n-2]);
	for (int i = 0; i < n-2; i += 2) {
		bool flags = 0;
		for (int j = 1; j < (int)stocks.size(); j++) if (splits[i] == stocks[j].name) {
			flags = 1;
			people->val.first[j] = stoi(splits[i+1]);
			break;
		}
		if (!flags) {
			stocks.push_back({splits[i], i/2+1});
			T = people;
			while (T != nullptr) { 
				T->val.first.push_back(0);
				T = T->next;
			}
			people->val.first[stocks.size()-1] = stoi(splits[i+1]);
		}
	}
	if (splits[n-1][0] == 's') {
		for (int j = 0; j < (int)stocks.size(); j++) people->val.first[j] *= -1;
		people->val.second = 1;
	}
	// ADDED ENTRY FOR NEW PERSON

	T = people->next;
	auto prev = people;
	U = orders;
	auto V = U->next;

	while (T != nullptr) { 
		bool flags = 1;
		bool cancel = 1;
		for (int j = 0; j < (int)stocks.size(); j++) {
			if (j != 0 && T->val.first[j] != people->val.first[j]) flags = 0;
			if (T->val.first[j] + people->val.first[j] != 0) cancel = 0;
			if (T->val.second + people->val.second != 1) cancel = 0; // One buy and one sell only.
		}
		if (flags == 1 && people->val.second == T->val.second) {
			// Both buy or sell: the first one is less efficient than second.
			if (T->val.first[0] < people->val.first[0])  {
				prev->next = T->next;
				U->next = V->next;
				delete T;
				delete V;
				T = prev->next;
				peps--;
			}
			else {
				cout << "No Trade\n";
				prev = people;
				U = orders;
				people = people->next;
				orders = orders->next;
				delete prev;
				delete U;
				peps--;
				return;
			}
		}
		else if (cancel == 1) {
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
			for (int j = 0; j < m; j++) resultant[j] += included[i]*T->val.first[j];
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
		ptr<Pair<vector<int>, int8_t>> prev = nullptr;
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
	for (int i = 0; i < (int)linComb.size(); i++)
	{
		auto& Dict_J = itr.val().first;
		auto& Dict_K = itr.val().second;
		int x = 0;
		// NOTATION: x < 0 means something we take buy orders. x > 0 means something we take sell orders.

		while (true) {
			profits[i][x] = 0;
			auto ktr = Dict_K.end();
			int y = 0;
			while (!ktr.isNull()) {
				if (y + ktr.val() <= -x) {
					y += ktr.val();
					profits[i][x] += ktr.val() * ktr.key();
					--ktr;
				} else {
					profits[i][x] -= ktr.key() * (x + y);
					break;
				}
			}
			if (ktr.isNull()) {
				break;
			}
			auto jtr = Dict_J.begin();
			int kIn = -(x + y);
			int jIn = 0;
			while (!jtr.isNull() && !ktr.isNull() && jtr.key() <= ktr.key()) {
				profits[i][x] += ktr.key() - jtr.key();
				kIn++;
				if (kIn == ktr.val()) {
					ktr--; kIn = 0;
				}
				jIn++;
				if (jIn == jtr.val()) {
					jtr++; jIn = 0;
				}
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
						profits[i][x] -= jtr.key() * jtr.val();
						++jtr;
					} else {
						profits[i][x] -= jtr.key() * (x - y);
						break;
					}
				}
				if (jtr.isNull()) {
					break;
				}
				auto ktr = Dict_K.end();
				int jIn = x - y;
				int kIn = 0;
				while (!jtr.isNull() && !ktr.isNull() && jtr.key() <= ktr.key()) {
					profits[i][x] += ktr.key() - jtr.key();
					kIn++;
					if (kIn == ktr.val()) {
						ktr--; kIn = 0;
					}
					jIn++;
					if (jIn == jtr.val()) {
						jtr++; jIn = 0;
					}
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
	for (int i = 0; i < (int)linComb.size(); i++) thisComb[i] = limits[i].first;
	bool flags = 1;
	int mainProfit = 0;

	while (flags) {
		vector<int> implication(names.size(), 0);
		int impliedProfit = 0;

		int i = 0;
		auto itr = linComb.begin();
		while (i < (int)linComb.size())
		{
			impliedProfit += profits[i][thisComb[i]];
			for (int j = 0; j < (int)names.size(); j++) implication[j] += thisComb[i]*itr.key()[j];
			++itr; i++;
		}
		bool acceptable = 1;

		for (int j = 0; j < (int)names.size(); j++) if (implication[j] != 0) acceptable = 0;
		if (acceptable && mainProfit < impliedProfit) {
			mainProfit = impliedProfit;
			finale = thisComb;
		}
		i = linComb.size() - 1;
		while (i >= 0 && thisComb[i] == limits[i].second) {
			thisComb[i] = limits[i].first; i--;
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
	for (int i = (int)history.size() - 1; i >= 0; i--) {
		auto itr = linComb.begin();
		int j = 0;
		while (!itr.isNull()) 
		{
			if (itr.key() == history[i].first && finale[j] != INT_MIN) {
				auto& Dict_J = itr.val().first;
				auto& Dict_K = itr.val().second;

				if (finale[j] < 0) {
					auto ktr = Dict_K.end();
					int y = 0;
					while (!ktr.isNull()) {
						if (y + ktr.val() <= -finale[j]) {
							y += ktr.val();
							if (ktr.val() > 0) cout << history[i].second << ktr.key() << " " << ktr.val() << " s#\n";
							auto ltr = ktr;
							--ktr;
							Dict_K.remove(ltr.key());
						} else {
							break;
						}
					}
					auto jtr = Dict_J.begin();
					if (!ktr.isNull()) {
						int jIn = 0;
						int kIn = -finale[j] - y;
						while (!jtr.isNull() && !ktr.isNull() && ktr.key() >= jtr.key()) {
							kIn++;
							if (kIn >= ktr.val()) {
								auto ltr = ktr;
								ktr--; kIn = 0;
								if (ltr.val() > 0) cout << history[i].second << ltr.key() << " " << ltr.val() << " s#\n";
								Dict_K.remove(ltr.key());
							}
							jIn++;
							if (jIn >= jtr.val()) {
								auto ltr = jtr;
								jtr++; jIn = 0;
								if (ltr.val() > 0) cout << history[i].second << ltr.key() << " " << ltr.val() << " b#\n";
								Dict_J.remove(ltr.key());
							}
						}
						if (!jtr.isNull()) {
							if (jIn > 0) cout << history[i].second << jtr.key() << " " << jIn << " b#\n";
							jtr.val() -= jIn;
							if (jtr.val() == 0) Dict_J.remove(jtr.key());
						}
						if (!ktr.isNull()) {
							if (kIn > 0) cout << history[i].second << ktr.key() << " " << kIn << " s#\n";
							ktr.val() -= kIn;
							if (ktr.val() == 0) Dict_K.remove(ktr.key());
						}
					}
				}
				else if (finale[j] > 0) {
					auto jtr = Dict_J.begin();
					int y = 0;
					while (!jtr.isNull()) {
						if (y + jtr.val() <= finale[j]) {
							y += jtr.val();
							if (jtr.val() > 0) cout << history[i].second << jtr.key() << " " << jtr.val() << " b#\n";
							auto ltr = jtr;
							++jtr;
							Dict_J.remove(ltr.key());
						} else {
							break;
						}
					}
					auto ktr = Dict_K.end();
					if (!jtr.isNull()) {
						int kIn = 0;
						int jIn = finale[j] - y;
						while (!jtr.isNull() && !ktr.isNull() && ktr.key() >= jtr.key()) {
							kIn++;
							if (kIn >= ktr.val()) {
								auto ltr = ktr;
								ktr--; kIn = 0;
								if (ltr.val() > 0) cout << history[i].second << ltr.key() << " " << ltr.val() << " s#\n";
								Dict_K.remove(ltr.key());
							}
							jIn++;
							if (jIn >= jtr.val()) {
								auto ltr = jtr;
								jtr++; jIn = 0;
								if (ltr.val() > 0) cout << history[i].second << ltr.key() << " " << ltr.val() << " b#\n";
								Dict_J.remove(ltr.key());
							}
						}
						if (!jtr.isNull()) {
							if (jIn > 0) cout << history[i].second << jtr.key() << " " << jIn << " b#\n";
							jtr.val() -= jIn;
							if (jtr.val() == 0) Dict_J.remove(jtr.key());
						}
						if (!ktr.isNull()) {
							if (kIn > 0) cout << history[i].second << ktr.key() << " " << kIn << " s#\n";
							ktr.val() -= kIn;
							if (ktr.val() == 0) Dict_K.remove(ktr.key());
						}
					}
				}
				else {
					auto jtr = Dict_J.begin();
					auto ktr = Dict_K.end();
					int kIn = 0;
					int jIn = 0;
					while (!jtr.isNull() && !ktr.isNull() && ktr.key() >= jtr.key()) {
						kIn++;
						if (kIn >= ktr.val()) {
							auto ltr = ktr;
							ktr--; kIn = 0;
							if (ltr.val() > 0) cout << history[i].second << ltr.key() << " " << ltr.val() << " s#\n";
							Dict_K.remove(ltr.key());
						}
						jIn++;
						if (jIn >= jtr.val()) {
							auto ltr = jtr;
							jtr++; jIn = 0;
							if (ltr.val() > 0) cout << history[i].second << ltr.key() << " " << ltr.val() << " b#\n";
							Dict_J.remove(ltr.key());
						}
					}
					if (!jtr.isNull()) {
						if (jIn > 0) cout << history[i].second << jtr.key() << " " << jIn << " b#\n";
						jtr.val() -= jIn;
						if (jtr.val() == 0) Dict_J.remove(jtr.key());
					}
					if (!ktr.isNull()) {
						if (kIn > 0) cout << history[i].second << ktr.key() << " " << kIn << " s#\n";
						ktr.val() -= kIn;
						if (ktr.val() == 0) Dict_K.remove(ktr.key());
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
	if (argc != 2) throw invalid_argument("\nUsage: ./trader <1, 2 or 3>");
	Receiver rcv;
	int i = 0;
	string message = rcv.readIML();
	string order = "";//stores the order
	bool flags = 1;

	// For part 1 -- Buy Low, Sell High
	dict<string, stockLowHigh> stocksLowHigh;

	// For part 2 -- Arbitrage
	vector<stock> stocksArbitrage(1, {"", 0});
	ptr<Pair<vector<int>, int8_t>> people = nullptr;
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
		
		while (i < (int)message.length() && (message[i] != 13 && message[i] != 0 && message[i] != 10)) order += message[i++];
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
			while (i < (int)message.length() && (message[i] == 13 || message[i] == 0 || message[i] == 10)) i++;
			if (i == (int)message.length()) {
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
