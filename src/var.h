#ifndef __BLOP_VAR_H__
#define __BLOP_VAR_H__

#include <string>
#include <iostream>
#include <vector>

//#define _BLOP_VAR_ARITHMETIC_OPERATORS_IN_NAMESPACE

namespace blop
{
    using namespace std;

    class array;

    class var
    {
    protected:
        double double_;
        string string_;
        string format_;
        static string &default_format_();

        friend var abs(var);
    public:


        // ----- Initialization from different types ------------------------------

        var();    // initialize to the empty string, and 0 numerical value
        var(const string &);
        var(double);
        var(long double);
        var(double, const var &format);
        var(int);
        var(long int);
        var(unsigned int);
        var(const var &);
        var(const char *s);
        var(char c);
        virtual ~var() {}
	    

        static var sprintf(const var &format, ...);
        // ----- FORMAT FOR NUMERICAL --> STRING CONVERSION -----------------------
        // The format provided here WILL BE used in later operations to create
        // the string representation of a variable,  if it
        // is initialized or modified as a numerical value.
        // That is, it does NOT re-format the string representation from the numerical
        // value at the time of the call, only sets the format to be used later. 
        // This  format is  used as the  format spec. in a 
        // sprintf command. 
        // The static function 'default_format' sets the default format
        // for all 'var's created after this point. 

        var &format(const var &f);
        static void default_format(const var &f);

        // Reformat the string representation from the floating-point representation according to
        // the given format
        var &reformat(const var &f, bool set_format=true);

        // ------ Format checking -------------------------------------------------

        bool is_int() const;  // returns true if non-empty, and contains only digits after an optional -
        bool is_dbl() const;


        // -----  Set the double or string representation directly ----------------
        // without affecting the other (it will cause inconsistency, use with care)

        void dbl(double d)             { double_ = d; }
        void str(const std::string &s) { string_ = s; }

        // ---- Assignment from different types -----------------------------------

        virtual const var &operator= (const string &);
        virtual const var &operator= (double);
        virtual const var &operator= (int);
        virtual const var &operator= (long int);
        virtual const var &operator= (long long);
        virtual const var &operator= (unsigned int);
        virtual const var &operator= (const var &);
        virtual const var &operator= (const char *);
        virtual const var &operator= (char c);


        // ---- ARITHMETIC OPERATIONS ---------------------------------------------

        virtual const var &operator+= (const var &);
        virtual const var &operator+= (double);
        virtual const var &operator+= (int);
        virtual const var &operator+= (const string &);
	    
        virtual const var &operator-= (const var &);
        virtual const var &operator-= (double);
        virtual const var &operator-= (int);
        virtual const var &operator-= (const string &);

        virtual const var &operator*= (const var &);
        virtual const var &operator*= (double);
        virtual const var &operator*= (int);
        virtual const var &operator*= (const string &);

        virtual const var &operator/= (const var &);
        virtual const var &operator/= (double);
        virtual const var &operator/= (int);
        virtual const var &operator/= (const string &);
	    
        virtual const var &operator++ ();
        virtual var        operator++ (int);
        virtual const var &operator-- ();
        virtual var        operator-- (int);

/*
  virtual const var &incr(const var &); // make exact incr, if
  // both are a number

  */


        // ---- APPEND AS A STRING ------------------------------------------
        // These operators append their arguments
        // as strings (to the string representation of the
        // variable)

        virtual const var &operator &= (double);
        virtual const var &operator &= (const string &);
        virtual const var &operator &= (const var &);
        virtual const var &operator &= (const char *);
        virtual const var &operator &= (int i);
        virtual const var &operator &= (char);


        // -----  CHARACTER INDEXING ----------------------------------------
        // Indexing operator: access the ith character of
        // the string representation. The indexing begins
        // at 0 !!!  This currently can not be used to
        // modify single characters.

        char operator[] (int i) const;
        char back() const;

        // return the last character and remove it from the string
        char pop_back();

        // ---- IMPLICIT CONVERSION OPERATORS -------------------------------
        // these are implicit conversion operators, so that any
        // mathetmatical or other functions (with double arguments)
        // are applicable on 'var' variables without any extra
        // effort

