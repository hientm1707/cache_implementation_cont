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

int max(int a, int b)
{
	return (a > b) ? a : b;
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

int getBalance(Node *N)
{
	if (N == NULL)
		return 0;
	return height(N->left) - height(N->right);
}

Node *insertNode(Node *node, Elem *data)
{

	if (node == NULL)
		return (new Node(data));

	if (data->addr < node->data->addr)
		node->left = insertNode(node->left, data);
	else if (data->addr > node->data->addr)
		node->right = insertNode(node->right, data);
	else
		return node;

	node->height = 1 + max(height(node->left),
						   height(node->right));

	int balance = getBalance(node);

	if (balance > 1 && data->addr < node->left->data->addr)
		return rightRotate(node);

	if (balance < -1 && data->addr > node->right->data->addr)
		return leftRotate(node);

	if (balance > 1 && data->addr > node->left->data->addr)
	{
		node->left = leftRotate(node->left);
		return rightRotate(node);
	}

	if (balance < -1 && data->addr < node->right->data->addr)
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
		if ((root->left == NULL) ||
			(root->right == NULL))
		{
			Node *temp = root->left ? root->left : root->right;

			if (temp == NULL)
			{
				temp = root;
				root = NULL;
			}
			else
				*root = *temp;
			free(temp);
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
	int balance = getBalance(root);

	if (balance < -1 && getBalance(root->right) > 0)
	{
		root->right = rightRotate(root->right);
		return leftRotate(root);
	}
	return root;

	if (balance < -1 && getBalance(root->right) <= 0)
		return leftRotate(root);

	if (balance > 1 && getBalance(root->left) < 0)
	{
		root->left = leftRotate(root->left);
		return rightRotate(root);
	}

	if (balance > 1 && getBalance(root->left) >= 0)
		return rightRotate(root);
}

enum STT_TYPES
{
	NIL,
	UN_EMPTY,
	DELETED
};

class ReplacementPolicy
{
protected:
	int count;
	Elem **slot;

public:
	virtual Elem *insert(Elem *e, int idx) = 0;
	virtual void proceed(int idx) = 0;
	virtual void proceed(Elem *e) = 0;
	virtual int remove() = 0;
	virtual void print() = 0;
	virtual ~ReplacementPolicy() {}
	bool full()
	{
		return (this->count == MAXSIZE);
	}

	void replace(int idx, Elem *e)
	{
		proceed(idx);
		slot[idx] = e;
	}

	Data *read(int addr)
	{
		for (int i = 0; i < this->count; i++)
		{
			if (this->slot[i]->addr == addr)
			{
				Data *ret = this->slot[i]->data;

				this->proceed(new Elem(addr, NULL, true));
				return ret;
			}
		}
		return NULL;
	}

	Elem *put(int addr, Data *cont)
	{
		Elem *e = new Elem(addr, cont, true);
		e = insert(e, 0);
		if (e != NULL)
			e->sync = true;
		return e;
	}

	Elem *write(int addr, Data *cont, bool &duplicated)
	{
		bool found = false;
		for (int i = 0; i < this->count; i++)
		{
			if (this->slot[i]->addr == addr)
			{
				this->slot[i]->data = cont;
				this->slot[i]->sync = false;
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
		this->count = 0;
		this->slot = new Elem *[MAXSIZE];
		
		
	}
	Elem *insert(Elem *e, int idx)
	{

		if (this->full())
		{
			Elem *ret;
			ret = this->slot[this->front];
			this->slot[this->front] = e;
			this->front = (this->front + 1) % MAXSIZE;
			return ret;
		}
		else
		{
			this->rear = (this->rear + 1) % MAXSIZE;
			this->slot[this->rear] = e;
			this->count++;
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

		for(int i = 0; i<this->count;i++)
		{
			this->slot[cur]->print();
			cur = (cur < this->count - 1) ? cur + 1 : 0;
		}
	}


	void proceed(int idx)
	{
	}
	void proceed(Elem *e)
	{
	}
};

class MRU : public ReplacementPolicy
{
protected:
	int activity;
	int *idx;
public:
	MRU()
	{
		count = 0;
		slot = new Elem *[MAXSIZE];
		activity = 0;
		idx = new int[MAXSIZE];
		for (int i = 0; i < MAXSIZE; i++)
			idx[i] = 0;
	}
	~MRU()
	{
		delete idx;
	}
	Elem *insert(Elem *e, int idx)
	{
		Elem *ret;
		if (this->full())
		{
			int re = remove();
			ret = this->slot[re];
			this->replace(re, e);
		}
		else
		{
			this->replace(this->count, e);
			ret = NULL;
			this->count++;
		}
		return ret;
	}
	void proceed(int idx)
	{
		this->idx[idx] = ++activity;
	}
	void proceed(Elem *ele)
	{
		for (int i = 0; i < this->count; i++)
			if (ele->addr == this->slot[i]->addr  )
			{
				proceed(i);
				break;
			}
	}
	int remove()
	{
		for (int i = 0; i < MAXSIZE; i++)
			if (this->activity == idx[i])
				return i;
		return 0;
	}
	void print()
	{
		if (this->count == 0)
			return;
		int cur = 0;
		int run_activity = this->activity;
		while (cur < this->count)
		{
			for (int i = 0; i < this->count; i++)
			{
				if (this->idx[i] == run_activity)
				{
					this->slot[i]->print();
					cur++;
					break;
				}
			}
			run_activity--;
		}
	}
};
class LRU : public MRU
{
public:
	int remove() override
	{
		int min = 0;
		for (int i = 1; i < MAXSIZE; i++)
			if (this->idx[min] > this->idx[i])
				min = i;
		return min;
	}
};

class LFU : public ReplacementPolicy
{
private:
	int *frequency;
	int *idx;
	int activity;
public:
	LFU()
	{
		count = 0;
		slot = new Elem *[MAXSIZE];
		frequency = new int[MAXSIZE];
		idx = new int[MAXSIZE];
		activity = 0;
		for (int i = 0; i < MAXSIZE; i++)
		{
			frequency[i] = 0;
			idx[i] = 0;
		}
	}
	~LFU()
	{
		delete frequency;
		delete idx;
	}
	Elem *insert(Elem *e, int idx)
	{
		Elem *ret;
		if (full())
		{
			int re = remove();
			ret = this->slot[re];
			this->slot[re] = this->slot[this->count - 1];
			this->frequency[re] = this->frequency[this->count - 1];
			this->count--;
			reheapDown(re);
			this->count++;
			replace(this->count - 1, e);
			this->frequency[this->count - 1] = 1;
			reheapUp(this->count - 1);
		}
		else
		{
			replace(this->count, e);
			this->count++;
			ret = NULL;
			reheapUp(this->count - 1);
		}
		return ret;
	}
	void proceed(int idx)
	{
		this->frequency[idx]++;
		this->idx[idx] = ++activity;
	}
	void proceed(Elem *e)
	{
		for (int i = 0; i < this->count; i++)
			if (this->slot[i]->addr == e->addr)
			{
				proceed(i);
				reheapDown(i);
				break;
			}
	}
	int remove()
	{
		return 0;
	}
	void print()
	{
		for (int i = 0; i < this->count; i++)
			this->slot[i]->print();
	}
	void reheapDown(int index)
	{
		int item = frequency[index];
		Elem *_item = this->slot[index];
		int i = index;
		int u = 0;
		int k;
		Elem *_k;
		bool check = false;
		while (2 * i + 1 < this->count)
		{
			u = 2 * i + 1;
			k = frequency[u];
			_k = this->slot[u];
			if (2 * i + 2 < this->count)
			{
				if (k > frequency[u + 1])
					k = frequency[++u];
				_k = this->slot[u];
			}
			if (item < k)
			{
				frequency[i] = item;
				this->slot[i] = _item;
				check = true;
				break;
			}
			else
			{
				frequency[i] = k;
				this->slot[i] = _k;
				i = u;
			}
		}
		if (!check)
		{
			frequency[i] = item;
			this->slot[i] = _item;
		}
	}
	void reheapUp(int index)
	{
		int item = frequency[index];
		Elem *_item = this->slot[index];
		int i = index;
		int k;
		Elem *_k;
		int u = (i % 2 == 0) ? (i / 2 - 1) : i / 2;
		bool check = false;
		while (u >= 0)
		{
			k = frequency[u];
			_k = this->slot[u];
			if (item >= k)
			{
				frequency[i] = item;
				this->slot[i] = _item;
				check = true;
				break;
			}
			else
			{
				frequency[i] = k;
				this->slot[i] = _k;
				i = u;
			}
			u = (i % 2 == 0) ? (i / 2 - 1) : i / 2;
		}
		if (!check)
		{
			frequency[i] = item;
			this->slot[i] = _item;
		}
	}
};

class SearchEngine
{
public:
	virtual int search(int data) = 0; // -1 if not found
	virtual void insert(Elem *data, int idx) = 0;
	virtual void deleteNode(int data) = 0;
	virtual void print(ReplacementPolicy *r) = 0;
	virtual void write(int add, Data *cont) = 0;
	virtual ~SearchEngine() {}
};

class DBHashing : public SearchEngine
{
	int (*h1)(int);
	int (*h2)(int);
	int size;
	STT_TYPES *status;
	Elem ** map;
public:
	DBHashing(int (*h1)(int), int (*h2)(int), int s)
	{
		this->h1 = h1;
		this->h2 = h2;
		this->size = s;
		this->status = new STT_TYPES[size];
		map = new Elem *[s];
		for (int i = 0; i < s; i++)
		{
			map[i] = NULL;
			this->status[i] = NIL;
		}
	}
	~DBHashing()
	{
		delete status;
	}
	void insert(Elem *data, int i)
	{
		i = 0;
		while (i < this->size)
		{
			int idx = (h1(data->addr) + i * h2(data->addr)) % this->size;
			if (this->status[idx] != UN_EMPTY)
			{
				this->map[idx] = data;
				this->status[idx] = UN_EMPTY;
				return;
			}
			else
				i++;
		}
	}
	void deleteNode(int data)
	{
		int i = 0;
		while (i < this->size)
		{
			int idx = (h1(data) + i * h2(data)) % this->size;
			if (this->map[idx]->addr == data)
			{
				this->status[idx] = DELETED;
				return;
			}
			else if (this->status[idx] != NIL)
				return;
			else
				i++;
		}
	}
	void write(int addr, Data *cont)
	{
		for (int i = 0; i < this->size; i++)
		{
			if (this->status[i] == UN_EMPTY)
				if (map[i]->addr == addr)
				{
					map[i]->data = cont;
					map[i]->sync = false;
					break;
				}
		}
	}
	void print(ReplacementPolicy *q)
	{
		cout << "Prime memory:" << endl;
		for (int i = 0; i < this->size; i++)
		{
			if (this->status[i] == UN_EMPTY)
				map[i]->print();
		}
	}
	int search(int data) { return 0; }
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
	void insert(Elem *data, int i)
	{
		this->root = insertNode(this->root, data);
	}
	void deleteNode(int data)
	{
		root = deleteNodeAVL(root, data);
	}
	void print(ReplacementPolicy *q)
	{
		cout << "Print AVL in inorder:" << endl;
		inOrder(this->root);
		cout << "Print AVL in preorder:" << endl;
		preOrder(this->root);
	}
	void write(int addr, Data *cont)
	{
		Node *cur = this->root;
		while (cur != NULL)
		{
			if (cur->data->addr == addr)
			{
				cur->data->data = cont;
				cur->data->sync = false;
				break;
			}
			else if (cur->data->addr > addr)
				cur = cur->left;

			else
				cur = cur->right;
		}
	}
	int search(int data)
	{
		return 0;
	}
	void inOrder(Node *node)
	{
		if (node == NULL)
			return;
		inOrder(node->left);
		node->data->print();
		inOrder(node->right);
	}
	void preOrder(Node *node)
	{
		if (node == NULL)
			return;
		node->data->print();
		preOrder(node->left);
		preOrder(node->right);
	}
};

#endif