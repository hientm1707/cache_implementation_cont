#ifndef CACHE_H
#define CACHE_H

#include "main.h"

//AVL Assignment 2
class Node{
	public: 
		Elem *key;
		Node *l;
		Node *r;
		int h;
		Node(){key=NULL; l=NULL; r=NULL; h=0;}
};

int max(int a,int b){
	return (a<b)?b:a;
}

int height(Node *x){
	if(x==NULL) return 0;
	return x->h;
}

Node *newNode(Elem *key){
	Node *node= new Node();
	node->key = key;
	node->l = NULL;
	node->r = NULL;
	node->h = 1;
	return node;
}

Node *rotateRight(Node *y)
{
    Node *x = y->l;
    Node *z = x->r;
    x->r = y;
    y->l = z;
    y->h = max(height(y->l),height(y->r)) + 1;
    x->h = max(height(x->l),height(x->r)) + 1;
    return x;
}

Node *rotateLeft(Node *x)
{
    Node *y = x->r;
    Node *z= y->l;
    y->l = x;
    x->r = z;
    x->h = max(height(x->l),height(x->r)) + 1;
    y->h = max(height(y->l),height(y->r)) + 1;
    return y;
}
int getBalance(Node *x)
{
    if (x == NULL)
        return 0;
    return height(x->l) - height(x->r);
}

Node* insertNode(Node* node, Elem *key)
{
    
    if (node == NULL)
        return(newNode(key));
 
    if (key->addr < node->key->addr)
        node->l = insertNode(node->l, key);
    else if (key->addr > node->key->addr)
        node->r = insertNode(node->r, key);
    else  
		return node;
 
    node->h = 1 + max(height(node->l),height(node->r));
    int balance = getBalance(node);
 
    // Left Left Case
    if (balance > 1 && key->addr < node->l->key->addr)
        return rotateRight(node);
 
    // Right Right Case
    if (balance < -1 && key->addr > node->r->key->addr)
        return rotateLeft(node);
 
    // Left Right Case
    if (balance > 1 && key->addr > node->l->key->addr)
    {
        node->l = rotateLeft(node->l);
        return rotateRight(node);
    }
 
    // Right Left Case
    if (balance < -1 && key->addr < node->r->key->addr)
    {
        node->r = rotateRight(node->r);
        return rotateLeft(node);
    }
 
    return node;
}

Node * minValueNode(Node* node) 
{ 
    Node* run = node; 
    while (run->l != NULL) 
        run = run->l; 
    return run; 
} 
Node* deleteNodeAVL(Node* root, int key) 
{ 
    if (root == NULL) return root; 

    if ( key < root->key->addr ) 
        root->l = deleteNodeAVL(root->l, key); 
    else if( key > root->key->addr ) 
        root->r = deleteNodeAVL(root->r, key); 
    else
    { 
        if( (root->l == NULL) ||
            (root->r == NULL) ) 
        { 
            Node *temp = root->l ? root->l : root->r; 
         
            if (temp == NULL) 
            { 
                temp = root; 
                root = NULL; 
            } 
            else  *root = *temp; 
            free(temp); 
        } 
        else
        { 
            Node* temp = minValueNode(root->r); 
            root->key = temp->key; 
            root->r = deleteNodeAVL(root->r, temp->key->addr); 
        } 
    } 
  
    if (root == NULL) 
    return root; 
    root->h = 1 + max(height(root->l), height(root->r)); 
    int balance = getBalance(root); 
  
    // Left Left Case 
    if (balance > 1 && getBalance(root->l) >= 0) 
        return rotateRight(root); 
  
    // Left Right Case 
    if (balance > 1 && getBalance(root->l) < 0) 
    { 
        root->l = rotateLeft(root->l); 
        return rotateRight(root); 
    } 
  
    // Right Right Case 
    if (balance < -1 && getBalance(root->r) <= 0) 
        return rotateLeft(root); 
  
    // Right Left Case 
    if (balance < -1 && getBalance(root->r) > 0) 
    { 
        root->r = rotateRight(root->r); 
        return rotateLeft(root); 
    }
    return root; 
} 


//Ass3 Begin
class ReplacementPolicy {
	protected:
	int count;
	Elem** arr;
	
	public:
	virtual Elem *insert(Elem* e,int idx){return NULL;};//insert e into arr[idx] if idx != -1 else into the position by replacement policy
	virtual void access(int idx) = 0;//idx is index in the cache of the accessed element 
	virtual void access(Elem *e) = 0;
	virtual int remove() = 0; 
	virtual void print() = 0;
	virtual ~ReplacementPolicy(){}
	bool isFull() {return count == MAXSIZE;}
	bool isEmpty() {return count == 0;}
	Elem* getValue(int idx){
		return arr[idx];
	}
	void replace(int idx,Elem* e) {
		access(idx);
		arr[idx] = e;
	}

