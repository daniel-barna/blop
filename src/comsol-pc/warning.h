#ifndef __BLOP_WARNING_H__
#define __BLOP_WARNING_H__

#include "var.h"
#include <iostream>
#include <vector>
#include <string>

namespace blop
{

class warning
{
 private:   
    static var prompt_;
    static var format_;
    static var fnformat_;
    static std::ostream *destination_;
    static bool delete_destination_;
    static std::vector<std::string> exclude_;
 public:

    // -------  Set the 'prompt' (first part of each warning output line) -------
    static void prompt(const var &p) { prompt_ = p; }
    static var &prompt() { return prompt_; }

    // -------  Specify format of warning messages ------------------------------
    // %p in this string will be substituted by prompt
    // %m will be substituted by the warning message
    // %F by the fnformat (below. it stands for function name format)
    // %f is replaced by the function name
    // defaults are: format = "%p %m\n%F"
    //               fnformat = "%p      [in %f]\n"
    //               prompt   = [blop]
    // therefore warning::print("Warning Message","function::name(...)");
    // will produce
    // [blop] Warning Message
    // [blop]        [in function::name(...)]

    static void format(const var &f) { format_ = f; }
    static var &format() { return format_; }

    static void fnformat(const var &f) { fnformat_ = f; }
    static var &fnformat() { return fnformat_; }


    // -------  Specify destination of warning messages (redirection) -----------
    static void   destination(ostream &dest);
    static void   destination(const var &filename);
    std::ostream &destination() { return *destination_ ; }

    static void   exclude(const var &);

    // -------  Print a warning message according to above specs ----------------
    // 
    static void print(const var &message, const var &funcname = "");
};

class debug
{
 private:   
    static var prompt_;
    static var format_;
    static var fnformat_;
    static std::ostream *destination_;
    static bool delete_destination_;
    static std::vector<std::string> exclude_;
    static bool on_;
 public:

    // ------- Switch debug on/off
    static void on(bool f) { on_ = f; }
    static bool on() { return on_; }

    // -------  Set the 'prompt' (first part of each warning output line) -------
    static void prompt(const var &p) { prompt_ = p; }
    static var &prompt() { return prompt_; }

    // -------  Specify format of warning messages ------------------------------
    // %p in this string will be substituted by prompt
    // %m will be substituted by the warning message
    // %F by the fnformat (below. it stands for function name format)
    // %f is replaced by the function name
    // defaults are: format = "%p %m\n%F"
    //               fnformat = "%p      [in %f]\n"
    //               prompt   = [blop]
    // therefore warning::print("Warning Message","function::name(...)");
    // will produce
    // [blop] Warning Message
    // [blop]        [in function::name(...)]

    static void format(const var &f) { format_ = f; }
    static var &format() { return format_; }

    static void fnformat(const var &f) { fnformat_ = f; }
    static var &fnformat() { return fnformat_; }


    // -------  Specify destination of warning messages (redirection) -----------
    static void   destination(ostream &dest);
    static void   destination(const var &filename);
    std::ostream &destination() { return *destination_ ; }

    static void   exclude(const var &);

    // -------  Print a warning message according to above specs ----------------
    // 
    static void print(const var &message, const var &funcname = "");
};



}

#endif
