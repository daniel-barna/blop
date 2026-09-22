#ifndef __BLOP_FACTORY_H__
#define __BLOP_FACTORY_H__

#include <memory>
#include "smartptr.h"
#include <iostream>


//            return std::make_shared<T>(std::forward<Args>(args)...);  

// These utilities set up factory functions for a class
// 1. your class 'A' must derive from factory_base<A>, or from a class B
//    which itself already derives from factory_base<B> (possibly via
//    several levels)
// 2. Insert the code 'FACTORY(A);' in your class declaration
// 
// It will have the following effects:
// 1. 'operator new' for the class will be made private, so you can
//    no more call 'new A' because it bypasses the shared_ptr mechanism,
//    and could lead to memory leakage
// 2. It creates a template function A::create(...) with arbitrary args,
//    which in turn calls the constructor (via shared_ptr) with the
//    same args.
// 3. It creates the type alias 'A::ptr', which is identical to
//    smartptr<T> (see smartptr.h)

template <typename T>
class factory_base : public std::enable_shared_from_this<T>
{
public:
    bool is_shared() const { return !std::enable_shared_from_this<T>::weak_from_this().expired(); }
};

#define FACTORY(T) \
    private:                                                                         \
        static void* operator new(std::size_t size) { return ::operator new(size); } \
    public:                                                                          \
        typedef blop::smartptr<T> ptr;                                               \
        typedef blop::smartptr<const T> cptr;                                        \
        template <typename... Args>                                                  \
        static ptr create(Args&&... args)                                            \
        {                                                                            \
            T *p = new T(std::forward<Args>(args)...);                               \
            return std::shared_ptr<T>(p);                                            \
        }                                                                            \
        smartptr<T> operator&()                                                      \
        {                                                                            \
            if(!is_shared())  return this;                                           \
            return std::dynamic_pointer_cast<T>(shared_from_this());                 \
        }                                                                            \
        smartptr<const T> operator&() const                                          \
        {                                                                            \
            if(!is_shared())  return this;                                           \
            return std::dynamic_pointer_cast<const T>(shared_from_this());           \
        }


#endif
