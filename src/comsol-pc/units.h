#ifndef __BLOP_UNITS_H__
#define __BLOP_UNITS_H__

#include "var.h"
#include "function.h"
#include <cmath>
namespace blop
{

namespace cons
{
    static constexpr  double pi   = 3.14159265358979323846; 
}

namespace unit
{
    static constexpr double percent = 0.01;
    static constexpr double permill = 0.001;
    static constexpr double permil  = 0.001;
    static constexpr double ppm     = 1e-6;

    static constexpr double deci  = 1e-1;
    static constexpr double centi = 1e-2;
    static constexpr double milli = 1e-3;
    static constexpr double micro = 1e-6;
    static constexpr double nano  = 1e-9;
    static constexpr double pico  = 1e-12;
    static constexpr double femto = 1e-15;
    static constexpr double atto  = 1e-18;
    static constexpr double zepto = 1e-21;
    static constexpr double yocto = 1e-24;

    static constexpr double deca  = 10;
    static constexpr double hecto = 100;
    static constexpr double kilo  = 1e3;
    static constexpr double mega  = 1e6;
    static constexpr double giga  = 1e9;
    static constexpr double tera  = 1e12;
    static constexpr double peta  = 1e15;
    static constexpr double exa   = 1e18;
    static constexpr double zetta = 1e21;
    static constexpr double yotta = 1e24;

    // -----------  mass units -----------------
    static constexpr  double kilogram = 1;
    static constexpr  double kg = kilogram;
    static constexpr double  ton = kg * 1e3;
    static constexpr  double g  = kg * 1e-3;
    static constexpr  double gramm = g;
    static constexpr  double mg = g*1e-3;
    static constexpr  double amu = 1.66053886e-27*kg;
    static constexpr  double pound = 0.45359237*kg;
    static constexpr  double lbs = pound;

    // ----------  time units ----------------
    static constexpr  double second = 1;
    static constexpr  double s   = second;
    static constexpr  double ms  = s * 1e-3;
    static constexpr  double mus = s * 1e-6;
    static constexpr  double us  = s * 1e-6;
    static constexpr  double ns  = s * 1e-9;
    static constexpr  double ps = s * 1e-12;
    static constexpr  double fs = s * 1e-15;
    static constexpr  double minute = 60*second;
    static constexpr  double min    = minute;
    static constexpr  double hour = 60*minute;
    static constexpr  double h    = hour;
    static constexpr  double day = 24*hour;
    static constexpr  double week = 7*day;
    static constexpr  double month = 30*day;
    static constexpr  double year = 365*day;

    // ---------  frequency units ----------------
    static constexpr double Hertz = 1.0/second;
    static constexpr double hertz = Hertz;
    static constexpr double Hz  = Hertz;
    static constexpr double kHz = Hz * 1e3;
    static constexpr double MHz = Hz * 1e6;
    static constexpr double GHz = Hz * 1e9;
    static constexpr double THz = Hz * 1e12;
    static constexpr double rpm = 1.0/unit::minute;

    // ---------- distance units ----------------
    static constexpr  double meter = 1;
    static constexpr  double m   = meter;
    static constexpr  double dm  = m * 1e-1;
    static constexpr  double cm  = m * 1e-2;
    static constexpr  double mm  = m * 1e-3;
    static constexpr  double mum = m * 1e-6;
    static constexpr  double um  = m * 1e-6;
    static constexpr  double micron = mum;
    static constexpr  double nm  = nano * m;
    static constexpr  double pm  = pico * m;
    static constexpr  double fm  = femto * m;
    static constexpr  double fermi = fm;
    static constexpr  double angstrom = 1e-10 * meter;
    static constexpr  double Angstrom = 1e-10 * meter;
    static constexpr  double km = 1000*meter;
    static constexpr  double mile = 1609.344*meter;
    static constexpr  double inch = 25.4*mm;
    static constexpr  double mil  = 1e-3*inch;
    static constexpr  double foot = 0.3048*meter;

    // ------------ volume units  ---------------
    static constexpr  double m3  = m*m*m;
    static constexpr  double dm3 = dm*dm*dm;
    static constexpr  double liter = dm3;
    static constexpr  double cm3 = cm*cm*cm;
    static constexpr  double mm3 = mm*mm*mm;

