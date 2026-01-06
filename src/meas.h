#ifndef __MEAS_H
#define __MEAS_H




#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "var.h"
//#include "function.h"
#include "warning.h"
#include "constants.h"


namespace blop
{


// Class to store measured values with errors. Knows the Gaussian error 
// propagation. Knows the error of enties in case of a histogram 
// (error = square root of entries, due to Poisson distribution).

class meas
{
    public:
	// Value:
	double v;
	// Error:
	double e;

    public:
	// Constructors, destructor:
	meas();
	meas(const double);
	meas(const double, const double);
	meas(const meas&);
	~meas();

	double value() const { return v; }
	double error() const { return e; }
	meas &value(double a) { v = a; return *this; }
	meas &error(double a) { e = a; return *this; }

	// Increment with an entry, as a histogram bin:
	void fill();
	// Increment with a weighted entry, as a histogram bin:
	void fill(const double weight);
	void fill(const var &weight);
	// Decrement with an entry, as a histogram bin:
	void remove();
	// Decrement with a weighted entry, as a histogram bin:
	void remove(const double weight);
	void remove(const var &weight);

	// Operations:
	const meas& operator=(const meas&);
	const meas& operator=(const double);
	const meas& operator+=(const meas&);
	const meas& operator+=(const double);
	const meas& operator-=(const meas&);
	const meas& operator-=(const double);
	const meas& operator*=(const meas&);
	const meas& operator*=(const double);
	const meas& operator/=(const meas&);
	const meas& operator/=(const double);
	const meas& operator++();
	const meas& operator--();
	bool operator==(const meas&) const;
	bool operator==(const double) const;
	bool operator!=(const meas&) const;
	bool operator!=(const double) const;
	bool operator<(const meas&) const;
	bool operator<(const double) const;
	bool operator>(const meas&) const;
	bool operator>(const double) const;
	bool operator<=(const meas&) const;
	bool operator<=(const double) const;
	bool operator>=(const meas&) const;
	bool operator>=(const double) const;

	// Apply a function to a meas (modify it)
//	meas &transform(const function &f);

	// Check for finiteness:
	bool isfinite	() const;
	bool isinf	() const;
	bool isnan	() const;
	bool isset	() const;
	bool isunset	() const;
	bool isok	() const;
	static bool isfinite	(const meas&);
	static bool isinf	(const meas&);
	static bool isnan	(const meas&);
	static bool isset	(const meas&);
	static bool isunset	(const meas&);
	static bool isok	(const meas&);

	// Explicit conversion to a var string:
	var to_var(const var &format="%g") const;

};

 meas operator+(const meas&, const meas&);
 meas operator+(const meas&, const double);
 meas operator+(const double, const meas&);
 meas operator-(const meas&, const meas&);
 meas operator-(const meas&, const double);
 meas operator-(const double, const meas&);
 meas operator-(const meas&);
 meas operator*(const meas&, const meas&);
 meas operator*(const meas&, const double);
 meas operator*(const double, const meas&);
 meas operator/(const meas&, const meas&);
 meas operator/(const meas&, const double);
 meas operator/(const double, const meas&);
// meas apply(const function &, const meas &);

// Iostream operators:
std::ostream& operator<<(std::ostream&, const meas&);
std::istream& operator>>(std::istream&, meas&);

}


#ifdef __CINT__
// Iostream operators:
std::ostream& operator<<(std::ostream&, const blop::meas&);
std::istream& operator>>(std::istream&, blop::meas&);
#endif

#endif

