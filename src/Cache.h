#ifndef CACHE_H
#define CACHE_H

#include "main.h"


class Node
{
public:
	Elem *data;
	Node *left;
	Node *right;
	int h;
	Node()
	{
		data = NULL;
		left = NULL;
		right = NULL;
		h = 0;
	}
};

int max(int a, int b)
{
	return (a < b) ? b : a;
}

int height(Node *x)
{
	if (x == NULL)
		return 0;
	return x->h;
}

Node *newNode(Elem *data)
{
	Node *node = new Node();
	node->data = data;
	node->left = NULL;
	node->right = NULL;
	node->h = 1;
	return node;
}

Node *rotateRight(Node *y)
{
	Node *x = y->left;
	Node *z = x->right;
	x->right = y;
	y->left = z;
	y->h = max(height(y->left), height(y->right)) + 1;
	x->h = max(height(x->left), height(x->right)) + 1;
	return x;
}

Node *rotateLeft(Node *x)
{
	Node *y = x->right;
	Node *z = y->left;
	y->left = x;
	x->right = z;
	x->h = max(height(x->left), height(x->right)) + 1;
	y->h = max(height(y->left), height(y->right)) + 1;
	return y;
}
int getBalance(Node *x)
{
	if (x == NULL)
		return 0;
	return height(x->left) - height(x->right);
}

Node *insertNode(Node *node, Elem *data)
{

	if (node == NULL)
		return (newNode(data));

	if (data->addr < node->data->addr)
		node->left = insertNode(node->left, data);
	else if (data->addr > node->data->addr)
		node->right = insertNode(node->right, data);
	else
		return node;

	node->h = 1 + max(height(node->left), height(node->right));
	int balance = getBalance(node);

	// Left Left Case
	if (balance > 1 && data->addr < node->left->data->addr)
		return rotateRight(node);

	// Right Right Case
	if (balance < -1 && data->addr > node->right->data->addr)
		return rotateLeft(node);

	// Left Right Case
	if (balance > 1 && data->addr > node->left->data->addr)
	{
		node->left = rotateLeft(node->left);
		return rotateRight(node);
	}

	// Right Left Case
	if (balance < -1 && data->addr < node->right->data->addr)
	{
		node->right = rotateRight(node->right);
		return rotateLeft(node);
	}

	return node;
}

Node *minValueNode(Node *node)
{
	Node *cur = node;
	while (cur->left != NULL)
		cur = cur->left;
	return cur;
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
	root->h = 1 + max(height(root->left), height(root->right));
	int balance = getBalance(root);

	// Left Left Case
	if (balance > 1 && getBalance(root->left) >= 0)
		return rotateRight(root);

	// Left Right Case
	if (balance > 1 && getBalance(root->left) < 0)
	{
		root->left = rotateLeft(root->left);
		return rotateRight(root);
	}

	// Right Right Case
	if (balance < -1 && getBalance(root->right) <= 0)
		return rotateLeft(root);

	// Right Left Case
	if (balance < -1 && getBalance(root->right) > 0)
	{
		root->right = rotateRight(root->right);
		return rotateLeft(root);
	}
	return root;
}

//Ass3 Begin
class ReplacementPolicy
{
protected:
	int count;
	Elem **arr;

public:
	virtual Elem *insert(Elem *e, int idx) { return NULL; }; //insert e into arr[idx] if idx != -1 else into the position by replacement policy
	virtual void access(int idx) = 0;						 //idx is index in the cache of the accessed element
	virtual void access(Elem *e) = 0;
	virtual int remove() = 0;
	virtual void print() = 0;
	virtual ~ReplacementPolicy() {}
	bool isFull() { return count == MAXSIZE; }
	bool isEmpty() { return count == 0; }
	Elem* getValue(int idx)
	{
		return arr[idx];
	}
	void replace(int idx, Elem *e)
	{	
		
		access(idx);
		arr[idx] = e;
	}