    static constexpr  double m2  = m*m;
    static constexpr  double dm2 = dm*dm;
    static constexpr  double cm2 = cm*cm;
    static constexpr  double mm2 = mm*mm;
    static constexpr  double barn = 1e-28*m2;
    static constexpr  double b    = barn;
    static constexpr  double fb   = femto*barn;

    
    // ---------- energy units ------------------
    static constexpr  double Joule   = kg * m*m / (second*second);
    static constexpr  double joule   = Joule;
    static constexpr  double J       = Joule;               // SI unit
    static constexpr  double mJ      = J * 1e-3;
    static constexpr  double kJ      = J * 1e3;
    static constexpr  double MJ      = J * 1e6;
    static constexpr  double GJ      = J * 1e9;
    static constexpr  double cal     = 4.184 * Joule;
    static constexpr  double kcal    = cal * 1000;

    // ---------- power units -------------------
    static constexpr  double Watt    = Joule/second;
    static constexpr  double watt    = Watt;
    static constexpr  double W       = Watt;
    static constexpr  double kW      = kilo*Watt;
    static constexpr  double MW      = mega*Watt;
    static constexpr  double GW      = giga*Watt;
    static constexpr  double TW      = tera*Watt;
    static constexpr  double PW      = peta*Watt;
    static constexpr  double mW      = W * 1e-3;
    
    // ----------  charge unit -------------------
    static constexpr  double Coulomb = 1;
    static constexpr  double coulomb = Coulomb;
    static constexpr  double C = Coulomb;     // Coulomb
    static constexpr  double mC  = 1e-3*C;
    static constexpr  double muC = 1e-6*C;
    static constexpr  double nC  = 1e-9*C;
    static constexpr  double pC  = 1e-12*C;
    static constexpr  double fC  = 1e-15*C;

    // ----------  current units -------------------
    static constexpr double Ampere = Coulomb/second;
    static constexpr double ampere = Ampere;
    static constexpr double A      = Ampere;
    static constexpr double mA     = 1e-3 * A;
    static constexpr double muA    = 1e-6 * A;
    static constexpr double nA     = 1e-9 * A;
    static constexpr double kA     = 1e3 * A;
    static constexpr double MA     = 1e6 * A;
    

    // ---------- force units ----------------------
    static constexpr double Newton  = kilogram * meter / (second*second);
    static constexpr double newton  = Newton;
    static constexpr double N       = Newton;
    static constexpr double kN      = Newton * 1e3;
    static constexpr double lbf     = 4.44822162*Newton; // http://en.wikipedia.org/wiki/Pound-force
    
    // --------  temperature units ---------------
    static constexpr double Kelvin = 1;
    static constexpr double kelvin = 1;
    static constexpr double K      = Kelvin;
    static constexpr double mK     = K * 1e-3;
    //static constexpr double Fahrenheit = 5./9. * Kelvin;

    // --------  amount of substance -------------
    static constexpr double mole   = 1;
    static constexpr double mol    = mole;
    
    // --------  luminous intensity  -------------
    static constexpr double candela = 1;
    static constexpr double cd      = candela;
    
    // ------------ Potential --------------------
    static constexpr  double Volt      = Joule/Coulomb;
    static constexpr  double volt      = Volt;
    static constexpr  double V         = Volt;
    static constexpr  double mV        = 1e-3 * V;
    static constexpr  double milliVolt = 1e-3 * V;
    static constexpr  double muV       = 1e-6 * V;
    static constexpr  double uV        = 1e-6 * V;
    static constexpr  double microVolt = 1e-6 * V;
    static constexpr  double kV        = 1e3 * V;
    static constexpr  double MV        = 1e6 * V;
    
