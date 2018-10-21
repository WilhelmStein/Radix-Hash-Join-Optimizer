#pragma once

namespace nstd {

	template<typename T>
	class List {
		//Inner Classes
	public:
		class Node {
			friend class List<T>;
			Node* next;
			Node* prev;
		public:
			T contents;

			Node(Node* _next, Node* _prev, T* _item) :next(_next), prev(_prev), contents(*_item) {}
			~Node() {}

			T* operator->() { return &contents; }
		};

		class Iterator {
			Node* ptr;
		public:

			Iterator(Node* _ptr = nullptr) : ptr(_ptr) {}
			Iterator(Node& _ptr) : ptr((&_ptr)) {}
			~Iterator() {}
			Iterator operator ++ (int)
			{
				Iterator temp(ptr);
				ptr = ptr->next;
				return temp;
			}

			Iterator& operator ++ () { (ptr = ptr->next); return *this; }
			Iterator operator -- (int)
			{
				Iterator temp(ptr);
				ptr = ptr->prev;
				return temp;
			}

			Iterator& operator -- () { (ptr = ptr->prev); return *this; }
			Node& operator->() const { return *ptr; }
			Iterator& operator = (const Node& i) { return Iterator(i); }
			bool operator!= (const Node* b) { return (ptr != b); }
			bool operator== (const Node* b) { return (ptr == b); }

			Iterator operator+(int moves)
			{
				Node* temp = ptr;
				for (int i = 0; i < moves; i++)
					temp = temp->next;
				return Iterator(temp);
			}

			T* getT() const { return ((ptr) ? (&(ptr->contents)) : (nullptr)); }
			Node* getNode() const { return ptr; }
		};
	private:
		//Members
		Node * head;
		Node* tail;
		unsigned int count;
	public:
		List() : head(nullptr), tail(nullptr), count(0) {}
		~List()  { while (!isEmpty()) popHead(); }
		List(List<T>& l);

		T& getHead() { return head->contents; }
		T& getTail() { return tail->contents; }

		Node* begin() { return head; }
		Node* end() { return nullptr; }

		void append(T* item);
		void popTail();
		void popHead();
        
		bool isEmpty() { return ((this->count) ? (false) : (true)); }
		unsigned int size() { return this->count; }
	};

	template<typename T>
	inline List<T>::List(List<T> & l)//ensure deep copy when copying lists
	{
		head = tail = nullptr;
		count = 0;

		Node* lptr = l.head;

		while(lptr != nullptr) {
			append(&lptr->contents);
			lptr = lptr->next;
		}
	}

	//Implementations
	template<typename T>
	inline void List<T>::append(T* item)
	{
		if (isEmpty())
		{
			tail = new Node(nullptr, nullptr, item);
			head = tail;
		}
		else
		{
			tail->next = new Node(nullptr, tail, item);
			tail = tail->next;
		}
		count++;
	}

	template<typename T>
	inline void List<T>::popTail()
	{
		if(isEmpty())
			return;
		Node *temp = tail->prev;
		(temp == nullptr) ? (head = nullptr) : (temp->next = nullptr);
		delete tail;
		tail = temp;
		count--;
	}

	template<typename T>
	void List<T>::popHead()
	{
		if (isEmpty())
			return;
		Node *temp = head->next;
		(temp == nullptr) ? (tail = nullptr) : (temp->prev = nullptr);
		delete head;
		head = temp;
		count--;
	}
}