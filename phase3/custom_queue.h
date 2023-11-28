#define CONST_GROWTH 10
#define INIT_SIZE 10

template <typename T> class Queue {
private:
  T *A; // the array used for implementing the dynamic dequeue
  unsigned int N; // the current size of array A
  unsigned int head; // index where next element of queue will be deleted from
  unsigned int tail; // index where next element will be inserted
  unsigned int nextSize() { return N+CONST_GROWTH; }
public:
  Queue() // default constructor
  {
    A = new T[INIT_SIZE];
    if (A != nullptr) N = INIT_SIZE;
    else N = 0;
    head = tail = 0;
  } 
  ~Queue(){delete[] A;}; // default destructor
  T Qfront(){
    return A[head];
  }
  T Qback(){
    return A[tail];
  }
  bool isEmpty(); // is the queue empty?
  bool isFull(); // is the queue full?
  void grow(); // grow the queue to its next size 
  unsigned int size(); // return the current number of elements in the queue
  void QInsert(T x); // insert given element at tail of the queue; grow array size as necessary
  bool QDelete(T *x); // delete element at head of the queue and place it in *T; returns false if queue is empty, true otherwise
};

template <typename T> bool Queue<T> :: isEmpty() {
  return head==tail;
}

template <typename T> bool Queue<T> :: isFull() {
  return size()==N-1;
}

template <typename T> void Queue<T> :: grow() {
T *B = new T[nextSize()];
int pointer=head;
for(int i=0; i<size(); i++)
{
	B[i]=A[pointer];
	pointer=(pointer+1)%N;
}
delete[] A;
A=B;
head=0; tail=N-1;
N=nextSize();
}

template <typename T> unsigned int Queue<T> :: size() {
  return (N+tail-head)%N;
}

template <typename T> void Queue<T> :: QInsert(T x) {
if(isFull()) grow();
A[tail]=x;
tail=(tail+1)%N;
}

template <typename T> bool Queue<T> :: QDelete(T* x) {
if(isEmpty()) return false;
*x=A[head];
head=(head+1)%N;
return true;
}

