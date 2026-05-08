#ifndef __BLOP_LENGTH_H__
#define __BLOP_LENGTH_H__

#include <vector>
#include <iostream>
#include <cstdlib>
#include "var.h"
#include "terminal.h"
#include "constants.h"
#include "tic.h"

namespace blop
{

    class axis;
    class length_temporary;

    class length
	{
	    friend class length_temporary;
	public:
#ifndef __MAKECINT__
	    class initializer
		{
		private:
		    static int &count();
		public:
		    initializer();
		    ~initializer();
		};
#endif


	    class base_t
		{
		protected:
		    int termspecific_id_;

		public:
		    static int &count();

		    mutable int reference_count;

		    base_t();
		    base_t(const base_t &);
		    virtual ~base_t();

		    virtual void reset();
		    virtual bool specialized() const;
		    virtual terminal::id  termspecific_id() const;
		    virtual void termspecific_id(int);
		    virtual bool specialize(terminal *) = 0;

		    virtual base_t *clone(void *) const = 0;

		    virtual void print(ostream &) const = 0;

		    virtual void dependencies
			(std::vector<blop::length::base_t *> *) const {};

		    virtual void set_scale(length::base_t *) {}

		    virtual bool depends_on(length::base_t *) const { return false; }
		};

	    class base_unset_t : public base_t
		{
		public:
		    virtual bool specialize(terminal *) { termspecific_id(terminal::ZERO); return true; }
		    virtual base_t *clone(void *ptr) const { return new(ptr) length::base_unset_t(); }
		    virtual void print(ostream &out) const { out<<"unset"; }
		};

	    class base_axis_t : public base_t
		{
		private:
		    axis *axis_;
		    double value1_,value2_;
		public:
		    base_axis_t(axis *, double, double);
		    ~base_axis_t() {}

		    bool specialize(terminal *);
		    base_t *clone(void *) const;
		    void print (ostream &) const;
		    void dependencies (std::vector<blop::length::base_t *> *) const;
		    bool depends_on(length::base_t *) const;
		};

	    class base_ref_t : public base_t
		{
		private:
		    length::base_t *the_length_;
		public:
		    base_ref_t(const base_ref_t &);
		    base_ref_t(length::base_t *o);
		    ~base_ref_t();

		    bool specialize(terminal *);
		    bool specialized() const;
		    terminal::id termspecific_id() const;
		    base_t *clone(void *) const;
		    void print(ostream &) const;

		    virtual void dependencies
			(std::vector<blop::length::base_t *> *) const;
		    bool depends_on(length::base_t *) const;

		};

	    class base_id_t : public base_t
		{
		public:
		    base_id_t(int i);
		    base_id_t(const base_id_t &);
		    bool specialize(terminal *) {return true;}
		    base_t *clone(void *) const;
		    void reset() {}
		    void print(ostream &)const ;
		};

	    class base_max_t : public base_t
		{
		private:
		    std::vector<blop::length::base_t *> lengths_;
		public:
		    base_max_t(const std::vector<blop::length::base_t *> &);
		    base_max_t(const base_max_t &o);
		    ~base_max_t();

		    bool specialize (terminal *);
		    base_t *clone(void *) const;
		    void print(ostream &) const;
		    void dependencies(std::vector<blop::length::base_t *> *) const;
		    bool depends_on(length::base_t *) const;
		};

	    class base_min_t : public base_t
		{
		private:
		    std::vector<blop::length::base_t *> lengths_;
		public:
		    base_min_t(const std::vector<blop::length::base_t *> &);
		    base_min_t(const base_min_t &o);
		    ~base_min_t();

		    bool specialize (terminal *);
		    base_t *clone(void *) const;
		    void print(ostream &) const;
		    void dependencies(std::vector<blop::length::base_t *> *) const;
		    bool depends_on(length::base_t *) const;
		};


