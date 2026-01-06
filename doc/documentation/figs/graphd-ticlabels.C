main()
{
    set::xrange(0.5,3.5);
    set::yrange(0,13);
    plot("<<1 5   \"May 01\"
            2 7   \"May 02\"
            3 10  \"May 03\" ").ds(xticlabels(bars())).legend("Consumed beer"); 

    blopeps::print("ticlabels.beps");
}
