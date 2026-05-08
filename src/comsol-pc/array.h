#ifndef __BLOP_ARRAY_H__
#define __BLOP_ARRAY_H__
#include "var.h"

namespace blop
{
    class array : public std::vector<blop::var>
    {
    private:
	unsigned int iterator_;

	static var outfs_;
	static var infs_;

	static array tmp_;
    public:
	array();
	array(const array &);
	array(unsigned int size);

#ifndef __MAKECINT__
        array(const std::initializer_list<double> &l);
        array(const std::initializer_list<int> &l);
        array(const std::initializer_list<std::string> &l);
        array(const std::initializer_list<const char *> &l);
        array(const std::initializer_list<blop::var> &l);
#endif    

	// output and input field separators (infs not yet used)
	static const var &outfs();
	static void       outfs(const var &);

	static const var &infs();
	static void       infs(const var &);

	// return the last element of the array, and remove it
	var pop_back();

	array &push_back(const array &);
	void push_back(const var &v) { vector<var>::push_back(v); }
        void push_back(double d) { vector<var>::push_back(d); }
        void push_back(int i) { vector<var>::push_back(i); }

	// assigmnemt
	const array &operator= (const array &rhs);

	// the same as push_back, but returns reference
	// to itself, so that it can be chained
	array &operator() (const var &v);

	void push_front(const var &);
	void push_front(const array &);

	// std::vector has only an erase(iterator) function
	// this is an added feature for ease of use
	array &erase(unsigned int pos);

	static array &make(const var &v);

	// Make a sequence of numbers, starting from 'from', not extending 'to'
	// (that is, 'to' is included in the sequence, but be aware of finite-precision
	// artefacts)
	static array &sequence(const var &step, const var &from, const var &to);

	// replace the string 'from' to 'to' in all of the elements
	array &replace(const var &from, const var &to);

        array &replace_re(const var &pattern, const var &replacement);

        // ----------  concatenate all elements using a separator ------

        var join(const var &separator="", unsigned int from=0, int to=-1) const;

	// ----------  iterating over a vector  --------------------

	// set innter iterator to first, last  or any other element
	void setfirst();  
	void setlast();   
	void set(int i);

	// get the current index (0-based);
	int pos() const;

	// test if iterator is within range
	operator bool() const;  

	// increment or decrement iterator
	const array &operator++();
	const array &operator--();

	// access current element
	var &operator()();
	const var &operator()() const;

	// --------------  sorting ----------------------------------

	void sort_s(); // sort as strings
	void sort_n(); // sort as numbers

    };


    // ---------------------------------------------------------
    // remove duplicate entries from the array 'a', and return
    // a new array

    array array_unique(const array &a);
}

std::ostream& operator<< (std::ostream &, const blop::array &);

#endif
