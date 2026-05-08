#ifndef __BlOP_GARBAGE_COLLECTOR_PTR_H__
#define __BlOP_GARBAGE_COLLECTOR_PTR_H__

#include <map>

namespace blop
{
    template <typename T>
    class garbage_collector_ptr
    {
    private:
        T *ptr_;
        static std::map<T*,int> &link_count()
        {
            static std::map<T*,int> m;
            return m;
        }
        
    public:
        garbage_collector_ptr() : ptr_(0) {}
        garbage_collector_ptr(T *ptr) : ptr_(ptr)
        {
            ++link_count()[ptr_];
        }
        garbage_collector_ptr(const garbage_collector_ptr &rhs) : ptr_(rhs.ptr_)
        {
            ++link_count()[ptr_];
        }
        ~garbage_collector_ptr()
        {
            if(--link_count()[ptr_] == 0)
            {
                link_count().erase(ptr_);
                delete ptr_;
            }
        }
        const garbage_collector_ptr &operator=(const garbage_collector_ptr &rhs)
        {
            ptr_ = rhs.ptr_;
            ++link_count()[ptr_];
            return *this;
        }

        T *operator->() const
        {
            return ptr_;
        }
        T &operator*() const
        {
            return *ptr_;
        }
    };

}


#endif
