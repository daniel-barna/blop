main()
{
    const double f0 = 10e6;  // 10 MHz
    const double T = 1.0/f0; // one period
    
    function f(cos(2*cons::pi*f0*_1),sin(2*cons::pi*f0*_1));
    
    const int N = 512;       // # of samples
    const double L = 10*T;   // Sampled time (10 cycles)
    const double dt = L/N;   // sampling step
    const double t0shift = 18*dt;  // Do not start sampling at t=0
    
    dgraph time, freq, time2;
    for(double t=t0shift; t<t0shift+L-0.9*dt; t+=dt)
    {
        vector<var> arg, result;
        arg.push_back(t);
        f.meval(arg,result);
        time.add(t,result[0],result[1]);
    }

    
    fft(time,freq,+1);
    fft(freq,time2,-1);
    
    set::xrange(0,2*T);
    set::yrange(-1,2);

    plot(time,_1,_2).ds(lines())
	.ac(blue).legend("Orig RE");
    mplot(time,_1,_3).ds(lines())
	.ac(red).legend("Orig IM");
    mplot(time2,_1,_2).ds(points()).ps(0.5*PS)
	.ac(blue).legend("2xFFT RE");
    mplot(time2,_1,_3).ds(points()).ps(0.5*PS)
	.ac(red).legend("2xFFT IM");
    blopeps::print("fft.beps");
}
