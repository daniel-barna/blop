#ifndef __BLOP_TEXT_H__
#define __BLOP_TEXT_H__

#include <vector>
#include <string>
#include "box.h"
#include "color.h"

// A multi-line text, which is typeset line-by-line

namespace blop
{
    class text : public box
    {
    private:
        bool verbatim_;
        color color_;
        std::vector<std::string>   lines_;
        std::string before_, after_;
        std::vector<blop::length>  ypositions_;
        length gap_;
        void recalculate_size_();
        sym::position align_;
        //char select_verbatim_char_(const std::string &s);
        std::string transform_text_(const std::string &s);

    public:
        text(const var &);
        text &add_line(const var &line);

        text &gap    (const length &);
        const length &gap() const { return gap_; }

        text &verbatim(bool f);
        bool  verbatim() const { return verbatim_; }

        text &align(sym::position);

        text &textcolor(const color &c) { color_ = c; return *this; }
        color textcolor() const { return color_; }

        text &before(const var &b) { before_ = b.str(); return *this; }
        std::string before() const { return before_; }
        text &after (const var &b) { after_  = b.str(); return *this; }
        std::string after()  const { return after_;  }

        text &left   (const length &);
        text &right  (const length &);
        text &xcenter(const length &);
        text &width  (const length &);

        text &bottom (const length &);
        text &top    (const length &);
        text &ycenter(const length &);
        text &height (const length &);

        const length &left() const { return box::left(); }
        const length &right() const { return box::right(); }
        const length &xcenter() const { return box::xcenter(); }
        const length &width() const { return box::width(); }
        const length &bottom() const { return box::bottom(); }
        const length &top() const { return box::top(); }
        const length &ycenter() const { return box::ycenter(); }
        const length &height() const { return box::height(); }

        static text &draw(container *parent, const var &);
        static text &fdraw(const var &);
        static text &pdraw(const var &);
        static text &cdraw(const var &);

        // needed only for CINT, dummy functions
        bool operator== (const text &) const {return false;}
        bool operator<  (const text &) const {return false;}
        bool operator>  (const text &) const {return false;}
        
        void prepare_for_draw();
        void print(terminal *);
    };
}


#endif