    // ---------  Magnetic field -----------------
    static constexpr  double T     = 1;  // Tesla
    static constexpr  double Tesla = T;  // move verbose name
    static constexpr  double mT    = 1e-3 * T;
    static constexpr  double muT    = 1e-6 * T;
    static constexpr  double nT     = 1e-9 * T;
    static constexpr  double Gauss = 1e-4 * Tesla;
    static constexpr  double G     = Gauss;
    static constexpr double mG     = milli*Gauss;
    static constexpr double mGauss = milli*Gauss;
    static constexpr  double kGauss = kilo*Gauss;
    static constexpr  double kG     = kilo*Gauss;
    static constexpr  double Oersted = 1000/(4*cons::pi)*Ampere/meter;
    static constexpr  double Oe      = Oersted;

    static constexpr double Weber = 1*T*m2;
    static constexpr double weber = Weber;
    static constexpr double Wb    = Weber;

    static constexpr  double Debye = 3.33564e-30 * Coulomb*meter; 
    
    // -------------- resistance units --------------
    static constexpr double Ohm      = Volt/Ampere;
    static constexpr double ohm      = Volt/Ampere;
    static constexpr double mOhm     = 1e-3 * Ohm;
    static constexpr double muOhm    = 1e-6 * Ohm;
    static constexpr double microOhm = 1e-6 * Ohm;
    static constexpr double uOhm     = 1e-6 * Ohm;
    static constexpr double nOhm     = 1e-9 * Ohm;
    static constexpr double nanoOhm  = nOhm;
    static constexpr double kOhm     = 1e3  * Ohm;
    static constexpr double MOhm     = 1e6  * Ohm;
    static constexpr double Siemens  = 1/Ohm;
    static constexpr double siemens  = Siemens;
    static constexpr double S        = Siemens;
    static constexpr double mS       = 1e-3 * Siemens;
    static constexpr double muS      = 1e-6 * Siemens;

    // --------  Angle units -----------------------
    static constexpr  double radian = 1;
    static constexpr  double rad    = 1;
    static constexpr  double mrad   = milli*radian;
    static constexpr  double mradian = mrad;
    static constexpr  double degree = 3.14159265358979323846/180.0;
    static constexpr  double deg    = degree;
    
    // --------  capacitance units ------------------
    static constexpr double Farad = s*s*s*s * A*A / kg / (m*m);
    static constexpr double farad = Farad;
    static constexpr double F       = Farad;
    static constexpr double mF      = 1e-3 * F;
    static constexpr double mFarad  = 1e-3 * F;
    static constexpr double muF     = 1e-6 * F;
    static constexpr double muFarad = 1e-6 * F;
    static constexpr double uF      = 1e-6 * F;
    static constexpr double nF      = 1e-9 * F;
    static constexpr double nFarad  = 1e-9 * F;
    static constexpr double pF      = 1e-12 * F;
    static constexpr double pFarad  = 1e-12 * F;

    // --------  inductance units -------------------
    static constexpr double Henry  = kg * m*m / (s*s) / (A*A);
    static constexpr double henry  = Henry;
    static constexpr double H      = Henry;
    static constexpr double mH     = 1e-3 * H;
    static constexpr double mHenry = 1e-3 * H;
    static constexpr double muH    = 1e-6 * H;
    static constexpr double muHenry= 1e-6 * H;
    static constexpr double microHenry= 1e-6 * H;
    static constexpr double microH    = microHenry;
    static constexpr double uH        = microHenry;
    static constexpr double nH     = 1e-9 * H;
    static constexpr double nHenry = 1e-9 * H;
    static constexpr double nanoHenry = 1e-9 * H;
    static constexpr double nanohenry = 1e-9 * H;
    static constexpr double pH     = 1e-12* H;
    static constexpr double pHenry = 1e-12* H;

    // ---------- pressure units -------------------
    static constexpr double Pascal = Newton / (meter*meter);
    static constexpr double pascal = Pascal;
    static constexpr double Pa     = Pascal;
    static constexpr double kPa    = 1e3 * Pa;
    static constexpr double MPa    = 1e6 * Pa;
    static constexpr double GPa    = 1e9 * Pa;
    static constexpr double mPa    = 1e-3 * Pa;
    static constexpr double muPa   = 1e-6 * Pa;
    static constexpr double Torr   = 133.3223999*Pa;
    static constexpr double mTorr  = 1e-3 * Torr;
    static constexpr double bar    = 1e5 * Pa;  // = 750.062 Torr = 0.9869 atm
    static constexpr double mbar   = 1e-3 * bar;
    static constexpr double atm    = 760 * Torr;
    static constexpr double psi    = lbf/(inch*inch);   // pound per square-inch http://en.wikipedia.org/wiki/Pounds_per_square_inch
    static constexpr double ksi    = 1000*psi;          // kilo-pound per square-inch

