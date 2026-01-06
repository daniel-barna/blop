#include "set.h"
#include "label.h"

namespace blop
{
    tic_setter set::xtics(axis::x1);
    tic_setter set::ytics(axis::y1);
    tic_setter set::x1tics(axis::x1);
    tic_setter set::x2tics(axis::x2);
    tic_setter set::y1tics(axis::y1);
    tic_setter set::y2tics(axis::y2);

    auto aline(double d,int ax, var label, const length &x, const length &y, sym::position halign, sym::position valign, double angle)
    {
        struct _ { blop::line &the_line; blop::label &the_label; };
        if(ax == axis::x1)
        {
            if(angle==unset) angle=90*unit::deg;
            return _ {line::fdraw(x1len(d),0.0)(x1len(d),1.0),
                    label::fdraw(label,x1len(d)+x,y).align(halign,valign).angle(angle)};
        }
        if(ax == axis::x2)
        {
            if(angle==unset) angle=90*unit::deg;
            return _ {line::fdraw(x2len(d),0.0)(x2len(d),1.0),
                    label::fdraw(label,x2len(d)+x,y).align(halign,valign).angle(angle)};
        }
        if(ax == axis::y1)
        {
            if(angle==unset) angle=0;
            return _ {line::fdraw(0.0,y1len(d))(1.0,y1len(d)),
                    label::fdraw(label,x,y1len(d)+y).align(halign,valign).angle(angle)};
        }
        if(ax == axis::y2)
        {
            if(angle==unset) angle=0;
            return _ {line::fdraw(0.0,y2len(d))(1.0,y2len(d)),
                    label::fdraw(label,x,y2len(d)+y).align(halign,valign).angle(angle)};
        }
        static blop::line the_line;
        static blop::label the_label;
        return _ {the_line,the_label};
    }
    
    replot_request set::y2transform(const function &to, const function &back, bool own_tics)
    {
	frame::current().y2axis()->transform(frame::current().y1axis(), to, back, own_tics);
	return replot_request();
    }
    replot_request set::x2transform(const function &to, const function &back, bool own_tics)
    {
	frame::current().x2axis()->transform(frame::current().x1axis(), to, back, own_tics);
	return replot_request();
    }
    replot_request set::y1transform(const function &to, const function &back, bool own_tics)
    {
	frame::current().y1axis()->transform(frame::current().y2axis(), to, back, own_tics);
	return replot_request();
    }
    replot_request set::x1transform(const function &to, const function &back, bool own_tics)
    {
	frame::current().x1axis()->transform(frame::current().x2axis(), to, back, own_tics);
	return replot_request();
    }
}
