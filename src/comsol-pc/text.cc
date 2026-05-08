#include "text.h"
#include "exc.hh"
#include "container.h"
#include "frame.h"
#include "canvas.h"
#include "bloputils.h"

#include <cstdio>

namespace blop
{
    /*
    char text::select_verbatim_char_(const std::string &s)
    {
        const char *chars = ".,+-:;!#?$^&()=/'\"|][0123456789~";
        for(int i=0; chars[i]!='\0'; ++i)
        {
            if(s.find(chars[i]) == string::npos) return chars[i];
        }
        return '\0';
    }
    */

    string text::transform_text_(const string &s)
    {
        string result = before_;
        if(verbatim_)
        {
            result += "\\texttt{";
            for(unsigned int i=0; i<s.size(); ++i)
            {
                if(s[i] == ' ') // handle spaces specially
                {
                    if(i==0 || s[i-1] != ' ') // a sequence of spaces begins now
                    {
                        result += "\\phantom{";
                    }
                    result += 'x';
                    if(i==s.size()-1 || s[i+1] != ' ')
                    {
                        result += "}";
                    }
                }
                else result += s[i];
            }
            result += "}";
        }
        else result += s;
        result += after_;
        return result;
    }

    text &text::verbatim(bool f)
    {
        if(f == verbatim_) return *this;
        verbatim_ = f;
        recalculate_size_();
        return *this;
    }

    text &text::align(sym::position a)
    {
        if(a!=sym::left && a!=sym::right && a!=sym::center)
        {
            warning::print("Text alignment can only be left|right|center","text::align(...)");
        }
        else align_ = a;
        return *this;
    }

    text &text::draw(container *parent, const var &ttt)
    {
        if(parent == 0) err("text::draw ==> parent == 0");
        text *t = new text(ttt);
        t->autodel(true);
        parent->add(t);
        return *t;
    }

    text &text::fdraw(const var &ttt)
    {
        return text::draw(&frame::current(),ttt);
    }
    text &text::pdraw(const var &ttt)
    {
        return text::draw(&pad::current(),ttt);
    }
    text &text::cdraw(const var &ttt)
    {
        return text::draw(&canvas::current(),ttt);
    }

    text::text(const var &ttt) : verbatim_(false), color_(0,0,0), gap_(0.3*EX)
    {
        vector<string> lines;
        split(ttt,"\n",lines);
        for(unsigned int i=0; i<lines.size(); ++i) add_line(lines[i]);
    }

    void text::recalculate_size_()
    {
        fix_width(false);
        fix_height(false);

        length height = 0.0;

        vector<string> transformed_lines(lines_.size());
        for(unsigned int i=0; i<lines_.size(); ++i) transformed_lines[i] = transform_text_(lines_[i]);

        for(unsigned int i=0; i<transformed_lines.size(); ++i) height += blop::height(transformed_lines[i],0);
        if(transformed_lines.size()>1) height += gap_*(transformed_lines.size()-1);
        box::height(height);

        box::width(maxwidth(transformed_lines));

        fix_width(true);
        fix_height(true);
    }

    text &text::add_line(const var &line)
    {
        lines_.push_back(line.str());
        recalculate_size_();
        return *this;
    }

    text &text::gap(const length &l)
    {
        gap_ = l;
        recalculate_size_();
        return *this;
    }

    text &text::width(const length &)
    {
        warning::print("The width of a text can not be set","text::width(const length &)");
        return *this;
    }
    text &text::height(const length &)
    {
        warning::print("The height of a text can not be set","text::height(const length &)");
        return *this;
    }

    text &text::left(const length &l)
    {
        box::left(l);
        return *this;
    }
    text &text::right(const length &l)
    {
        box::right(l);
        return *this;
    }
    text &text::xcenter(const length &l)
    {
        box::xcenter(l);
        return *this;
    }
    text &text::bottom(const length &l)
    {
        box::bottom(l);
        return *this;
    }
    text &text::top(const length &l)
    {
        box::top(l);
        return *this;
    }
    text &text::ycenter(const length &l)
    {
        box::ycenter(l);
        return *this;
    }

    void text::print(terminal *t)
    {
        if(print_me_<2) return;
        t->set_color(color_);
        for(unsigned int i=0; i<lines_.size(); ++i)
        {
            string tt = transform_text_(lines_[i]);

            if(align_ == sym::right)
            {
                t->draw_text(terminal::coord(right().termspecific_id(),ypositions_[i].termspecific_id()),
                             tt,
                             sym::right,
                             sym::top,
                             0,
                             false);

            }
            else if(align_ == sym::center)
            {
                t->draw_text(terminal::coord(xcenter().termspecific_id(),ypositions_[i].termspecific_id()),
                             tt,
                             sym::center,
                             sym::top,
                             0,
                             false);

            }
            else
            {
                t->draw_text(terminal::coord(left().termspecific_id(),ypositions_[i].termspecific_id()),
                             tt,
                             sym::left,
                             sym::top,
                             0,
                             false);
            }
        }
    }

    void text::prepare_for_draw()
    {
        if(print_me_<1) return;
        box::prepare_for_draw();
        ypositions_.clear();
        ypositions_.resize(lines_.size());
        ypositions_[0] = box::top();
        ypositions_[0].register_me();
        for(unsigned int i=1; i<lines_.size(); ++i)
        {
            ypositions_[i] = ypositions_[i-1]-blop::height(lines_[i-1])-gap_;
            ypositions_[i].register_me();
        }
    }
    
}