        inline operator string() const { return string_; }
        inline operator double() const { return double_; }
        inline operator int   () const { return integer(); }
        operator bool () const;

        // Conversion to bool, to be used in if(...), while(...), etc
        // A variable of type 'var' will be converted to false, if
        // its string representation is "" (the empty string) or "0"
        // otherwise it is converted to true


        // ---- EXPLICIT CONVERSIONS ----------------------------------------

        int         integer() const;  // unfortunately the keyword 'int' is reserved...

        // The returned reference by the non-const functions (dbl and str)
        // can also be assigned. In this case no string <--> double conversion
        // is cone, more efficient for handling numerical-only values
        double       &dbl()       { return double_; }
        double        dbl() const { return double_; }
        string       &str()       { return string_; }
        const string &str() const { return string_; }

        const char *c_str() const;

        var &round(int n=0);

        typedef string::size_type size_type;
        static const size_type npos;


        // ---- SUBSTRING REPLACEMENT ---------------------------------------

        var &replace(const var &from,const var &to);  // replace all occurences
        var &replace1(const var &from,const var &to); // replace first occurence
        var &replace(size_type pos, size_type n, const var &to); // the same as std::string's replace


        // ----- MEMBER FUNCTIONS OF std::string ----------------------------


        size_type find (const var &,size_type i=0   ) const;
        size_type rfind(const var &,size_type i=npos) const;
        size_type find_first_of(const var &, size_type i=0   ) const;
        size_type find_last_of (const var &, size_type i=npos) const;
        size_type find_first_not_of(const var &, size_type i=0   ) const;
        size_type find_last_not_of (const var &, size_type i=npos) const;

        size_type size() const;
        size_type length() const;


        // ----- SUBSTRING ACCESS -------------------------------------------

        class subvar;
        subvar operator() (string::size_type from, string::size_type to = std::string::npos);

        // The 'subvar' class is a wrapper class, which behaves exactly
        // like a 'var' when used for reading. When this is modified, it
        // updates the 'var', of which is is a substring. For example
        // var s = "temperature-1.dat";
        // s(12,13) += 3;
        // The variable 's' is now: "temperature-4.dat"

        friend class var::subvar;

        // ----- PATTERN MATCHING -------------------------------------------

        bool matches(const var &pattern);
        int matches(const var &pattern, array &the_matches);

        var &replace_re(const var &pattern, const var &replacement);

    };

    // =============================================
    // replace all occurences of 'from' to 'to' in the variable 'v',
    // and return the new string. The original variable 'v' is not
    // modified (in contrast to the member function 'replace')
    // The second version, 'replace1' replaces only the first occurence

    var replace(const var &from, const var &to, const var &in);
    inline var replace(const var &from, const string &to, const var &in)       { return replace(from,var(to),in); }
    inline var replace(const string &from, const var &to, const var &in)       { return replace(var(from),to,in); }
    inline var replace(const string &from, const string &to, const var &in)    { return replace(var(from),var(to),in); }
    inline var replace(const var &from, const var &to, const string &in)       { return replace(from, to, var(in)); }
    inline var replace(const var &from, const string &to, const string &in)    { return replace(from,var(to),var(in)); }
    inline var replace(const string &from, const var &to, const string &in)    { return replace(var(from),to,var(in)); }
    inline var replace(const string &from, const string &to, const string &in) { return replace(var(from),var(to),var(in)); }
    inline var replace(const char *from, const char *to, const char *in)        { return replace(var(from),var(to),var(in)); }
    inline var replace(const char *from, const char *to, const var &in)        { return replace(var(from),var(to),in); }
    inline var replace(const char *from, const char *to, const string &in)        { return replace(var(from),var(to),var(in)); }
    inline var replace(const char *from, const var &to, const char *in)           { return replace(var(from),to,var(in)); }
    inline var replace(const char *from, const var &to, const var &in)            { return replace(var(from),to,in); }
    inline var replace(const char *from, const var &to, const string &in)         { return replace(var(from),to,var(in)); }
    inline var replace(const var &from, const char *to, const char *in)           { return replace(from,var(to),var(in)); }
    inline var replace(const var &from, const char *to, const var &in)            { return replace(from,var(to),in); }
    inline var replace(const var &from, const char *to, const string &in)         { return replace(from,var(to),var(in)); }



