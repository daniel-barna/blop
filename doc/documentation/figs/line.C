#include <cmath>
using namespace std;
int main()
{
    length x1 = CM;
    length y1 = CM;
    length x2 = 2*CM;
    length y2 = 3*CM;
    length x3 = 4*CM;
    length y3 = 3*CM;

    double arrowangle = 30;

    line &l = line::cdraw(x1,y1)(x2,y2)(x3,y3).arrow(true).linewidth(0.5*MM).arrowlength(7*MM).arrowangle(arrowangle);
    line &l2 = line::cdraw(x3,y3+2*MM)(x3,y3-3*CM).linestyle(sym::dashed).linecolor(blue);
    line &l3 = line::cdraw(x3-!l.arrowlength(), y3+2*MM)(x3-!l.arrowlength(), y3-3*CM).linestyle(sym::dashed).linecolor(blue);

    label::cdraw("arrowlength",x3-0.5*!l.arrowlength(), y3-2*CM).xalign(sym::center).textcolor(blue);

    length XX = 2*CM;
    length XXX = x3 + XX;
    length YY1 = y3 + tan(arrowangle/2*3.1415/180)*XX;
    length YY2 = y3 - tan(arrowangle/2*3.1415/180)*XX;

    line &l4 = line::cdraw(XXX,YY1)(x3,y3)(XXX,YY2).linestyle(sym::dashed).linecolor(blue);

    label::cdraw("arrowangle",XXX,y3).yalign(sym::center).textcolor(blue);

    blopeps::print("line.beps");
}
