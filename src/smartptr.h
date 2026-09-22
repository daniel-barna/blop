#ifndef __BLOP_SMARTPTR_H__
#define __BLOP_SMARTPTR_H__

#include <memory>
#include <vector>
#include <stdexcept>

/*

  A smart pointer, which can keep references to standard object on the stack (pure raw
  pointer) or on the heap by shared_ptr. 

 */

namespace blop
{
    template<typename T>
    class smartptr
    {
    private:
        // The raw c++ pointer. This is always set, and this is the pointer to access
        // the object
        T* raw_ptr_ = nullptr;

        // This is just a dummy, its role is to keep a shareds_ptr reference to the
        // object (if this reference was created from a shared_ptr) which keeps
        // the referenced object alive (we assume it was created by make_shared)
        std::shared_ptr<T> shared_ptr_;

        // Allow all smartptr<T> specializations to access each other's members
        template<typename>
        friend class smartptr;        

        // This constructor is used by dyncast only.
        smartptr(T *raw_p, const std::shared_ptr<T> &shared_p) : raw_ptr_(raw_p), shared_ptr_(shared_p) {}

    public:
        smartptr() : raw_ptr_(nullptr) {}

        // Initialize with a native C++ raw pointer. We assume it's a stack object.
        // No shared-pointer is created to keep the object alive. 
        smartptr(T* p) : raw_ptr_(p) {}

        // Initialize with a C++ shared_ptr, we keep a copy of this shared_ptr so that
        // we keep the object alive
        smartptr(const std::shared_ptr<T>& p) : raw_ptr_(p.get()), shared_ptr_(p) {}

        template <typename P>
        requires std::derived_from<P,T>
        smartptr(const smartptr<P> &rhs) : raw_ptr_(rhs.raw_ptr_), shared_ptr_(rhs.shared_ptr_) {}

        // Assignment from any other smartptr type. This code is only valid if P derives from T
        template <typename P>
        requires std::derived_from<P,T>
        smartptr<T> &operator=(const smartptr<P> &p)
        {
            raw_ptr_ = p.raw_ptr_;
            shared_ptr_ = p.shared_ptr_;
            return *this;
        }

        T* get() const {return raw_ptr_;}
        std::shared_ptr<T> get_shared() const { return shared_ptr_; }

        T& operator*() const {return *raw_ptr_;}

        T* operator->() const {return raw_ptr_;}

// Disable temporarily the automatic conversion operator to T* so that we are forced
// to revise the code and switch to smartptr<T> everywhere instead of T* 
//        operator T*() const { return raw_ptr_; }

        operator bool() const {return raw_ptr_ != nullptr;}

        template <typename P>
        requires std::derived_from<P,T>
        smartptr<P> dyncast()
        {
            return smartptr<P>(dynamic_cast<P*>(raw_ptr_), std::dynamic_pointer_cast<P>(shared_ptr_));
        }

        // Return whether this smart pointer keeps a shared_ptr to keep the referenced
        // object alive 
        bool is_shared() const { return (bool)shared_ptr_; }
    };

    template <typename T1, typename T2>
    requires (std::derived_from<T1,T2> || std::derived_from<T2,T1>)
    bool operator==(const smartptr<T1> &r1, const smartptr<T2> &r2) {return r1.get() == r2.get();}

    template <typename T1, typename T2>
    requires (std::derived_from<T1,T2> || std::derived_from<T2,T1>)
    bool operator==(const smartptr<T1> &r1, const T2 *r2) {return r1.get() == r2;}

    template <typename T1, typename T2>
    requires (std::derived_from<T1,T2> || std::derived_from<T2,T1>)
    bool operator==(const T1 *r1, const smartptr<T2> &r2) {return r1 == r2.get();}

    template <typename T1, typename T2>
    bool operator<(const smartptr<T1> &p1, const smartptr<T2> &p2) { return p1.get()<p2.get(); }
    template <typename T1, typename T2>
    bool operator<=(const smartptr<T1> &p1, const smartptr<T2> &p2) { return p1.get()<=p2.get(); }
    template <typename T1, typename T2>
    bool operator>(const smartptr<T1> &p1, const smartptr<T2> &p2) { return p1.get()>p2.get(); }
    template <typename T1, typename T2>
    bool operator>=(const smartptr<T1> &p1, const smartptr<T2> &p2) { return p1.get()>=p2.get(); }

}    

#endif