    var replace1(const var &from, const var &to, const var &in);
    var replace_re(const var &pattern, const var &replacement, const var &in);

    class var::subvar : public var
    {
    private:
	subvar(var *v,string::size_type i1, string::size_type i2);
	friend class var;
	void update_var();
	
	var                     *var_;
	string::size_type   from_,to_;
    public:
	subvar(const subvar &);
	
	// assignment operators
	const var &operator= (const string &s) {var::operator=(s); update_var(); return *this;}
	const var &operator= (const char   *s) {var::operator=(s); update_var(); return *this;}
	const var &operator= (double        s) {var::operator=(s); update_var(); return *this;}
	const var &operator= (int           s) {var::operator=(s); update_var(); return *this;}
	const var &operator= (const var    &s) {var::operator=(s); update_var(); return *this;}
	const var &operator= (char          s) {var::operator=(s); update_var(); return *this;}
	
	// arithmetic operators
	virtual const var &operator+= (const var &s)    {var::operator+=(s); update_var(); return *this;}
	virtual const var &operator+= (double     s)    {var::operator+=(s); update_var(); return *this;}
	virtual const var &operator+= (const string &s) {var::operator+=(s); update_var(); return *this;}
	
	virtual const var &operator-= (const var &s)    {var::operator-=(s); update_var(); return *this;}
	virtual const var &operator-= (double     s)    {var::operator-=(s); update_var(); return *this;}
	virtual const var &operator-= (const string &s) {var::operator-=(s); update_var(); return *this;}
	
	virtual const var &operator*= (const var &s)    {var::operator*=(s); update_var(); return *this;}
	virtual const var &operator*= (double     s)    {var::operator*=(s); update_var(); return *this;}
	virtual const var &operator*= (const string &s) {var::operator*=(s); update_var(); return *this;}
	
	virtual const var &operator/= (const var &s)    {var::operator/=(s); update_var(); return *this;}
	virtual const var &operator/= (double     s)    {var::operator/=(s); update_var(); return *this;}
	virtual const var &operator/= (const string &s) {var::operator/=(s); update_var(); return *this;}
	
	virtual const var &operator++ ()      {var::operator++(); update_var(); return *this;}
	virtual var        operator++ (int)   {var::operator++(0); update_var(); return *this;}
	virtual const var &operator-- ()      {var::operator--(); update_var(); return *this;}
	virtual var        operator-- (int)   {var::operator--(0); update_var(); return *this;}
	
	// append as string:
	virtual const var &operator&= (double s)         {var::operator&=(s); update_var(); return *this;}
	virtual const var &operator&= (const string &s)  {var::operator&=(s); update_var(); return *this;}
	virtual const var &operator&= (const var &s)     {var::operator&=(s); update_var(); return *this;}
	virtual const var &operator&= (const char *s)    {var::operator&=(s); update_var(); return *this;}
	virtual const var &operator&= (int s)            {var::operator&=(s); update_var(); return *this;}
	virtual const var &operator&= (char s)           {var::operator&=(s); update_var(); return *this;}
	
    };
    
    
    var abs(var);
    
    

    // ----- comparison operators: as floating point numbers  -------------------
    bool operator< (const blop::var &v,double d);
    bool operator< (const blop::var &v,int    i);
    bool operator< (double d,const blop::var &v);
    bool operator< (int    i,const blop::var &v);
    bool operator< (const blop::var &v1,const blop::var &v2);

    bool operator<= (const blop::var &v,double d);
    bool operator<= (const blop::var &v,int i);
    bool operator<= (double d,const blop::var &v);
    bool operator<= (int    i,const blop::var &v);
    bool operator<= (const blop::var &v1,const blop::var &v2);

    bool operator> (const blop::var &v,double d);
    bool operator> (const blop::var &v,int i);
    bool operator> (double d,const blop::var &v);
    bool operator> (int    i,const blop::var &v);
    bool operator> (const blop::var &v1,const blop::var &v2);

    bool operator>= (const blop::var &v,double d);
    bool operator>= (const blop::var &v,int i);
    bool operator>= (double d,const blop::var &v);
    bool operator>= (int    i,const blop::var &v);
    bool operator>= (const blop::var &v1,const blop::var &v2);


