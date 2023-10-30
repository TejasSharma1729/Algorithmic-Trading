/*
template <typename T, typename U> 
class minHeap {
	minHeap();
	minHeap(T imp, U val);
	~minHeap();
	void enqueue(T imp, U val);
	pair<T, U> front();
	pair<T, U> dequeue();
	int size();
	bool empty();
}
template <typename T, typename U> 
class maxHeap {
	maxHeap();
	maxHeap(T imp, U val);
	~maxHeap();
	void enqueue(T imp, U val);
	pair<T, U> front();
	pair<T, U> dequeue();
	int size();
	bool empty();
}
*/

#include <cstdlib>
#include <iostream>
#include <utility>
using namespace std;

template <typename T, typename U>
class minHeap {
private:
	struct entity {
		T priority;
		long history;
		U data;
	
		int operator < (entity B) {
			if (priority < B.priority) return 1;
			if (priority == B.priority && history < B.history) return 1;
			return 0;
		}
		int operator > (entity B) {
			if (priority > B.priority) return 1;
			if (priority == B.priority && history > B.history) return 1;
			return 0;
		}
		// No two entities (within a queue) can be identical. All have different history.
		// Entity with LOWER priority goes to top first. Use -ve if you want high priority first.
		// If two entities have same priority then one with lower history goes to front first.
		// History: unique, presered even after entity is deleted.
	};
	entity *entities;
	int length;
	int max;
	long record;
	
public:
	minHeap() {
		entities = new entity [1];
		entities[0].priority = 0;
		entities[0].history = 0;
		max = 2;
		length = 0;
		record = 0;
	}
	minHeap(T imp, U val) {
		entities = new entity [2];
		entities[0].priority = 0;
		entities[0].history = 0;
		entities[1].priority = imp;
		entities[1].history = 1;
		entities[1].data = val;
		record = 1;
		length = 1;
		max = 2;
	}
	~minHeap() {
		delete [] entities;
	}
	
	pair<T, U> front() {
		if (length == 0) throw invalid_argument("\nEmpty Queue Error");
		pair<T, U> top (entities[1].priority, entities[1].data);
		return top;
	}

	int size() {
		return length;
	}
	bool empty() {
		return (length == 0);
	}

	void enqueue(T imp, U val) {
		length++;
		record++;
		// Resizing Queue.
		if (length == max)
		{
			max *= 2;
			entity* temp = new entity [max];
			for (int i = 0; i < length; i++) temp[i] = entities[i];
			for (int i = length; i < max; i++) temp[i].priority = 0;
			delete [] entities;
			entities = temp;
		}
		entities[length].priority = imp;
		entities[length].history = record;
		entities[length].data = val;

		// OK. Now the entity is added. Maintaining heap integrity.
		int loc = length;
		while (loc > 1 && entities[loc] < entities[loc/2])
		{
			entity temp = entities[loc];
			entities[loc] = entities[loc/2];
			entities[loc/2] = temp;
			loc /= 2;
		}
	}

	pair<T, U> dequeue() {
		if (length == 0) throw invalid_argument("\nEmpty Queue Error");
		pair<T, U> top (entities[1].priority, entities[1].data);

		// Swap top and bottomright(last). Delete last (== deleting front) return front().
		entity temp = entities[1];
		entities[1] = entities[length];
		entities[length--].priority = 0;

		// Update size of array;
		if (length+1 == max/4) {
			max /= 2;
			entity* temp1 = new entity [max];
			for (int i = 0; i <= length; i++) temp1[i] = entities[i];
			for (int i = length+1; i < max; i++) temp1[i].priority = 0;
			delete [] entities;
			entities = temp1;
		}

		// Integrity of heap
		int loc = 1;
		while ((2*loc <= length && entities[loc] > entities[2*loc]) || (2*loc < length && entities[loc] > entities[2*loc + 1]))
		{
			if (2*loc == length || entities[2*loc] < entities[2*loc + 1]) {
				entity temp1 = entities[loc];
				entities[loc] = entities[2*loc];
				entities[2*loc] = temp1;
				loc = 2*loc;
			}
			else {
				entity temp1 = entities[loc];
				entities[loc] = entities[2*loc + 1];
				entities[2*loc + 1] = temp1;
				loc = 2*loc + 1;
			}
		}
		return top;
	}
};