	Data *read(int addr){
		for(int i=0; i<this->count; i++){
			if(this->arr[i]->addr == addr){
				Data *ret = this->arr[i]->data;
				Elem *e = new Elem(addr,NULL,true);
				this->access(e);
				return ret;
			}
		}
		return NULL;
	}

	Elem *put(int addr, Data *cont){
		Elem *e = new Elem(addr,cont,true);
		e = insert(e,0);
		if(e!=NULL) 
			e->sync = true;
		return e;
	}

	Elem *write(int addr, Data *cont, bool &overwrite){
		bool found = false;
		for(int i=0; i<this->count; i++){
			if(this->arr[i]->addr == addr){
				this->arr[i]->data = cont;
				this->arr[i]->sync = false;
				overwrite = true;
				found = true;
				break;
			}
		}
		Elem *ret = NULL; 
		if(!found){
			Elem *pNew = new Elem(addr,cont,false);
			pNew = insert(pNew,0);
			if(pNew!=NULL) pNew->sync = true;
			ret = pNew;
		}
		return ret;
	}
};

class SearchEngine {
	public:
	virtual int search(int key) = 0; // -1 if not found
	virtual void insert(Elem *key,int idx) = 0;
	virtual	void deleteNode(int key) = 0;
	virtual void print(ReplacementPolicy* r) = 0;
	virtual void write(int add, Data *cont) = 0;
	virtual ~SearchEngine(){}
};


class FIFO : public ReplacementPolicy {
	//Use Queue
	public:
	int front, rear;
	int max;
	FIFO(){count = 0; arr = new Elem*[MAXSIZE]; this->max = MAXSIZE; this->front = 0; this->rear = this->max - 1;}
	~FIFO(){}
	Elem *insert(Elem *e,int idx){
		Elem *ret;
		if(isFull()){
			ret = this->arr[this->front];
			this->arr[this->front] = e;
			this->front = (this->front + 1) % this->max;
		}
		else{
			this->rear = (this->rear + 1) % this->max;
			this->arr[this->rear] = e;
			this->count++;
			ret = NULL;
		}
		return ret;
	}
	
	void access(int idx) {
	
	}
	void access(Elem *e){

	}
	int remove(){return 0;}
	void print(){
		int run = this->front;
		int i = 0;
		while(i < this->count){
			this->arr[run]->print();
			i++;
			run = (run < this->count - 1) ? run + 1 : 0;
		}
	}
};

class MRU : public ReplacementPolicy {
	public:
	int activity;
	int *idx;
	MRU(){
		count = 0; 
		arr = new Elem*[MAXSIZE]; 
		activity = 0;
		idx = new int[MAXSIZE];
		for(int i=0; i < MAXSIZE; i++) idx[i] = 0;
	}
	~MRU(){
		delete idx;
	}
	Elem *insert(Elem* e,int idx){
		Elem *ret;
		if(isFull()){
			int re = remove();
			ret = this->arr[re];
			this->replace(re,e);
		}
		else{
			this->replace(this->count,e);
			ret = NULL;
			this->count++;
		}
		return ret;
	}
	void access(int idx){
		this->idx[idx] = ++activity;
	}
	void access(Elem *e){
		for(int i=0; i<this->count; i++)
			if(this->arr[i]->addr == e->addr){
				access(i);
				break;
			}
	}
	int remove(){
		for(int i=0; i<MAXSIZE; i++)
			if(this->activity == idx[i]) return i;
		return 0;
	}
	void print(){
		if(this->count == 0) return ;
		int run = 0;
		int run_activity = this->activity;
		while(run<this->count){
			for(int i=0; i< this->count; i++){
				if(this->idx[i] == run_activity) {this->arr[i]->print(); run++;break;}
			}
			run_activity--;
		}
	}
};
class LRU: public MRU {
	public:
	int remove() override {
		int min = 0;
		for(int i=1; i<MAXSIZE; i++)
			if(this->idx[min] > this->idx[i]) min = i;
		return min;
	}
};

