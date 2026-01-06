main()
{
    set::x1range(-2,2);
    set::y1range(-2,2);
    plot(_1, _2, exp(-0.5*_1*_1-_2*_2)).ds(isolines()).legend("");
    blopeps::print("graphd-isolines.beps");
}

