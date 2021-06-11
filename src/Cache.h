#ifndef CACHE_H
#define CACHE_H

#include "main.h"

class Node
{
public:
	Elem *data;
	Node *left;
	Node *right;
	int height;
	Node(Elem *data)
	{
		this->data = data;
		this->left = NULL;
		this->right = NULL;
		this->height = 1;
	}
};

int height(Node *N)
{
	if (N == NULL)
		return 0;
	return N->height;
}


Node *rightRotate(Node *y)
{
	Node *x = y->left;
	Node *T2 = x->right;

	x->right = y;
	y->left = T2;

	y->height = max(height(y->left),
					height(y->right)) +
				1;
	x->height = max(height(x->left),
					height(x->right)) +
				1;

	return x;
}

Node *leftRotate(Node *x)
{
	Node *y = x->right;
	Node *T2 = y->left;

	y->left = x;
	x->right = T2;

	x->height = max(height(x->left),
					height(x->right)) +
				1;
	y->height = max(height(y->left),
					height(y->right)) +
				1;

	return y;
}

int getBalanceFactor(Node *N)
{
	if (N == NULL)
		return 0;
	return height(N->left) - height(N->right);
}



int max(int a, int b){
	if (a>=b) return a;
	else return b;
}
Node *insertNode(Node *node, Elem *data)
{

	if (node == NULL)
		return new Node(data);

	if (data->addr < node->data->addr)
		node->left = insertNode(node->left, data);
	else if (data->addr > node->data->addr)
		node->right = insertNode(node->right, data);
	else
		return node;

	node->height = max(height(node->left),
					   height(node->right)) +
				   1;

	int bFactor = getBalanceFactor(node);

	if (bFactor > 1 && data->addr < node->left->data->addr)
		return rightRotate(node);

	if (bFactor < -1 && data->addr > node->right->data->addr)
		return leftRotate(node);

	if (bFactor > 1 && data->addr > node->left->data->addr)
	{
		node->left = leftRotate(node->left);
		return rightRotate(node);
	}

	if (bFactor < -1 && data->addr < node->right->data->addr)
	{
		node->right = rightRotate(node->right);
		return leftRotate(node);
	}

	return node;
}

Node *minValueNode(Node *node)
{
	Node *current = node;

	while (current->left != NULL)
		current = current->left;

	return current;
}

Node *deleteNodeAVL(Node *root, int data)
{
	if (root == NULL)
		return root;

	if (data < root->data->addr)
		root->left = deleteNodeAVL(root->left, data);
	else if (data > root->data->addr)
		root->right = deleteNodeAVL(root->right, data);
	else
	{
		if (!root->left || !root->right)
		{
			Node *temp = root->left ? root->left : root->right;

			if (temp == NULL)
			{
				temp = root;
				root = NULL;
			}
			else
				*root = *temp;
			delete (temp);
		}
		else
		{
			Node *temp = minValueNode(root->right);
			root->data = temp->data;
			root->right = deleteNodeAVL(root->right, temp->data->addr);
		}
	}

	if (root == NULL)
		return root;
	root->height = 1 + max(height(root->left), height(root->right));
	int bFactor = getBalanceFactor(root);

	if (bFactor < -1 && getBalanceFactor(root->right) > 0)
	{
		root->right = rightRotate(root->right);
		return leftRotate(root);
	}
	return root;

	if (bFactor < -1 && getBalanceFactor(root->right) <= 0)
		return leftRotate(root);

	if (bFactor > 1 && getBalanceFactor(root->left) < 0)
	{
		root->left = leftRotate(root->left);
		return rightRotate(root);
	}

	if (bFactor > 1 && getBalanceFactor(root->left) >= 0)
		return rightRotate(root);
}

class ReplacementPolicy
{
protected:
	int size;
	Elem **slot;
	bool full()
	{
		return (this->size == MAXSIZE);
	}
	virtual Elem *insert(Elem *elem, int index) = 0;
	virtual int remove() = 0;
	virtual void replace(int index, Elem *elem) {}

public:
	virtual ~ReplacementPolicy(){};
	virtual void print() = 0;
	virtual void visit(int index){};
	virtual void visit(Elem *elem){};
	Data *read(int addr)
	{
		for (int i = 0; i < this->size; i++)
		{
			if (this->slot[i]->addr == addr)
			{
				Data *ret = this->slot[i]->data;
				this->visit(new Elem(addr, NULL, true));
				return ret;
			}
		}
		return NULL;
	}

	Elem *put(int addr, Data *cont)
	{
		Elem *elem = new Elem(addr, cont, true);
		elem = insert(elem, 0);
		if (elem != NULL)
			elem->sync = true;
		return elem;
	}

	Elem *write(int addr, Data *cont, bool &duplicated)
	{
		bool found = false;
		for (int i = 0; i < this->size; i++)
		{
			if (this->slot[i]->addr == addr)
			{
				slot[i]->data = cont;
				slot[i]->sync = false;
				duplicated = true;
				found = true;
				break;
			}
		}

		if (!found)
		{
			Elem *newElem = new Elem(addr, cont, false);
			newElem = insert(newElem, 0);
			if (newElem != NULL)
				newElem->sync = true;
			return newElem;
		}
		return NULL;
	}
};

