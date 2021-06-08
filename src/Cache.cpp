#include "main.h"


Cache::Cache(SearchEngine* s,ReplacementPolicy* r):rp(r),s_engine(s) {}
Cache::~Cache(){
	delete rp;
	delete s_engine;
}
Data* Cache::read(int addr) {
    return rp->read(addr);
}
Elem* Cache::put(int addr, Data* cont) {
    Elem *pNew = new Elem(addr,cont,true);
    Elem *ret = rp->put(addr,cont);
    if(ret!=NULL)
        this->s_engine->deleteNode(ret->addr);
    this->s_engine->insert(pNew,0);
    return ret;
}
Elem* Cache::write(int addr, Data* cont) {
    bool overwrite = false;
    Elem *pNew = new Elem(addr,cont,false);
    Elem *ret = rp->write(addr,cont,overwrite);
    if(overwrite) {
        s_engine->write(addr,cont);
        rp->access(pNew);
    }
    else{
        if(ret!=NULL) 
            this->s_engine->deleteNode(ret->addr);
        this->s_engine->insert(pNew,0);
    }
    return ret;
}
void Cache::printRP() {
    rp->print();
}
void Cache::printSE() {
    s_engine->print(this->rp);
}

