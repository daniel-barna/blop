/*
  This script plots the field patterns of the TM_nm mode of a circular waveguide.
  The produced output is also in this directory: circular-waveguide.pdf
  Note that the H field is only plotted in the xy plane, since it has no z components
  (it is a TM mode).
 */

int main()
{
    // Robert E. Collin: Foundations for microwave engineering, page 197
    //

    // waveguide radius
    const double a = 5*unit::mm;

    // Mode indexes
    const int n = 1;
    const int m = 1;

    const double p_nm = bessel_J_zero(n,m);
    const double k_cnm = p_nm/a;

    cerr<<"Cutoff frequency for TM"<<n<<m<<" mode: "
	<<p_nm/(a*2*cons::pi*sqrt(cons::mu0*cons::epsilon0))/unit::GHz<<" GHz"<<endl;

    const double f = 100*unit::GHz;
    const double w = 2*cons::pi*f;
    const double k0 = w*sqrt(cons::mu0*cons::epsilon0);

    if(k0<=p_nm/a)
    {
	cerr<<"This frequency can not propagate!"<<endl;
	return 1;
    }

    const double beta_nm = sqrt(k0*k0 - p_nm*p_nm/(a*a));
    const double lambdaz_nm = 2*cons::pi/beta_nm;
    cerr<<"Wavelength in z: "<<lambdaz_nm/unit::mm<<" mm"<<endl;

    function x   = _1;
    function y   = _2;
    function z   = _3;
    function r   = sqrt(x*x+y*y);
    function phi = atan2(y,x);


    const double Z0 = sqrt(cons::mu0/cons::epsilon0);

    // -----------------------  TM modes  -------------------------

    double Z_enm = beta_nm/k0*Z0;

    function bessel_Jprime = bessel_J(n,_1).derivative(1);

    function Er =
	-beta_nm*p_nm/(a*k_cnm*k_cnm)*bessel_Jprime(p_nm*r/a)*cos(n*phi)*sin(beta_nm*z);
    function Ephi =
	n*beta_nm/(r*k_cnm*k_cnm)*bessel_J(n,p_nm*r/a)*sin(n*phi)*sin(beta_nm*z);
    
    function Ex = Er*cos(phi)-Ephi*sin(phi);
    function Ey = Er*sin(phi)+Ephi*cos(phi);
    function Ez = bessel_J(n,p_nm*r/a)*cos(n*phi)*cos(beta_nm*z);

    function Hr = -Ephi/Z_enm;
    function Hphi = Er/Z_enm;

    function Hx = Hr*cos(phi)-Hphi*sin(phi);
    function Hy = Hr*sin(phi)+Hphi*cos(phi);
    function Hz = 0;


    // vector draw style customized
    graphd_vectors vec = vectors.use_color(true).arrowlength(0.8*EX).arrowangle(40);

    mpad &p = mpad::mknew(2,2).gap(2*MM);
    int nsamples = 25;

    p.cd(1,2);
    set::nodistort(true);
    set::xtitle("x [mm]");
    set::ytitle("y [mm]");
    frame *f12 = &frame::current();
    plot_if(r(_1,_2)<a,
	    _1/unit::mm,
	    _2/unit::mm,
	    Ex(_1,_2,cons::pi/2/beta_nm),
	    Ey(_1,_2,cons::pi/2/beta_nm))
	.p1range(-a,a).p2range(-a,a)
	.ds(vec)
	.nsamples(nsamples)
	.legend("\\tiny $E_t$");

    p.cd(2,2);
    set::xtitle("z [mm]");
    set::ytitle("x [mm]");
    double x1 = 0;
    double x2 = lambdaz_nm/unit::mm;
    double y1 = -a/unit::mm;
    double y2 =  a/unit::mm;
    set::xrange(x1,x2);
    set::yrange(y1,y2);
    set::nodistort(true);
    set::tmargin(!f12->tmargin());

    plot(_1/unit::mm, // z
	 _2/unit::mm, // x
	 Ez(_2,0,_1),
	 Ex(_2,0,_1))
	.p1range(0,lambdaz_nm).p2range(-a,a)
	.ds(vec)
	.nsamples2(nsamples).nsamples1((int)(nsamples*(x2-x1)/(y2-y1)))
	.legend("\\tiny $E_{zx}(y=0)$");
    

    p.cd(1,1);
    set::nodistort(true);
    set::xtitle("x [mm]");
    set::ytitle("y [mm]");
    plot_if(r(_1,_2)<a,
	    _1/unit::mm,
	    _2/unit::mm,
	    Hx(_1,_2,cons::pi/2/beta_nm),
	    Hy(_1,_2,cons::pi/2/beta_nm))
	.p1range(-a,a).p2range(-a,a)
	.ds(vec).nsamples(nsamples).legend("\\tiny $H_t$");


    pdf::print("circular-waveguide.pdf","-C -w 22cm -h 20cm");

}
