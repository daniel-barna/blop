color quantize(double val, double min, double max)
{
    // quantize the value into 4 values
    double d =(val-min)/(max-min);
    if(d < 0.25) return yellow;
    if(d < 0.5 ) return green;
    if(d < 0.75) return blue;
    return red;
}

main()
{

    mpad &p = mpad::mknew(1,4).gap(5*MM);

    p.cd(1,4);
    set::x1range(-2,2);
    set::y1range(-2,2);
    plot(_1,_2,exp(-0.5*_1*_1-_2*_2)).nsamples(40).ds(cboxes()).legend("");

    p.cd(1,3);
    set::x1range(-2,2);
    set::y1range(-2,2);
    plot(_1,_2,exp(-0.5*_1*_1-_2*_2)).nsamples(40).ds(cboxes()(white,black).title("TITLE")).legend("");


    //This makes segfault... why?
    p.cd(1,2);
    set::x1range(-2,2);
    set::y1range(-2,2);
    plot(_1,_2,exp(-0.5*_1*_1-_2*_2)).nsamples(40).ds(cboxes(quantize)).legend("");


    p.cd(1,1);
    plot(_1,_2,_1*sin(_2))
	.p1range(1,99)
	.p2range(0.1,3)
	.nsamples(40).ds(cboxes().logscale(true)).legend("");
    set::x1tics(20);

    pdf::print("proba.pdf");

    blopeps::print("graphd-cbox.beps");
    
}
