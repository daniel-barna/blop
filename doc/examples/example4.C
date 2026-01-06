int main()
{
    line::default_arrowlength(MM);
    const double E1 = 0.1;
    const double E2 = 0.05;

    set::xrange(-5,5);
    set::yrange(-5,5);


    // this file caused a setfault on a 64 bit system
/*
    for(double x = -3; x < 3.1; x += 0.5)
    {
	for(double y = -3; y < 3.1; y += 0.5)
	{
	    if(fabs(x)<0.1 && fabs(y)<0.1) continue;
	    double Ex = E1*x + E2*y;
	    double Ey = -E1*y + E2*x;
	    line &l = line::fdraw(x1len(x),y1len(y));
	    l(x1len(x+Ex), y1len(y+Ey));
	    l.arrow(true);
	}
    }
*/
    blopeps::print("example4.beps");

}
