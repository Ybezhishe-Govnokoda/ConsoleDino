#pragma once
template <typename T>
class List
{
public:
	List();
	~List();

	void pop_front();
	void push_back(T data);
	int get_size();
	void clear() { while (size) pop_front(); }
	T &operator[](const int index);

private:

	template <typename T>
	struct Node
	{
		Node *pNext;
		T data;

		Node(T data = T(), Node *pNext = nullptr)
		{
			this->data = data;
			this->pNext = pNext;
		}
	};
	int size;
	Node<T> *head;
};


template <typename T>
List<T>::List()
{
	head = nullptr;
	size = 0;
}

template <typename T>
List<T>::~List()
{
	while (size) pop_front();
}

template <typename T>
void List<T>::push_back(T data)
{
	if (head == nullptr)
		head = new Node<T>(data);
	else
	{
		Node<T> *current = this->head;

		while (current->pNext != nullptr)
			current = current->pNext;

		current->pNext = new Node<T>(data);
	}
	size++;
}

template <typename T>
void List<T>::pop_front()
{
	Node<T> *temp = head;
	head = head->pNext;
	delete temp;

	size--;
}

template <typename T>
int List<T>::get_size()
{
	return size;
}

template <typename T>
T &List<T>::operator[](const int index)
{
	Node<T> *current = this->head;

	for (int i = 0; current != nullptr; i++)
	{
		if (i == index)
			return current->data;

		current = current->pNext;
	}
}