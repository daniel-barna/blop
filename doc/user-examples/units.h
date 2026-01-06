
namespace unit
{
    // -----------  mass units -----------------
    static const  double kilogram = 1;
    static const  double kg = kilogram;
    static const  double g  = kg * 1e-3;
    static const  double gramm = g;
    static const  double amu = 1.66053886e-27*kg;

    // ----------  time units ----------------
    static const  double second = 1;
    static const  double s   = second;
    static const  double ms  = s * 1e-3;
    static const  double mus = s * 1e-6;
    static const  double ns  = s * 1e-9;
    static const  double ps = s * 1e-12;
    static const  double minute = 60*second;
    static const  double hour = 60*minute;

    // ---------  frequency units ----------------
    static const double Hertz = 1.0/second;
    static const double hertz = Hertz;
    static const double Hz  = Hertz;
    static const double kHz = Hz * 1e3;
    static const double MHz = Hz * 1e6;
    static const double GHz = Hz * 1e9;

    // ---------- distance units ----------------
    static const  double meter = 1;
    static const  double m   = meter;
    static const  double cm  = m * 1e-2;
    static const  double mm  = m * 1e-3;
    static const  double mum = m * 1e-6;
    static const  double micron = mum;
    static const  double nm  = m * 1e-9;
    static const  double pm  = m * 1e-12;
    static const  double angstrom = 1e-10 * meter;
    static const  double km = 1000*meter;
    static const  double mile = 1609.344*meter;
    static const  double inch = 25.4*mm;
    static const  double foot = 0.3048*meter;

    static const  double cm3 = cm*cm*cm;
    static const  double cm2 = cm*cm;
    static const  double mm3 = mm*mm*mm;
    static const  double mm2 = mm*mm;

    // ---------- energy units ------------------
    static const  double Joule   = kg * m*m / (second*second);
    static const  double joule   = Joule;
    static const  double J       = Joule;               // SI unit
    static const  double mJ      = J * 1e-3;
    static const  double kJ      = J * 1e3;
    static const  double MJ      = J * 1e6;
    static const  double cal     = 4.1868 * Joule;
    static const  double kcal    = cal * 1000;

    // ---------- power units -------------------
    static const  double Watt    = Joule/second;
    static const  double watt    = Watt;
    static const  double W       = Watt;
    static const  double kW      = W * 1e3;
    static const  double MW      = W * 1e6;
    static const  double GW      = W * 1e9;
    static const  double mW      = W * 1e-3;
    
    // ----------  charge unit -------------------
    static const  double Coulomb = 1;
    static const  double coulomb = Coulomb;
    static const  double C = Coulomb;     // Coulomb

    // ----------  current units -------------------
    static const double Ampere = Coulomb/second;
    static const double ampere = Ampere;
    static const double A      = Ampere;
    static const double mA     = 1e-3 * A;
    static const double muA    = 1e-6 * A;
    static const double nA     = 1e-9 * A;

    // ---------- force units ----------------------
    static const double Newton  = kilogram * meter / (second*second);
    static const double newton  = Newton;
    static const double N       = Newton;
    
    
    // --------  temperature units ---------------
    static const double Kelvin = 1;
    static const double kelvin = 1;
    static const double K      = Kelvin;
    static const double mK     = K * 1e-3;
    static const double Fahrenheit = 5./9. * Kelvin;

    // --------  amount of substance -------------
    static const double mole   = 1;
    static const double mol    = mole;
    
    // --------  luminous intensity  -------------
    static const double candela = 1;
    static const double cd      = candela;
    
    // ------------ Potential --------------------
    static const  double Volt = Joule/Coulomb;
    static const  double volt = Volt;
    static const  double V  = Volt;
    static const  double mV = 1e-3 * V;
    static const  double kV = 1e3 * V;
    static const  double MV = 1e6 * V;
    
    // ---------  Magnetic field -----------------
    static const  double T     = 1;  // Tesla
    static const  double Tesla = T;  // move verbose name
    static const  double Gauss = 1e-4 * Tesla;
    
    static const  double Debye = 3.33564e-30 * Coulomb*meter; 
    
    // -------------- resistance units --------------
    static const double Ohm   = Volt/Ampere;
    static const double ohm   = Volt/Ampere;
    static const double mOhm  = 1e-3 * Ohm;
    static const double muOhm = 1e-6 * Ohm;
    static const double nOhm  = 1e-9 * Ohm;
    static const double nanoOhm = nOhm;
    static const double kOhm  = 1e3  * Ohm;
    static const double MOhm  = 1e6  * Ohm;
    static const double Siemens = 1/Ohm;
    static const double siemens = Siemens;
    static const double S       = Siemens;
    static const double mS      = 1e-3 * Siemens;
    static const double muS     = 1e-6 * Siemens;

    // --------  Angle units -----------------------
    static const  double radian = 1;
    static const  double rad    = 1;
    static const  double degree = 3.14159265358979323846/180.0;
    static const  double deg    = degree;
    