template <typename T, typename U>
class maxHeap {
private:
	struct entity {
		T priority;
		long history;
		U data;
	
		int operator < (entity B) {
			if (priority > B.priority) return 1;
			if (priority == B.priority && history < B.history) return 1;
			return 0;
		}
		int operator > (entity B) {
			if (priority < B.priority) return 1;
			if (priority == B.priority && history > B.history) return 1;
			return 0;
		}
		// No two entities (within a queue) can be identical. All have different history.
		// Entity with LOWER priority goes to top first. Use -ve if you want high priority first.
		// If two entities have same priority then one with lower history goes to front first.
		// History: unique, presered even after entity is deleted.
	};
	entity *entities;
	int length;
	int max;
	long record;
	
public:
	maxHeap() {
		entities = new entity [1];
		entities[0].priority = 0;
		entities[0].history = 0;
		max = 2;
		length = 0;
		record = 0;
	}
	maxHeap(T imp, U val) {
		entities = new entity [2];
		entities[0].priority = 0;
		entities[0].history = 0;
		entities[1].priority = imp;
		entities[1].history = 1;
		entities[1].data = val;
		record = 1;
		length = 1;
		max = 2;
	}
	~maxHeap() {
		delete [] entities;
	}
	
	pair<T, U> front() {
		if (length == 0) throw invalid_argument("\nEmpty Queue Error");
		pair<T, U> top (entities[1].priority, entities[1].data);
		return top;
	}

	int size() {
		return length;
	}
	bool empty() {
		return (length == 0);
	}

	void enqueue(T imp, U val) {
		length++;
		record++;
		// Resizing Queue.
		if (length == max)
		{
			max *= 2;
			entity* temp = new entity [max];
			for (int i = 0; i < length; i++) temp[i] = entities[i];
			for (int i = length; i < max; i++) temp[i].priority = 0;
			delete [] entities;
			entities = temp;
		}
		entities[length].priority = imp;
		entities[length].history = record;
		entities[length].data = val;

		// OK. Now the entity is added. Maintaining heap integrity.
		int loc = length;
		while (loc > 1 && entities[loc] < entities[loc/2])
		{
			entity temp = entities[loc];
			entities[loc] = entities[loc/2];
			entities[loc/2] = temp;
			loc /= 2;
		}
	}

	pair<T, U> dequeue() {
		if (length == 0) throw invalid_argument("\nEmpty Queue Error");
		pair<T, U> top (entities[1].priority, entities[1].data);

		// Swap top and bottomright(last). Delete last (== deleting front) return front().
		entity temp = entities[1];
		entities[1] = entities[length];
		entities[length--].priority = 0;

		// Update size of array;
		if (length+1 == max/4) {
			max /= 2;
			entity* temp1 = new entity [max];
			for (int i = 0; i <= length; i++) temp1[i] = entities[i];
			for (int i = length+1; i < max; i++) temp1[i].priority = 0;
			delete [] entities;
			entities = temp1;
		}

		// Integrity of heap
		int loc = 1;
		while ((2*loc <= length && entities[loc] > entities[2*loc]) || (2*loc < length && entities[loc] > entities[2*loc + 1]))
		{
			if (2*loc == length || entities[2*loc] < entities[2*loc + 1]) {
				entity temp1 = entities[loc];
				entities[loc] = entities[2*loc];
				entities[2*loc] = temp1;
				loc = 2*loc;
			}
			else {
				entity temp1 = entities[loc];
				entities[loc] = entities[2*loc + 1];
				entities[2*loc + 1] = temp1;
				loc = 2*loc + 1;
			}
		}
		return top;
	}
};