    // -------  COMPARISON OPERATORS  -----------------------------------------
    // The method of comparisons ( == and != ) depends on the
    // type of the other object, to which a 'var' is compared.
    // Comparisons with 'var', 'string' or 'char*' are carried
    // out using the string representation of 'var', while
    // comparisons with 'double' use the floating point repre-
    // sentation of 'var'.

    // WARNING: with the current version of CINT (5.15.92)
    // these comparison operators don't work, they produce a segfault.
    // Use explicit conversions for comparing a 'var' with a string
    // or a double, for example:
    // var v = "something";
    // if(v.str() == "something") { ... }

    bool operator== (const blop::var &v1, const blop::var &v2);
    bool operator== (const blop::var &v1, const char * v2);
    bool operator== (const char *v1     , const blop::var &v2);
    bool operator== (const blop::var &v1, const std::string &v2);
    bool operator== (const std::string &v1   , const blop::var &v2);

    // the above comparison operators compare the arguments as
    // strings !

    bool operator== (const blop::var &v1, double d);
    bool operator== (const blop::var &v1, int    i);
    bool operator== (double d, const blop::var &v2);
    bool operator== (int    i, const blop::var &v2);

    // these comparison operators compare the floating point
    // representation of 'var'

    bool operator != (const blop::var &v1, const blop::var &v2);
    bool operator != (const blop::var &v1, const char * v2);
    bool operator != (const char *v1     , const blop::var &v2);
    bool operator != (const blop::var &v1, const std::string &v2);
    bool operator != (const std::string &v1   , const blop::var &v2);
    bool operator != (const blop::var &v1, double d);
    bool operator != (double d,            const blop::var &v2);
    bool operator != (const blop::var &v1, int    i);
    bool operator != (int    i,            const blop::var &v2);

    // these are simply the negated == operators

    std::ostream &operator<<(std::ostream &out,const blop::var &v);
    std::istream &operator>>(std::istream &in ,      blop::var &v);


    var operator& (const var &v1,const var &v2);
    var operator& (const var &v1,const std::string &s    );
    var operator& (const std::string &s    ,const var &v2);
    var operator& (const var &v1,const char *s      );
    var operator& (const char *s      ,const var &v2);
    var operator& (const var &v1,double d           );
    var operator& (double d           ,const var &v2);
    var operator& (const std::string &s1,const std::string &s2);
    var operator& (const std::string &s1,const char   *s2);
    var operator& (const char   *s1,const std::string &s2);
    var operator& (double, const std::string &);
    var operator& (const std::string &,double);
    var operator& (const var &v, int i);
    var operator& (int i, const var &v);
    var operator& (const var &v, char c);
    var operator& (char c, const var &v);

#ifdef _BLOP_VAR_ARITHMETIC_OPERATORS_IN_NAMESPACE
    blop::var operator+ (const blop::var &v1,const blop::var &v2) ;
    blop::var operator+ (const blop::var &v1,double d)            ;
    blop::var operator+ (double d, const blop::var &v2)           ;
    blop::var operator+ (const blop::var &v1, int d)              ;
    blop::var operator+ (int d, const blop::var &v2)              ;
    blop::var operator+ (const blop::var &v1,const std::string &s);
    blop::var operator+ (const std::string &s,const blop::var &v2);
    blop::var operator+ (const blop::var &v1,const char *s)       ;
    blop::var operator+ (const char *s,const blop::var &v2)       ;

    blop::var operator- (const blop::var &v1,const blop::var &v2) ;
    blop::var operator- (const blop::var &v1,double d)            ;
    blop::var operator- (double d, const blop::var &v2)           ;
    blop::var operator- (const blop::var &v1, int d)              ;
    blop::var operator- (int d, const blop::var &v2)              ;
    blop::var operator- (const blop::var &v1,const std::string &s);
    blop::var operator- (const std::string &s,const blop::var &v2);
    blop::var operator- (const blop::var &v1,const char *s)       ;
    blop::var operator- (const char *s,const blop::var &v2)       ;