    inline static double F2K(double F) { return (F+459.67)*5.0/9.0; } // Fahrenheit to Kelvin
    inline static double K2F(double K) { return K*9.0/5.0-459.67; }  // Kelvin to Fahrenheit
    inline static double F2C(double F) { return (F-32)*5.0/9.0; }    // Fahrenheit to Celsius
    inline static double C2F(double C) { return C*9.0/5.0+32;   }    // Celsius to Fahrenheit
    inline static function F2K(function F) { return (F+459.67)*5.0/9.0; } // Fahrenheit to Kelvin
    inline static function K2F(function K) { return K*9.0/5.0-459.67; }  // Kelvin to Fahrenheit
    inline static function F2C(function F) { return (F-32.0)*5.0/9.0; }    // Fahrenheit to Celsius
    inline static function C2F(function C) { return C*9.0/5.0+32.0;   }    // Celsius to Fahrenheit


    // ----------  radiation --------------------------------------
    static constexpr double Becquerel = 1/s;
    static constexpr double Bq = Becquerel;
    static constexpr double Curie = 3.7e10*Bq;
    static constexpr double Ci = Curie;
    static constexpr double mCi = 1e-3*Ci;
    static constexpr double muCi = 1e-6*Ci;
    static constexpr double gray = Joule/kg;  // 1 Joule absorbed in 1 kg
    static constexpr double Gy = gray;
    //static constexpr double rad = 0.01*Gy;  this already exists as radian!
    static constexpr double Sievert = 1;
    static constexpr double Sv = Sievert;
    static constexpr double mSv = 1e-3*Sv;
    static constexpr double muSv = 1e-6*Sv;
    static constexpr double rem = 0.01*Sv;
    static constexpr double mrem = 1e-3*rem;

}

namespace cons
{
     static constexpr  double c    = 299792458  * blop::unit::m/blop::unit::s;       // speed of light     [m/s] (CODATA, 2006)
     static constexpr  double h    = 6.6260693e-34 * blop::unit::J*blop::unit::s;    // Planck's constexprant  [Js] (CODATA, 2006)
     static constexpr  double hbar = h/2/pi;                             // Planck's constexprant  [Js]
     static constexpr  double kB   = 1.3806505e-23 * blop::unit::J/blop::unit::K;    // Boltzmann constexprant [J/K] (CODATA, 2006)
     static constexpr  double e    = 1.60217653e-19 * blop::unit::C;           // electron charge  [C] (CODATA, 2006)
     static constexpr  double epsilon0 = 8.854187817e-12;                // permittivity of free space [F/m] (CODATA 2006)
     static constexpr  double mu0      = 12.566370614e-7;                // permeability of free spa [N/A^2] (CODATA 2006)
     static const  double Z0       = ::sqrt(mu0/epsilon0);           // impedance of free space

     static constexpr  double m_proton   = 1.67262171e-27*blop::unit::kg;      // mass of proton (CODATA, 2006)
     static constexpr  double m_electron = 9.1093826e-31*blop::unit::kg;       // mass of electron (CODATA, 2006)
     static constexpr  double m_deuteron = 3.34358335e-27 * blop::unit::kg;    // mass of deuteron (CODATA, 2006)
     static constexpr  double m_helion   = 5.00641214e-27*blop::unit::kg;      // mass of 3He nucleus (CODATA, 2006)

     static constexpr  double bohr_radius = 0.5291772108e-10;            // [m] (CODATA, 2006)
     static constexpr  double bohr_magneton = e*(hbar/(2*m_electron)); 

     static constexpr  double Rydberg = 1.0973731568525e7;    // cons::m_electron*pow(cons::e,4)/(8*pow(cons::epsilon0,2)*pow(cons::h,3)*cons::c)

