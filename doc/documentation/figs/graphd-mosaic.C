main()
{

    mpad &f = mpad::mknew(1,3);
    f.cd(1,1);
    set::xtics(5);
    set::ytics(2);
    set::xrange(-10,10);
    set::yrange(-10,10);
    plot(_1,_2,2*_1*_2).ds(cboxes(-200,200))
	.nsamples1(20).nsamples2(20).p1range(-10,10).p2range(-10,10).legend("");
    f.cd(1,2);
    set::xrange(-10,10);
    set::yrange(-10,10);
    plot(_1,_2,_1*_1*sin(2*_2)).ds(mosaic_polar(-200,200))
	.nsamples1(20).nsamples2(20).p1range(0,30).p2range(0,2*3.1415);

    f.cd(1,3);
    set::xrange(-10,10);
    set::yrange(-10,10);
    double p1 = 6.90776; //std::log(1000.0);
    double p2 = -6.90776; //std::log(0.001);
    plot(_1,_2,2*_2*_2).ds(mosaic(_2*exp(_1),_2*exp(-_1))(-200,200))
	.nsamples1(20).nsamples2(20).p1range(-0.5*p1,-0.5*p2)
	.p2range(-10,10);
    mplot(_1,_2,-2*_2*_2).ds(mosaic(_2*exp(_1),-_2*exp(-_1))(-200,200).legend(false));
//	.nsamples1(20).nsamples2(20);
//	.p1range(-0.5*log(1000.0),-0.5*log(0.001));
//	.p2range(-10.0,10.0);


/*
    set::xtics(5);
    set::ytics(2);
    plot(_1,_2,_1*_1*(cos(_2)*cos(_2)-sin(_2)*sin(_2))).ds(mosaic_polar())
	.nsamples1(8).nsamples2(30).p1range(0,10).p2range(0,2*3.1415);
*/
    blopeps::print("graphd-mosaic.beps");
}
