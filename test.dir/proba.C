#include <variant>
#include <iostream>
#include <vector>
using namespace std;

class A
{
public:
    virtual ~A() {}
    virtual void f() = 0;
};

class B1 : public A
{
public:
    void f() { cerr<<"B1"<<endl; }
};
class B2 : public A
{
public:
    void f() { cerr<<"B2"<<endl; }
};
class B3 : public A
{
public:
    void f() { cerr<<"B3"<<endl; }
};
class C
{
public:
    void f() { cerr<<"C"<<endl; }
};

int main()
{
    typedef std::variant<B1,B2,B3,C> V;
    vector<V> v = {B1(), B2(), B3(),C()};

    for(auto i : v) visit([](auto&& arg){arg.f();}, i); 

    return 0;
}