     // Coulomb's constexprant: F = k*q1*q2/r^2
     static constexpr  double ke = 8.9875517873681764e9*blop::unit::Newton*blop::unit::m2/(blop::unit::Coulomb*blop::unit::Coulomb);
     static constexpr double coulomb_constexprant = ke;

     // Avogadro's constexprant
     static constexpr  double Avogadro = 6.02214199e23;

     // fine structure constexprant
     static constexpr  double alpha = 7.297352568e-3; // e*e/2/epsilon0/h/c;      (CODATA value, 2006)

     // gravitational constexprant: F = G*m1*m2/r^2
     static constexpr  double G     = 6.67428e-11 * blop::unit::Newton*blop::unit::m*blop::unit::m/(blop::unit::kg*blop::unit::kg);
     // standard gravity (acceleration on the surface of earth)
     static constexpr  double g     = 9.80665 * blop::unit::m/(blop::unit::s*blop::unit::s); // http://en.wikipedia.org/wiki/Pound-force


}

namespace unit
{
   // ------------- atomic units --------------------------

     static constexpr  double atomic_mass     = cons::m_electron;

     static constexpr  double atomic_charge   = cons::e;

     static constexpr  double atomic_length   = cons::bohr_radius;

    // atomic unit of velocity: velocity in first bohr orbit
     static constexpr  double atomic_velocity = cons::e*cons::e/(4*cons::pi*cons::epsilon0*cons::hbar);

    // atomic unit of time: period of the first bohr orbit divided by 2pi
     static constexpr  double atomic_time     = cons::bohr_radius / atomic_velocity;
	                                       
     static constexpr  double atomic_frequency = 1.0/atomic_time;

    // atomic unit of energy, twice the ionization otential of atomic hydrogen, 27.2113845 eV
     static constexpr  double atomic_energy   = cons::e*cons::e/(4*cons::pi*cons::epsilon0*cons::bohr_radius);
     static constexpr  double hartree         = atomic_energy;

     static constexpr  double eV      = cons::e * unit::V;   //1.602176462e-19 * J;
     static constexpr  double keV     = eV * 1e3;
     static constexpr  double MeV     = eV * 1e6;
     static constexpr  double GeV     = eV * 1e9;
     static constexpr  double TeV     = eV * 1e12;
     static constexpr  double meV     = eV * 1e-3;

     // units for masses of elementary particles
     static constexpr  double MeVc2   = MeV*cons::c*cons::c;
     static constexpr  double keVc2   = keV*cons::c*cons::c;
     static constexpr  double GeVc2   = GeV*cons::c*cons::c;

    // Rydberg = ionization potential of hydrogen
     static constexpr  double Ry              = 0.5 * hartree;   // 13.60569172 * eV;
     static constexpr  double Rydberg         = Ry;
    
    // electric dipole
     static constexpr  double atomic_dipole   = atomic_charge * atomic_length;  // 2.54175*Debye


     string default_format();
     void default_format(const var &f);

}

namespace cons
{
     static constexpr double m_muon     = 105.658367*unit::MeV/(blop::cons::c*blop::cons::c); // http://pdg.lbl.gov/2010/tables/rpp2010-sum-leptons.pdf
     static constexpr double m_pion0    = 134.9766 * unit::MeV/(blop::cons::c*blop::cons::c); // http://pdg.lbl.gov/2010/tables/rpp2010-sum-mesons.pdf
     static constexpr double m_pionpm   = 139.57018* unit::MeV/(blop::cons::c*blop::cons::c);
     static constexpr double m_pion     = 0.5*(m_pion0+m_pionpm);
     static constexpr double m_kaonpm   = 493.677  * unit::MeV/(blop::cons::c*blop::cons::c);
     static constexpr double m_Kpm      = m_kaonpm;
     static constexpr double m_kaon0    = 497.614  * unit::MeV/(blop::cons::c*blop::cons::c);
     static constexpr double m_K0       = m_kaon0;

}

bool find_unit_or_cons(const var &name, double *value, string *symbol=0);
std::string symbol_of_unit_or_cons(const var &name);
double      value_of_unit_or_cons(const var &name);


}

#endif