	    class base_textsize_t : public base_t
		{
		protected:
		    double angle_;
		    string text_;
		    int    direction_;
		public:
		    enum {horizontal,vertical};

		    base_textsize_t(const base_textsize_t &);
		    base_textsize_t(const string &s = "",
				    int dir=horizontal,
				    double ang = 0);
		    bool specialize(terminal *);

		    void text(const string &s);
		    void angle(double);
	 
		    void print(ostream &) const;
		};

	    class base_textwidth_t : public base_textsize_t
		{
		public:
		    base_textwidth_t(const string &s = "",double angle=0);
		    length::base_t *clone(void *) const;
		};

	    class base_textheight_t : public base_textsize_t
		{
		public:
		    base_textheight_t(const string &s = "",double angle=0);
		    length::base_t *clone(void *) const;
		};

	    class base_max_textsize_t : public base_t
		{
		protected:
		    std::vector<std::string> texts_;
		public:
		    base_max_textsize_t(const base_max_textsize_t &);
		    base_max_textsize_t(const std::vector<std::string> &);
		    base_max_textsize_t() {}
		    void add(const string &s);
		    void clear();
		};

	    class base_max_textwidth_t : public base_max_textsize_t
		{
		public:
		    base_max_textwidth_t();
		    base_max_textwidth_t(const std::vector<std::string> &);
		    base_max_textwidth_t(const base_max_textwidth_t &);
		    bool specialize(terminal *);
		    length::base_t *clone(void *) const;
		    void print(ostream &) const;
		};

	    class base_max_textheight_t : public base_max_textsize_t
		{
		public:
		    base_max_textheight_t();
		    base_max_textheight_t(const std::vector<std::string> &);
		    base_max_textheight_t(const base_max_textheight_t &);
		    bool specialize(terminal *);
		    length::base_t *clone(void *) const;
		    void print(ostream &) const;
		};

	    class base_max_textlheight_t : public base_max_textsize_t
		{
		public:
		    base_max_textlheight_t();
		    base_max_textlheight_t(const std::vector<std::string> &);
		    base_max_textlheight_t(const base_max_textlheight_t &);
		    bool specialize(terminal *);
		    length::base_t *clone(void *) const;
		    void print(ostream &) const;
		};

	    class base_lincombi_t : public base_t
		{
		public:
		    std::vector<double>                factors_;
		    std::vector<blop::length::base_t*> lengths_;

		public:
		    base_lincombi_t();
		    base_lincombi_t(const length::base_lincombi_t &);
		    base_lincombi_t(double factor, length::base_t *b);
		    ~base_lincombi_t();

		    bool specialize(terminal *);
		    void add     (double f,length::base_t     *l);
		    void multiply(double f);
		    length::base_t *clone(void *) const;
		    void print(ostream &) const;

		    virtual void dependencies
			(std::vector<blop::length::base_t *> *) const;
		    bool depends_on(length::base_t *) const;
		    void set_scale(length::base_t *);
		};

	    class base_scaleable_t : public base_t
		{
		public:
		    base_t *the_length_;
		    double factor_;
		public:
		    base_scaleable_t(double factor = 0);
		    base_scaleable_t(const base_scaleable_t &);
		    ~base_scaleable_t();

		    void set_scale(length::base_t *);

		    bool specialize(terminal *);
		    base_t *clone(void *) const;
		    void print(ostream &) const;
		    void dependencies(std::vector<blop::length::base_t *> *) const;
		    bool depends_on(length::base_t *) const;
		};

	protected:
	    base_t *length_base_ptr_;

	public:


	    // ---------------  Constructors, destructor -----------------------------------------------------

	    length(const length &l);          // initialize from another length

            length(int factor);
	    length(double factor = 0);        // If initialized from a double, this means a scaling factor.
                                              // The length will be 0, unless the 'set_scale' function is called on this
                                              // length later. In this case it will be 'factor' times the length
                                              // provided to 'set_scale'