class LFU: public ReplacementPolicy {
	public:
	int *freq;
	int *idx;
	int activity;
	LFU(){
		count = 0; 
		arr = new Elem*[MAXSIZE]; 
		freq = new int[MAXSIZE];
		idx = new int[MAXSIZE];
		activity = 0;
		for(int i=0; i < MAXSIZE; i++) {freq[i] = 0; idx[i]=0;}
	}
	~LFU(){
		delete freq;
		delete idx;
	}
	Elem *insert(Elem* e,int idx){
		Elem *ret;
		if(isFull()){
			int re = remove();
			ret = this->arr[re];
			this->arr[re] = this->arr[this->count - 1];
			this->freq[re] = this->freq[this->count - 1];
			this->count--;
			reheapDown(re);
			this->count++;
			replace(this->count-1,e);
			this->freq[this->count-1] = 1;
			reheapUp(this->count-1);
		}
		else{
			replace(this->count,e);
			this->count++;
			ret = NULL;
			reheapUp(this->count-1);
		}
		return ret;
	}
	void access(int idx){
		this->freq[idx]++;
		this->idx[idx] = ++activity;
	}
	void access(Elem *e){
		for(int i=0; i<this->count; i++)
			if(this->arr[i]->addr == e->addr){
				access(i);
				reheapDown(i);
				break;
			}
	}
	int remove(){
		return 0;
	}
	void print(){
		for(int i=0; i<this->count; i++)
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
    while(2*i + 1 < this->count){
        u = 2*i + 1;
        k = freq[u];
		_k = this->arr[u];
        if(2*i + 2 < this->count) {
            if(k > freq[u+1])
                k = freq[++u];
				_k = this->arr[u];
        }
        if(item < k){
            freq[i] = item;
			this->arr[i] = _item;
            check = true;
            break;
        }
        else{
            freq[i] = k;
			this->arr[i] = _k;
            i = u;
        }
    }
    if(!check){
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
    int u = (i%2 == 0)? (i/2 - 1) : i/2; 
    bool check = false;
    while(u >= 0){
        k = freq[u];
		_k = this->arr[u];
        if(item >= k){
            freq[i] = item;
			this->arr[i] = _item;
            check = true;
            break;
        }
        else{
            freq[i] = k;
			this->arr[i] = _k;
            i = u;
        }
        u = (i%2 == 0)? (i/2 - 1) : i/2;
    }
    if(!check){
        freq[i] = item;
		this->arr[i] = _item;
    }
}

};
enum STATUS_TYPE {NIL, NON_EMPTY, DELETED};
class DBHashing: public SearchEngine {
	int (*h1)(int);
	int (*h2)(int);
	int size;
	STATUS_TYPE* status;
	
	Elem **hashtable;
	public:
		DBHashing(int (*h1)(int),int (*h2)(int),int s){
			this->h1 =h1;
			this->h2 =h2;
			this->size =s;
			this->status = new STATUS_TYPE[size];
			hashtable = new Elem*[s];
			for(int i=0; i<s; i++){
				hashtable[i]=NULL;
				this->status[i]=NIL;
			}
		}
		~DBHashing(){
			delete status;
		}
		void insert(Elem *key,int i){
			i=0;
			while(i<this->size){
				int idx = (h1(key->addr) + i*h2(key->addr)) % this->size;
				if(this->status[idx]!=NON_EMPTY){
					this->hashtable[idx] = key;
					this->status[idx]=NON_EMPTY;
					return ;
				}
				else i++;
			}
		}
		void deleteNode(int key){
			int i=0;
			while(i<this->size){
				int idx = (h1(key) + i*h2(key)) % this->size;
				if(this->hashtable[idx]->addr == key){
					this->status[idx] = DELETED;
					return ;
				}
				else if(this->status[idx]!=NIL){
					return ;
				}
				else i++;
			}
		}
		void write(int addr, Data *cont){
			for(int i=0; i<this->size; i++){
				if(this->status[i]==NON_EMPTY)
				if(hashtable[i]->addr == addr){
					hashtable[i]->data = cont;
					hashtable[i]->sync = false;
					break;
				}
			}
		}
		void print(ReplacementPolicy* q){
			cout << "Prime memory:" << endl;
			for(int i=0; i<this->size; i++){
				if(this->status[i]==NON_EMPTY) 
					hashtable[i]->print();
			}
		}
		int search(int key) {return 0;}
};

class AVL : public SearchEngine {
	Node *root;
	public:
		AVL() {
			root = NULL;
		}
		~AVL() {
			delete root;
		}
		void insert(Elem *key,int i){
			root = insertNode(root, key);
		}
		void deleteNode(int key){
			root = deleteNodeAVL(root,key);
		}
		void print(ReplacementPolicy* q) {
			cout << "Print AVL in inorder:\n";
			printInOrder(this->root);
			cout << "Print AVL in preorder:\n";
			printPreOrder(this->root);
		}
		void write(int addr, Data *cont){
			Node *run = this->root;
			while(run!=NULL){
				if(run->key->addr == addr){
					run->key->data = cont;
					run->key->sync = false;
					break;
				}
				else if(run->key->addr > addr){
					run = run->l;
				}
				else run = run->r;
			}
		}
		int search(int key){
			return 0;
		}
		void printInOrder(Node *x){
			if(x!=NULL){
				printInOrder(x->l);
				x->key->print();
				printInOrder(x->r);
			}
		}
		void printPreOrder(Node *x){
			if(x!=NULL){
				x->key->print();
				printPreOrder(x->l);
				printPreOrder(x->r);
			}
		}
};

#endif