	Data *read(int addr)
	{
		for (int i = 0; i < this->count; i++)
		{
			if (this->arr[i]->addr == addr)
			{
				Data *ret = this->arr[i]->data;
				Elem *e = new Elem(addr, NULL, true);
				this->access(e);
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

	Elem *write(int addr, Data *cont, bool &overwrite)
	{
		bool found = false;
		for (int i = 0; i < this->count; i++)
		{
			if (this->arr[i]->addr == addr)
			{
				this->arr[i]->data = cont;
				this->arr[i]->sync = false;
				overwrite = true;
				found = true;
				break;
			}
		}
		Elem *ret = NULL;
		if (!found)
		{
			Elem *pNew = new Elem(addr, cont, false);
			pNew = insert(pNew, 0);
			if (pNew != NULL)
				pNew->sync = true;
			ret = pNew;
		}
		return ret;
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

class FIFO : public ReplacementPolicy
{
	
public:
	int front, rear;
	int max;
	FIFO()
	{
		count = 0;
		arr = new Elem *[MAXSIZE];
		this->max = MAXSIZE;
		this->front = 0;
		this->rear = this->max - 1;
	}
	~FIFO() {}
	Elem *insert(Elem *e, int idx)
	{
		Elem *ret;
		if (isFull())
		{
			ret = this->arr[this->front];
			this->arr[this->front] = e;
			this->front = (this->front + 1) % this->max;
		}
		else
		{
			this->rear = (this->rear + 1) % this->max;
			this->arr[this->rear] = e;
			this->count++;
			ret = NULL;
		}
		return ret;
	}

	void access(int idx)
	{
	}
	void access(Elem *e)
	{
	}
	int remove() { return 0; }
	void print()
	{
		int cur = this->front;
		int i = 0;
		while (i < this->count)
		{
			this->arr[cur]->print();
			i++;
			cur = (cur < this->count - 1) ? cur + 1 : 0;
		}
	}
};

class MRU : public ReplacementPolicy
{
public:
	int activity;
	int *idx;
	MRU()
	{
		count = 0;
		arr = new Elem *[MAXSIZE];
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
		if (isFull())
		{
			int re = remove();
			ret = this->arr[re];
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
	void access(int idx)
	{
		this->idx[idx] = ++activity;
	}
	void access(Elem *e)
	{
		for (int i = 0; i < this->count; i++)
			if (this->arr[i]->addr == e->addr)
			{
				access(i);
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
					this->arr[i]->print();
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
public:
	int *freq;
	int *idx;
	int activity;
	LFU()
	{
		count = 0;
		arr = new Elem *[MAXSIZE];
		freq = new int[MAXSIZE];
		idx = new int[MAXSIZE];
		activity = 0;
		for (int i = 0; i < MAXSIZE; i++)
		{
			freq[i] = 0;
			idx[i] = 0;
		}
	}
	~LFU()
	{
		delete freq;
		delete idx;
	}
	Elem *insert(Elem *e, int idx)
	{
		Elem *ret;
		if (isFull())
		{
			int re = remove();
			ret = this->arr[re];
			this->arr[re] = this->arr[this->count - 1];
			this->freq[re] = this->freq[this->count - 1];
			this->count--;
			reheapDown(re);
			this->count++;
			replace(this->count - 1, e);
			this->freq[this->count - 1] = 1;
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
	void access(int idx)
	{
		this->freq[idx]++;
		this->idx[idx] = ++activity;
	}
	void access(Elem *e)
	{
		for (int i = 0; i < this->count; i++)
			if (this->arr[i]->addr == e->addr)
			{
				access(i);
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
			this->arr[i]->print();
	}
	void reheapDown(int index)
	{
		int item = freq[index];
		Elem *_item = this->arr[index];
		int i = index;
		int u = 0;
		int k;
		Elem *_k;
		bool check = false;
		while (2 * i + 1 < this->count)
		{
			u = 2 * i + 1;
			k = freq[u];
			_k = this->arr[u];
			if (2 * i + 2 < this->count)
			{
				if (k > freq[u + 1])
					k = freq[++u];
				_k = this->arr[u];
			}
			if (item < k)
			{
				freq[i] = item;
				this->arr[i] = _item;
				check = true;
				break;
			}
			else
			{
				freq[i] = k;
				this->arr[i] = _k;
				i = u;
			}
		}
		if (!check)
		{
			freq[i] = item;
			this->arr[i] = _item;
		}
	}
	void reheapUp(int index)
	{
		int item = freq[index];
		Elem *_item = this->arr[index];
		int i = index;
		int k;
		Elem *_k;
		int u = (i % 2 == 0) ? (i / 2 - 1) : i / 2;
		bool check = false;
		while (u >= 0)
		{
			k = freq[u];
			_k = this->arr[u];
			if (item >= k)
			{
				freq[i] = item;
				this->arr[i] = _item;
				check = true;
				break;
			}
			else
			{
				freq[i] = k;
				this->arr[i] = _k;
				i = u;
			}
			u = (i % 2 == 0) ? (i / 2 - 1) : i / 2;
		}
		if (!check)
		{
			freq[i] = item;
			this->arr[i] = _item;
		}
	}
};
enum STATUS_TYPE
{
	NIL,
	NON_EMPTY,
	DELETED
};
class DBHashing : public SearchEngine
{
	int (*h1)(int);
	int (*h2)(int);
	int size;
	STATUS_TYPE *status;

	Elem **hashtable;

public:
	DBHashing(int (*h1)(int), int (*h2)(int), int s)
	{
		this->h1 = h1;
		this->h2 = h2;
		this->size = s;
		this->status = new STATUS_TYPE[size];
		hashtable = new Elem *[s];
		for (int i = 0; i < s; i++)
		{
			hashtable[i] = NULL;
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
			if (this->status[idx] != NON_EMPTY)
			{
				this->hashtable[idx] = data;
				this->status[idx] = NON_EMPTY;
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
			if (this->hashtable[idx]->addr == data)
			{
				this->status[idx] = DELETED;
				return;
			}
			else if (this->status[idx] != NIL)
			{
				return;
			}
			else
				i++;
		}
	}
	void write(int addr, Data *cont)
	{
		for (int i = 0; i < this->size; i++)
		{
			if (this->status[i] == NON_EMPTY)
				if (hashtable[i]->addr == addr)
				{
					hashtable[i]->data = cont;
					hashtable[i]->sync = false;
					break;
				}
		}
	}
	void print(ReplacementPolicy *q)
	{
		cout << "Prime memory:" << endl;
		for (int i = 0; i < this->size; i++)
		{
			if (this->status[i] == NON_EMPTY)
				hashtable[i]->print();
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
		root = NULL;
	}
	~AVL()
	{
		delete root;
	}
	void insert(Elem *data, int i)
	{
		root = insertNode(root, data);
	}
	void deleteNode(int data)
	{
		root = deleteNodeAVL(root, data);
	}
	void print(ReplacementPolicy *q)
	{
		cout << "Print AVL in inorder:\n";
		inOrder(this->root);
		cout << "Print AVL in preorder:\n";
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
			{
				cur = cur->left;
			}
			else
				cur = cur->right;
		}
	}
	int search(int data)
	{
		return 0;
	}
	void inOrder(Node *x)
	{
		if (x != NULL)
		{
			inOrder(x->left);
			x->data->print();
			inOrder(x->right);
		}
	}
	void preOrder(Node *x)
	{
		if (x != NULL)
		{
			x->data->print();
			preOrder(x->left);
			preOrder(x->right);
		}
	}
};

#endif