	    length(const base_t &);           // copy the given base to it's own area

	    length(base_t *);                 // don't copy. Assumes, that the pointer provided points to a place,
                                              // which has been allocated with length::allocate()

	    //void init(const base_t &); 

	    explicit length(const char *); // dummy initializer, does not make anything

	    virtual ~length();

	    


	    // ----------------  Set a reference scale -------------------------------------------------------
	    // The length can be initialized from a double (which means, that - depending on
	    // the context in which this length is used - this length should be a multiple
	    // of some scale). If this was initialized from a double indeed,
	    // set the scale
	    
	    void set_scale(const length &);


	    // -------------   Assignment ----------------------------------

	    const length &operator=(const length &l);
	    const length &operator=(double);


	    // ------------- Arithmetic ------------------------------------

	    void operator+= (const length &l);
	    void operator-= (const length &l);
	    void operator*= (double f);
	    void operator/= (double f);

	    // ---------  not to be called by user -------------------------

	    int  specialize(terminal *) const;
	    bool specialized() const;
	    terminal::id  termspecific_id() const;

	    mutable bool locked_;

	public:
	    void print(ostream &) const;

	    static void         deallocate(const base_t *);
	    static void        *allocate();

	    const length &lock(bool f) const { locked_ = f; return *this; }
	    length       &lock(bool f)       { locked_ = f; return *this; }
	    bool locked() const { return locked_; }

	    length operator! () const;

	    bool initialized() const;

	    base_t *clone_base() const;
	    void register_me() const;
	    static void clear_regs();
	    static void specialize_regs(terminal *);

	    bool depends_on(const length &l) const;

	    static length_temporary max(std::vector<blop::length> &);
	    static length_temporary min(std::vector<blop::length> &);
	    static length_temporary max(const length &, const length &);
	    static length_temporary min(const length &, const length &);
	    static length_temporary max(const length &, const length &, const length &);
	    static length_temporary min(const length &, const length &, const length &);

	private:
	    static std::vector<blop::length::base_t *> registered_entries_;
	    
	    static void detect_deadlock();

	    friend class length::base_axis_t;
	    
	};

    class length_temporary : public length
	{
	public:
	    length_temporary (const length::base_t &);
	    length_temporary (length::base_t *);
	    length_temporary (const length &l);
	    base_t *clone_base() const;
	};
    
    length_temporary width(const var &s,double angle=0);
    length_temporary height(const var &s,double angle=0);
    length_temporary maxwidth (const std::vector<blop::var> &);
    length_temporary maxwidth (const std::vector<std::string> &);
    length_temporary maxwidth (const std::vector<blop::tic> &);
    length_temporary maxheight(const std::vector<blop::var> &);
    length_temporary maxlheight(const std::vector<blop::var> &);

    length_temporary operator+ (const length &,const length &);
    length_temporary operator- (const length &,const length &);
    length_temporary operator* (double        ,const length &);
    length_temporary operator* (const length &,double        );
    length_temporary operator/ (const length &,double        );
    length_temporary operator- (const length &);

    extern  length ZERO, PW, PH, PS, LW, CM, MM, PT, EM, EX;


    ostream &operator<< (ostream &out, const length &l);
    ostream &operator<< (ostream &out, const  length::base_t &l); 



// needed only because of cint, dummy functions

inline bool operator== (const blop::length &, const blop::length)
    { std::cerr<<"You should not call operator==(length,length) !!!\n"; return false; }
inline bool operator>  (const blop::length &, const blop::length)
    { std::cerr<<"You should not call operator> (length,length) !!!\n"; return false; }
inline bool operator<  (const blop::length &, const blop::length)
    { std::cerr<<"You should not call operator< (length,length) !!!\n"; return false; }


length_temporary lincombi(double f,int id);

}

#ifndef __MAKECINT__
namespace {
    blop::length::initializer __blop_length_initializer;
}
#endif

#endif