    blop::var operator* (const blop::var &v1,const blop::var &v2) ;
    blop::var operator* (const blop::var &v1,double d)            ;
    blop::var operator* (double d, const blop::var &v2)           ;
    blop::var operator* (const blop::var &v1,int d)               ;
    blop::var operator* (int d, const blop::var &v1)              ;
    blop::var operator* (const blop::var &v1,const std::string &s);
    blop::var operator* (const std::string &s,const blop::var &v2);
    blop::var operator* (const blop::var &v1,const char *s)       ;
    blop::var operator* (const char *s,const blop::var &v2)       ;

    blop::var operator/ (const blop::var &v1,const blop::var &v2) ;
    blop::var operator/ (const blop::var &v1,double d)            ;
    blop::var operator/ (double d, const blop::var &v2)           ;
    blop::var operator/ (const blop::var &v1, int d)              ;
    blop::var operator/ (int d, const blop::var &v2)              ;
    blop::var operator/ (const blop::var &v1,const std::string &s);
    blop::var operator/ (const std::string &s,const blop::var &v2);
    blop::var operator/ (const blop::var &v1,const char *s)       ;
    blop::var operator/ (const char *s,const blop::var &v2)       ;

    blop::var operator- (const blop::var &);
    
#endif // _BLOP_VAR_ARITHMETIC_OPERATORS_IN_NAMESPACE

}

#ifdef __CINT__
std::ostream &operator<<(std::ostream &out,const blop::var &v);
std::istream &operator>>(std::istream &in ,      blop::var &v);
#endif

blop::var pow  (const blop::var &,const blop::var &);
blop::var exp  (const blop::var &);
blop::var log  (const blop::var &);
blop::var log10(const blop::var &);
blop::var sqrt (const blop::var &);
blop::var sin  (const blop::var &);
blop::var cos  (const blop::var &);
blop::var asin (const blop::var &);
blop::var acos (const blop::var &);
blop::var tan  (const blop::var &);
blop::var atan (const blop::var &);
blop::var sinh (const blop::var &);
blop::var asinh(const blop::var &);
blop::var cosh (const blop::var &);
blop::var acosh(const blop::var &);

#ifndef _BLOP_VAR_ARITHMETIC_OPERATORS_IN_NAMESPACE

blop::var operator+ (const blop::var &v1,const blop::var &v2) ;
blop::var operator+ (const blop::var &v1,double d)            ;
blop::var operator+ (double d, const blop::var &v2)           ;
blop::var operator+ (const blop::var &v1, int d)              ;
blop::var operator+ (int d, const blop::var &v2)              ;
blop::var operator+ (const blop::var &v1,const std::string &s);
blop::var operator+ (const std::string &s,const blop::var &v2);
blop::var operator+ (const blop::var &v1,const char *s)       ;
blop::var operator+ (const char *s,const blop::var &v2)       ;

blop::var operator- (const blop::var &v1,const blop::var &v2) ;
blop::var operator- (const blop::var &v1,double d)            ;
blop::var operator- (double d, const blop::var &v2)           ;
blop::var operator- (const blop::var &v1, int d)              ;
blop::var operator- (int d, const blop::var &v2)              ;
blop::var operator- (const blop::var &v1,const std::string &s);
blop::var operator- (const std::string &s,const blop::var &v2);
blop::var operator- (const blop::var &v1,const char *s)       ;
blop::var operator- (const char *s,const blop::var &v2)       ;

blop::var operator* (const blop::var &v1,const blop::var &v2) ;
blop::var operator* (const blop::var &v1,double d)            ;
blop::var operator* (double d, const blop::var &v2)           ;
blop::var operator* (const blop::var &v1,int d)               ;
blop::var operator* (int d, const blop::var &v1)              ;
blop::var operator* (const blop::var &v1,const std::string &s);
blop::var operator* (const std::string &s,const blop::var &v2);
blop::var operator* (const blop::var &v1,const char *s)       ;
blop::var operator* (const char *s,const blop::var &v2)       ;

blop::var operator/ (const blop::var &v1,const blop::var &v2) ;
blop::var operator/ (const blop::var &v1,double d)            ;
blop::var operator/ (double d, const blop::var &v2)           ;
blop::var operator/ (const blop::var &v1, int d)              ;
blop::var operator/ (int d, const blop::var &v2)              ;
blop::var operator/ (const blop::var &v1,const std::string &s);
blop::var operator/ (const std::string &s,const blop::var &v2);
blop::var operator/ (const blop::var &v1,const char *s)       ;
blop::var operator/ (const char *s,const blop::var &v2)       ;