class FIFO : public ReplacementPolicy
{

private:
	int front, rear;

public:
	FIFO()
	{
		this->front = 0;
		this->rear = MAXSIZE - 1;
		this->size = 0;
		this->slot = new Elem *[MAXSIZE];
	}
	Elem *insert(Elem *elem, int index)
	{

		if (this->full())
		{
			Elem *ret;
			ret = this->slot[this->front];
			this->slot[this->front] = elem;
			this->front = (this->front + 1) % MAXSIZE;
			return ret;
		}
		else
		{

			this->rear = (this->rear + 1) % MAXSIZE;
			this->slot[this->rear] = elem;
			this->size++;
			return NULL;
		}
	}

	int remove()
	{
		return 0;
	}
	void print()
	{
		int cur = this->front;

		for (int i = 0; i < this->size; i++)
		{
			this->slot[cur]->print();
			if (cur < this->size - 1)
				cur++;
			else
				cur = 0;
		}
	}
};

class MRU : public ReplacementPolicy
{
protected:
	int counter;
	int *index;

public:
	MRU()
	{
		this->size = 0;
		this->slot = new Elem *[MAXSIZE];
		this->counter = 0;
		this->index = new int[MAXSIZE];
		for (int i = 0; i < MAXSIZE; i++)
			index[i] = 0;
	}
	~MRU()
	{
		delete index;
	}

	void replace(int index, Elem *elem)
	{
		visit(index);
		slot[index] = elem;
	}
	void visit(int index)
	{
		this->index[index] = ++counter;
	}
	Elem *insert(Elem *elem, int index)
	{
		Elem *ret;
		if (this->full())
		{
			int rem = this->remove();
			ret = this->slot[rem];
			this->replace(rem, elem);
		}
		else
		{
			this->replace(this->size, elem);
			ret = NULL;
			this->size++;
		}
		return ret;
	}

	void visit(Elem *ele)
	{
		for (int i = 0; i < this->size; i++)
			if (ele->addr == this->slot[i]->addr)
			{
				visit(i);
				break;
			}
	}

	int remove()
	{
		for (int i = 0; i < MAXSIZE; i++)
			if (this->counter == index[i])
				return i;
		return 0;
	}
	void print()
	{
		if (this->size == 0)
			return;
		int cur = 0;
		int runner = this->counter;
		while (cur < this->size)
		{
			for (int i = 0; i < this->size; i++)
			{
				if (this->index[i] == runner)
				{
					this->slot[i]->print();
					cur++;
					break;
				}
			}
			runner--;
		}
	}
};
class LRU : public MRU
{
public:
	int remove() 
	override
	{
		int minCount = 0;
		for (int i = 1; i < MAXSIZE; i++)
			if (this->index[minCount] > this->index[i])
				minCount = i;
		return minCount;
	}
};

class LFU : public ReplacementPolicy
{
private:
	int *frequency;
	int *index;
	int counter;

public:
	LFU()
	{
		this->frequency = new int[MAXSIZE];
		this->index = new int[MAXSIZE];
		this->size = 0;
		this->slot = new Elem *[MAXSIZE];
		this->counter = 0;
		for (int i = 0; i < MAXSIZE; i++)
		{
			index[i] = 0;
			frequency[i] = 0;
		}
	}
	
	~LFU()
	{
		delete frequency;
		delete index;
	}
	void replace(int index, Elem *elem)
	{
		visit(index);
		slot[index] = elem;
	}
	void visit(int index)
	{
		this->frequency[index] = this->frequency[index] + 1;
		this->index[index] = counter + 1;
		this->counter++;
	}
	Elem *insert(Elem *elem, int index)
	{
		if (full())
		{
			Elem *ret;
			int removeIndex = remove();
			ret = slot[removeIndex];
			slot[removeIndex] = slot[size - 1];
			frequency[removeIndex] = frequency[size - 1];
			--size;
			downHeap(removeIndex);
			++size;
			replace(size - 1, elem);
			frequency[size - 1] = 1;
			upHeap(size - 1);
			return ret;
		}
		else
		{
			replace(size, elem);
			size++;
			upHeap(size - 1);
		}
		return NULL;
	}

	void visit(Elem *elem)
	{
		for (int i = 0; i < this->size; i++)
			if (this->slot[i]->addr == elem->addr)
			{
				visit(i);
				downHeap(i);
				break;
			}
	}
	
	void print()
	{

		int i = 0;
		while (i < this->size)
		{
			this->slot[i++]->print();
		}
	}
	
