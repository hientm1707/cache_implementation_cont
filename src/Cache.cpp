#include "main.h"

Cache::Cache(SearchEngine *sEngine, ReplacementPolicy *rPolicy)
{
    this->s_engine = sEngine;
    this->rp = rPolicy;
}
Cache::~Cache()
{
    delete rp;
    delete s_engine;
}
void Cache::printRP()
{
    rp->print();
}
void Cache::printSE()
{
    s_engine->print();
}

Data *Cache::read(int addr)
{
    return rp->read(addr);
}
Elem *Cache::put(int addr, Data *cont)
{

    Elem *retValue = rp->put(addr, cont);
    if (retValue)
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
        rp->visit(newElem);
    }
    else
    {
        if (retValue)
            s_engine->deleteNode(retValue->addr);
        s_engine->insert(newElem, 0);
    }
    return retValue;
}