    // --------  capacitance units ------------------
    static const double Farad = s*s*s*s * A*A / kg / (m*m);
    static const double farad = Farad;
    static const double F       = Farad;
    static const double mF      = 1e-3 * F;
    static const double mFarad  = 1e-3 * F;
    static const double muF     = 1e-6 * F;
    static const double muFarad = 1e-6 * F;
    static const double nF      = 1e-9 * F;
    static const double nFarad  = 1e-9 * F;
    static const double pF      = 1e-12 * F;
    static const double pFarad  = 1e-12 * F;

    // --------  inductance units -------------------
    static const double Henry  = kg * m*m / (s*s) / (A*A);
    static const double henry  = Henry;
    static const double H      = Henry;
    static const double mH     = 1e-3 * H;
    static const double mHenry = 1e-3 * H;
    static const double muH    = 1e-6 * H;
    static const double muHenry= 1e-6 * H;
    static const double microHenry= 1e-6 * H;
    static const double microH    = 1e-6 * H;
    static const double nH     = 1e-9 * H;
    static const double nHenry = 1e-9 * H;
    static const double nanoHenry = 1e-9 * H;
    static const double nanohenry = 1e-9 * H;
    static const double pH     = 1e-12* H;
    static const double pHenry = 1e-12* H;

    // ---------- pressure units -------------------
    static const double Pascal = Newton / (meter*meter);
    static const double pascal = Pascal;
    static const double Pa     = Pascal;
    static const double kPa    = 1e3 * Pa;
    static const double MPa    = 1e6 * Pa;
    static const double GPa    = 1e9 * Pa;
    static const double mPa    = 1e-3 * Pa;
    static const double muPa   = 1e-6 * Pa;
    static const double Torr   = 133.3223999*Pa;
    static const double mTorr  = 1e-3 * Torr;
    static const double bar    = 1e5 * Pa;  // = 750.062 Torr = 0.9869 atm
    static const double mbar   = 1e-3 * bar;
    static const double atm    = 760 * Torr;

}

namespace cons
{
     static const  double pi   = 3.14159265358979323846; 
     static const  double c    = 299792458  * unit::m/unit::s;       // speed of light     [m/s] (CODATA, 2006)
     static const  double h    = 6.6260693e-34 * unit::J*unit::s;    // Planck's constant  [Js] (CODATA, 2006)
     static const  double hbar = h/2/pi;                             // Planck's constant  [Js]
     static const  double kB   = 1.3806505e-23 * unit::J/unit::K;    // Boltzmann constant [J/K] (CODATA, 2006)
     static const  double e    = 1.60217653e-19 * unit::C;           // electron charge  [C] (CODATA, 2006)
     static const  double epsilon0 = 8.854187817e-12;                // permittivity of free space [F/m] (CODATA 2006)
     static const  double mu0      = 12.566370614e-7;                // permeability of free spa [N/A^2] (CODATA 2006)
     static const  double Z0       = ::sqrt(mu0/epsilon0);           // impedance of free space
     static const  double bohr_radius = 0.5291772108e-10;            // [m] (CODATA, 2006)

     static const  double m_proton   = 1.67262171e-27*unit::kg;      // mass of proton (CODATA, 2006)
     static const  double m_electron = 9.1093826e-31*unit::kg;       // mass of electron (CODATA, 2006)
     static const  double m_deuteron = 3.34358335e-27 * unit::kg;    // mass of deuteron (CODATA, 2006)
     static const  double m_helion   = 5.00641214e-27*unit::kg;      // mass of 3He nucleus (CODATA, 2006)

     static const  double Rydberg = m_electron*pow(e,4)/(pow(4*pi*epsilon0,2)*pow(hbar,3)*4*pi*c);

    // Avogadro's constant
     static const  double Avogadro = 6.02214199e23;

    // fine structure constant
     static const  double alpha = 7.297352568e-3; // e*e/2/epsilon0/h/c;      (CODATA value, 2006)

}

namespace unit
{
   // ------------- atomic units --------------------------

     static const  double mass_atomic     = cons::m_electron;

     static const  double charge_atomic   = cons::e;

     static const  double length_atomic   = cons::bohr_radius;

    // atomic unit of velocity: velocity in first bohr orbit
     static const  double velocity_atomic = cons::e*cons::e/(4*cons::pi*cons::epsilon0*cons::hbar);

    // atomic unit of time: period of the first bohr orbit divided by 2pi
     static const  double time_atomic     = cons::bohr_radius / velocity_atomic;
	                                       
     static const  double frequency_atomic = 1.0/time_atomic;

    // atomic unit of energy, twice the ionization otential of atomic hydrogen, 27.2113845 eV
     static const  double energy_atomic   = cons::e*cons::e/(4*cons::pi*cons::epsilon0*cons::bohr_radius);
     static const  double hartree         = energy_atomic;

     static const  double eV      = cons::e * unit::V;   //1.602176462e-19 * J;
     static const  double keV     = eV * 1e3;
     static const  double MeV     = eV * 1e6;
     static const  double GeV     = eV * 1e9;
     static const  double TeV     = eV * 1e12;

    // Rydberg = ionization potential of hydrogen
     static const  double Ry              = 0.5 * hartree;   // 13.60569172 * eV;
     static const  double Rydberg         = Ry;
    
    // electric dipole
     static const  double dipole_atomic   = charge_atomic * length_atomic;  // 2.54175*Debye
}


