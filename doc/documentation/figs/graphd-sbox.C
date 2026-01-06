main()
{
    set::xrange(-2,2);
    set::yrange(-2,2);
    plot(_1,_2,exp(-0.5*_1*_1-_2*_2)).nsamples(20).ds(sboxes()).legend("");
    blopeps::print("graphd-sbox.beps");
}
