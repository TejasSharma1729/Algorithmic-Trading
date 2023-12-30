#include <iostream>
#include <string>
#include <vector>
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

/*
template <typename T, typename U> class dict {
public:
	dict();
	operator = (dict& Oth);
	dict(dict& Oth);
	~dict();
		// Takes care of destruction of dictionary itself.
	void insert(T key, U val); 
		// Error if double insertion
	iterator find(T key); 
		// Null iterator if not found.
	iterator begin(); 
		// Smallest element Null iterator if empty dict. ++ rightwards.
	iterator end(); 
		// Use -- to go leftwards. Not ++. Also this is rbegin (largest element) not outside.
	U at(T key);
	U& operator [] (T key); 
		// Not found: nitializes to 0 and accepts not-in-dict values.
	U remove(T key);
	int size();
	bool empty();

	class iterator {
	private:
		rbNode* node;
	public:
		T& key();
		U& val();
		bool isNull(); 
			// Forward/Backward out of range etc.
		bool isBegin(); 
			// 1 if null or smallest element.
		bool isEnd();
		operator ++, --, +=, -=; 
			// forward 1, backward 1, (n times)
	};
	// Works well and iterates in order (when ++, -- ..) after insertion and deletion too.
	// WARNING: Undefined Behaviour of Iterator when it points to element just deleted.

private:
	struct rbNode {
		T key;
		U val;
		bool color; // 0{Black} 1{Red}
		rbNode* left;
		rbNode* right;
		rbNode* parent;
	};
	rbNode* root;
	int length;
};
// IT IS SINGLEMAP, WELL ORDERED KEY WITH == AND < DEFINED AND ANY NON-VOID VALUE 
// SUGGESTIONS: Set: boolean value (false?) or int8_t;   Multiset: int/uint (for no. of entries), remove if 0, first insertion 1;
// Map: the usual value_datatype;   Multimap: vector<value_datatype>, remove if empty, first insertion add element.
*/

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

	rbNode* root = nullptr;
	int length = 0;

public:
	dict() {root = nullptr; length = 0;}
	dict(dict& Oth) {
		length = Oth.length;
		if (root == nullptr || Oth.root == root) return; else root->destroy();
		if (Oth.root != nullptr) root = Oth.root->copy();
		else root = nullptr;
	}
	void operator = (dict& Oth) {
		length = Oth.length;
		if (root == nullptr || Oth.root == root) return; else root->destroy();
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
