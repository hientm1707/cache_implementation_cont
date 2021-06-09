#include "main.h"

Cache::Cache(SearchEngine *s, ReplacementPolicy *r)
{

    this->rp = r;
    this->s_engine = s;
}
Cache::~Cache()
{
    delete rp;
    delete s_engine;
}
Data *Cache::read(int addr)
{
    return rp->read(addr);
}
Elem *Cache::put(int addr, Data *cont)
{
    
    Elem *retValue = rp->put(addr, cont);
    if (retValue != NULL)
        this->s_engine->deleteNode(retValue->addr);
    this->s_engine->insert(new Elem(addr, cont, true), 0);
    return retValue;
}
Elem *Cache::write(int addr, Data *cont)
{
    bool duplicated = false;
    Elem *newElem = new Elem(addr, cont, false);
    Elem *retValue = rp->write(addr, cont, duplicated);
    if (duplicated)
    {
        s_engine->write(addr, cont);
        rp->proceed(newElem);
    }
    else
    {
        if (retValue != NULL)
            this->s_engine->deleteNode(retValue->addr);
        this->s_engine->insert(newElem, 0);
    }
    return retValue;
}
void Cache::printRP()
{
    rp->print();
}
void Cache::printSE()
{
    s_engine->print(this->rp);
}
