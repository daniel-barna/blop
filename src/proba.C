#include "smartptr.h"
#include "factory.h"
#include <iostream>
using namespace std;

class k
{
public:
    k() { cerr<<"k::k()"<<endl; }
    ~k() { cerr<<"k::~k()"<<endl; }
};

class A : public factory_base<A>
{
public:
    FACTORY(A);
    k data[3];
};


int main()
{
    auto sptr = A::create();
    cerr<<"is shared: "<<sptr.is_shared()<<endl;
    return 0;
}

