blop::var operator- (const blop::var &v)                      ;

#else

inline blop::var operator+ (const blop::var &v1,const blop::var &v2) { return blop::operator+(v1,v2); }
inline blop::var operator+ (const blop::var &v1,double d)            { return blop::operator+(v1,d);  }
inline blop::var operator+ (double d, const blop::var &v2)           { return blop::operator+(d,v2);  }
inline blop::var operator+ (const blop::var &v1, int d)              { return blop::operator+(v1,d);  }
inline blop::var operator+ (int d, const blop::var &v2)              { return blop::operator+(d,v2);  }
inline blop::var operator+ (const blop::var &v1,const std::string &s){ return blop::operator+(v1,s);  }
inline blop::var operator+ (const std::string &s,const blop::var &v2){ return blop::operator+(s,v2);  }
inline blop::var operator+ (const blop::var &v1,const char *s)       { return blop::operator+(v1,s);  }
inline blop::var operator+ (const char *s,const blop::var &v2)       { return blop::operator+(s,v2);  }

inline blop::var operator- (const blop::var &v1,const blop::var &v2) { return blop::operator-(v1,v2); }
inline blop::var operator- (const blop::var &v1,double d)            { return blop::operator-(v1,d);  }
inline blop::var operator- (double d, const blop::var &v2)           { return blop::operator-(d,v2);  }
inline blop::var operator- (const blop::var &v1, int d)              { return blop::operator-(v1,d);  }
inline blop::var operator- (int d, const blop::var &v2)              { return blop::operator-(d,v2);  }
inline blop::var operator- (const blop::var &v1,const std::string &s){ return blop::operator-(v1,s);  }
inline blop::var operator- (const std::string &s,const blop::var &v2){ return blop::operator-(s,v2);  }
inline blop::var operator- (const blop::var &v1,const char *s)       { return blop::operator-(v1,s);  }
inline blop::var operator- (const char *s,const blop::var &v2)       { return blop::operator-(s,v2);  }

inline blop::var operator* (const blop::var &v1,const blop::var &v2) { return blop::operator*(v1,v2); }
inline blop::var operator* (const blop::var &v1,double d)            { return blop::operator*(v1,d);  }
inline blop::var operator* (double d, const blop::var &v2)           { return blop::operator*(d,v2);  }
inline blop::var operator* (const blop::var &v1,int d)               { return blop::operator*(v1,d);  }
inline blop::var operator* (int d, const blop::var &v1)              { return blop::operator*(d,v1);  }
inline blop::var operator* (const blop::var &v1,const std::string &s){ return blop::operator*(v1,s);  }
inline blop::var operator* (const std::string &s,const blop::var &v2){ return blop::operator*(s,v2);  }
inline blop::var operator* (const blop::var &v1,const char *s)       { return blop::operator*(v1,s);  }
inline blop::var operator* (const char *s,const blop::var &v2)       { return blop::operator*(s,v2);  }

inline blop::var operator/ (const blop::var &v1,const blop::var &v2) { return blop::operator/(v1,v2); }
inline blop::var operator/ (const blop::var &v1,double d)            { return blop::operator/(v1,d);  }
inline blop::var operator/ (double d, const blop::var &v2)           { return blop::operator/(d,v2);  }
inline blop::var operator/ (const blop::var &v1, int d)              { return blop::operator/(v1,d);  }
inline blop::var operator/ (int d, const blop::var &v2)              { return blop::operator/(d,v2);  }
inline blop::var operator/ (const blop::var &v1,const std::string &s){ return blop::operator/(v1,s);  }
inline blop::var operator/ (const std::string &s,const blop::var &v2){ return blop::operator/(s,v2);  }
inline blop::var operator/ (const blop::var &v1,const char *s)       { return blop::operator/(v1,s);  }
inline blop::var operator/ (const char *s,const blop::var &v2)       { return blop::operator/(s,v2);  }

inline blop::var operator- (const blop::var &v)                      { return blop::operator-(v);     }

#endif // _BLOP_VAR_ARITHMETIC_OPERATORS_IN_NAMESPACE

#endif
