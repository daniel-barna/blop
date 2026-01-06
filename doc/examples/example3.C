var func1(var a)
{
    return sin(a) + cos(a);
}

var func2(var a,var b,var c)
{
    if(a < 3) return a;
    return sin(a)*cos(b)+c;
}

int main()
{
    plot(_1,sin(_1)).p1range(0,4).legend("sin");

// ooops, the commented code makes a segfault!
//    function f1 = cfunc(func1);
//    mplot("data",_1,f1(_2)).legend("func1 is called on the 2$^\\mathrm{nd}$ column");
//    mplot("data",_1,cfunc(func2)).legend("func2 called on 1st, 2nd and 3rd columns");
    blopeps::print("example3.beps");
}
