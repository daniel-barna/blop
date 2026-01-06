/*
 * This is a sample user-specific  initfile  created automatically
 * It contains  many  useful  examples to  make  your life easier. 
 * You may uncomment parts of this file to use these functions, or 
 * - inspired by these examples - write  others,  which  you like 
 * even better
 */

/*

extern "C" void BLOP_USER_INIT()
{
    // this is the function which will be called at program startup
    // write here every configuration commands, for example:

    // pad::default_fillcolor(red);

    // switch on grids along the x1 and y1 axes
    // frame::default_grid(axis::x1 | axis::y1);
}

// you can also provide handy functions:

// --- this draws a horizontal line in the current frame at the given value
line &hline(double d) 
{
   return line::fdraw(0,y1len(d))(1,y1len(d));
}

// --- this draws an arrow in the frame
line &arrow(double x1,double y1,double x2,double y2)
{
   return line::fdraw(x1len(x1),y1len(y1))(x1len(x2),y1len(y2)).arrow(true);
}

// --- switch to logscale or back
void x1log(bool i)
{
    frame::current().x1axis()->logscale(i);
}
void y1log(bool i)
{
    frame::current().y1axis()->logscale(i);
}
void x2log(bool i)
{
    frame::current().x2axis()->logscale(i);
}
void y2log(bool i)
{
    frame::current().y2axis()->logscale(i);
}


*/