	int remove()
	{
		return 0;
	}
	void upHeap(int index)
	{
		int item = frequency[index];
		Elem *_elem = this->slot[index];
		int i = index;
		int freq;
		Elem *temp;
		int leftChild = (i % 2 == 0) ? (i / 2 - 1) : i / 2;
		bool check = false;
		while (leftChild >= 0)
		{
			freq = frequency[leftChild];
			temp = this->slot[leftChild];
			if (item >= freq)
			{
				frequency[i] = item;
				this->slot[i] = _elem;
				check = true;
				break;
			}
			else
			{
				frequency[i] = freq;
				this->slot[i] = temp;
				i = leftChild;
			}
			leftChild = (i % 2 == 0) ? (i / 2 - 1) : i / 2;
		}
		if (!check)
		{
			frequency[i] = item;
			this->slot[i] = _elem;
		}
	}

	void downHeap(int index)
	{
		int item = frequency[index];
		Elem *_elem = this->slot[index];
		int i = index;
		int freq;
		Elem *temp;

		bool check = false;
		while (2 * i + 1 < this->size)
		{
			int leftChild = 2 * i + 1;
			freq = frequency[leftChild];
			temp = this->slot[leftChild];
			if (2 * i + 2 < this->size)
			{
				if (freq > frequency[leftChild + 1])
					freq = frequency[++leftChild];
				temp = this->slot[leftChild];
			}
			if (item < freq)
			{
				check = true;
				frequency[i] = item;
				this->slot[i] = _elem;

				break;
			}
			else
			{
				frequency[i] = freq;
				this->slot[i] = temp;
				i = leftChild;
			}
		}
		if (!check)
		{
			frequency[i] = item;
			this->slot[i] = _elem;
		}
	}

};

class SearchEngine
{
public:
	virtual ~SearchEngine(){};
	virtual void print() = 0;
	virtual void write(int add, Data *cont) = 0;
	virtual void insert(Elem *data, int index) = 0;
	virtual void deleteNode(int data) = 0;
};

class DBHashing : public SearchEngine
{
public:
	enum STT_TYPES
	{
		_NOT_EMPTY,
		_DELETED,
		_NOT_IN_LIST,
	};

private:
	int (*h1)(int);
	int (*h2)(int);
	int size;
	STT_TYPES *current_status;
	Elem **map;

public:
	;
	DBHashing(int (*h1)(int), int (*h2)(int), int size)
	{
		this->size = size;
		this->current_status = new STT_TYPES[size];
		this->h1 = h1;
		this->h2 = h2;
		map = new Elem *[size];
		for (int i = 0; i < size; ++i)
		{
			this->current_status[i] = _NOT_IN_LIST;
			map[i] = nullptr;
		}
	}
	~DBHashing()
	{
		delete current_status;
	}
	void insert(Elem *data, int iterator)
	{

		for (iterator = 0; iterator < this->size; iterator++)
		{
			int index = (h1(data->addr) + iterator * h2(data->addr)) % size;
			if (this->current_status[index] != _NOT_EMPTY)
			{
				this->map[index] = data;
				this->current_status[index] = _NOT_EMPTY;
				return;
			}
		}
	}
	void deleteNode(int add)
	{
		for (int i = 0; i < this->size; i++)
		{
			int index = (h1(add) + i * h2(add)) % this->size;
			if (this->map[index]->addr == add)
			{
				this->current_status[index] = _DELETED;
				return;
			}
			else if (this->current_status[index] != _NOT_IN_LIST)
				return;

			continue;
		}
	}
	void write(int address, Data *dataToWrite)
	{
		for (int i = 0; i < this->size; ++i)
		{
			if (this->current_status[i] == _NOT_EMPTY)
				if (map[i]->addr == address)
				{
					map[i]->sync = false;
					map[i]->data = dataToWrite;
					break;
				}
		}
	}
	void print()
	{
		cout << "Prime memory:" << endl;
		for (int i = 0; i < this->size; ++i)
			if (this->current_status[i] == _NOT_EMPTY)
				map[i]->print();
	}
};

class AVL : public SearchEngine
{
	Node *root;

public:
	AVL()
	{
		this->root = NULL;
	}
	~AVL()
	{
		delete root;
	}
	void deleteNode(int data)
	{
		root = deleteNodeAVL(root, data);
	}
	void insert(Elem *data, int i)
	{
		this->root = insertNode(this->root, data);
	}
	void print()
	{
		cout << "Print AVL in inorder:" << endl;
		inOrder(this->root);
		cout << "Print AVL in preorder:" << endl;
		preOrder(this->root);
	}
	void write(int address, Data *dataToWrite)
	{
		Node *cur = this->root;
		while (cur != NULL)
		{
			if (cur->data->addr == address)
			{
				cur->data->data = dataToWrite;
				cur->data->sync = false;
				break;
			}
			else if (cur->data->addr > address) // Move left
				cur = cur->left;
			else
				cur = cur->right; //Move right
		}
	}

	void inOrder(Node *root)
	{
		if (!root)
			return;
		inOrder(root->left);
		root->data->print();
		inOrder(root->right);
	}
	void preOrder(Node *root)
	{
		if (!root)
			return;
		root->data->print();
		preOrder(root->left);
		preOrder(root->right);
	}
};

#endif