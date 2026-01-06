#include "units.h"
#include "warning.h"
#include <map>
#include <sstream>
#include <cstdlib>
using namespace std;


namespace blop
{
    class unit_cons_rep
    {
    public:
        unit_cons_rep() {}
        unit_cons_rep(const string s, double v) : symbol(s), value(v) {}
        string symbol;
        double value;
    };


    namespace unit
    {
	string default_format_ = "[$%s$]";
	void default_format(const var &f) { default_format_ = f.str(); }
	string default_format() { return default_format_; }

        map<string,unit_cons_rep> all_units_;

	void put(const string &a, const string &b, double v)
	{
	    all_units_[a] = unit_cons_rep(b,v);
	}

        void register_all_units_()
        {
#define PUTU(a) all_units_[#a] = unit_cons_rep(#a,unit::a)

            if(!all_units_.empty()) return;

            put("%","\\%",percent);
            put("percent","\\%",unit::percent);
            put("permill","\\permill",unit::permill);
            put("permil","\\permill",unit::permill);
            PUTU(ppm);

            put("kilogram","kg",unit::kg);
            PUTU(kg);
            PUTU(g);
            PUTU(amu);

            put("second","s",unit::s);
            PUTU(s);
            PUTU(ms);
            put("mus","{\\textmu}s",unit::mus);
            PUTU(ns);
            PUTU(ps);
            put("minute","min",unit::minute);
            put("min","min",unit::minute);
            PUTU(hour);
            PUTU(h);

            put("Hertz","Hz",unit::Hz);
            put("hertz","Hz",unit::Hz);
            PUTU(Hz);
            PUTU(kHz);
            PUTU(MHz);
            PUTU(GHz);

            put("meter","m",unit::meter);
            PUTU(m);
            PUTU(cm);
            PUTU(mm);
            put("mum"   ,"{\\textmu}m",unit::mum);
            put("um"   ,"{\\textmu}m",unit::mum);
            put("micron","{\\textmu}m",unit::mum);
            PUTU(nm);
            PUTU(pm);
            PUTU(fm);
            put("fermi","fm",unit::fm);
            put("angstrom","$\\AA$",unit::angstrom);
            put("Angstrom","$\\AA$",unit::angstrom);
            PUTU(km);
            PUTU(mile);
            PUTU(inch);
            PUTU(foot);
            put("m3","m$^3$",unit::m3);
            put("m2","m$^2$",unit::m2);
            put("cm3","cm$^3$",unit::cm3);
            put("cm2","cm$^2$",unit::cm2);
            put("mm3","mm$^3$",unit::mm3);
            put("mm2","mm$^2$",unit::mm2);

	 
            put("Joule","J",unit::J);
            put("joule","J",unit::J);
            PUTU(J);
            PUTU(mJ);
            PUTU(kJ);
            PUTU(MJ);
            PUTU(cal);
            PUTU(kcal);

            put("Watt","W",unit::W);
            put("watt","W",unit::W);
            PUTU(W);
            PUTU(kW);
            PUTU(MW);
            PUTU(GW);
            PUTU(mW);

            put("Coulomb","C",unit::C);
            put("coulomb","C",unit::C);
            PUTU(C);

            put("Ampere","A",unit::A);
            put("ampere","A",unit::A);
            PUTU(A);
            PUTU(mA);
            put("muA","{\\textmu}A",unit::muA);
            PUTU(nA);
            PUTU(kA);

            put("Newton","N",unit::N);
            put("newton","N",unit::N);
            PUTU(N);
            PUTU(kN);

            put("Kelvin","K",unit::K);
            put("kelvin","K",unit::K);
            PUTU(K);
            PUTU(mK);

            PUTU(mole);
            PUTU(mol);

            put("candela","cd",unit::cd);
            PUTU(cd);

            put("Volt","V",unit::V);
            put("volt","V",unit::V);
            PUTU(V);
            PUTU(mV);
            put("microVolt","{\\textmu}V",unit::uV);
            put("uV","{\\textmu}V",unit::uV);
            put("muV","{\\textmu}V",unit::uV);
            PUTU(kV);
            PUTU(MV);

            put("Weber","Wb",unit::Wb);
            put("weber","Wb",unit::Wb);
            put("Wb","Wb",unit::Wb);

            put("Tesla","T",unit::T);
            put("tesla","T",unit::T);
            PUTU(T);
            PUTU(mT);
            put("muT","{\\textmu}T",unit::muT);
            PUTU(nT);
            put("Gauss","G",unit::G);
            put("gauss","G",unit::G);
            put("mG","mG",unit::mG);
            put("mGauss","mG",unit::mG);
            put("kGauss","kG",unit::kG);
            put("kgauss","kG",unit::kG);
            PUTU(G);
            PUTU(kG);

            PUTU(Debye);

            put("Ohm","$\\Omega$",unit::Ohm);
            put("ohm","$\\Omega$",unit::Ohm);
            put("mOhm","m$\\Omega$",unit::mOhm);
            put("muOhm","\\textmu$\\Omega$",unit::muOhm);
            put("nOhm","n$\\Omega$",unit::nOhm);
            put("nanoOhm","n$\\Omega$",unit::nOhm);
            put("kOhm","k$\\Omega$",unit::kOhm);
            put("MOhm","M$\\Omega$",unit::MOhm);

            put("Siemens","S",unit::S);
            put("siemens","S",unit::S);
            PUTU(S);
            PUTU(mS);
            put("muS","{\\textmu}S",unit::muS);

            put("radian","rad",unit::rad);
            PUTU(rad);
            PUTU(mrad);
            put("degree","deg",unit::deg);
            PUTU(deg);

            put("Farad","F",unit::F);
            put("farad","F",unit::F);
            PUTU(F);
            PUTU(mF);
            put("muF","{\\textmu}F",unit::muF);
            put("nanoFarad","nF",unit::nF);
            put("nanofarad","nF",unit::nF);
            put("picoFarad","pF",unit::pF);
            put("picofarad","pF",unit::pF);
            PUTU(nF);
            PUTU(pF);

            put("Henry","H",unit::H);
            put("henry","H",unit::H);
            PUTU(H);
            PUTU(mH);
            put("muH","{\\textmu}H",unit::muH);
            PUTU(nH);
            PUTU(pH);

            put("Pascal","Pa",unit::Pa);
            put("pascal","Pa",unit::Pa);
            PUTU(Pa);
            PUTU(kPa);
            PUTU(MPa);
            PUTU(GPa);
            PUTU(mPa);
            put("muPa","{\\textmu}Pa",unit::muPa);
            PUTU(Torr);
            PUTU(mTorr);
            PUTU(bar);
            PUTU(mbar);
            PUTU(atm);

            put("atomic_mass","atomic\\_mass",unit::atomic_mass);
            put("atomic_charge","atomic\\_charge",unit::atomic_charge);
            put("atomic_length","atomic\\_length",unit::atomic_length);
            put("atomic_velocity","atomic\\_velocity",unit::atomic_velocity);
            put("atomic_time","atomic\\_time",unit::atomic_time);
            put("atomic_frequency","atomic\\_frequency",unit::atomic_frequency);
            put("atomic_energy","atomic\\_energy",unit::atomic_energy);
            put("atomic_dipole","atomic\\_dipole",unit::atomic_dipole);
            PUTU(hartree);

            PUTU(eV);
            PUTU(keV);
            PUTU(MeV);
            PUTU(GeV);
            PUTU(TeV);

            PUTU(Ry);
            put("Rydberg","Ry",Ry);
#undef PUTU
        }    

    }

    namespace cons
    {
        map<string,unit_cons_rep> all_cons_;

	void put(const string &a, const string &b, double v)
	{
	    all_cons_[a] = unit_cons_rep(b,v);
	}

        void register_all_cons_()
        {
#define PUT(a) all_cons_[#a] = unit_cons_rep(#a,a)

            if(!all_cons_.empty()) return;
            PUT(c);  // register the speed of light
            PUT(e);  // register the elementary charge
            put("pi","$\\pi$",pi);
            put("hbar","$\\hbar$",hbar);
            put("kB","$k_B$",kB);
            PUT(g);
            put("epsilon0","$\\epsilon_0$",epsilon0);
            put("mu0","$\\mu_0$",mu0);
            put("m_electron","$M_e$",m_electron);
            put("m_proton","$M_p$",m_proton);
            put("m_muon","$M_\\mu$",m_muon);
            put("m_pion","$M_\\pi$",m_pion);
#undef PUT            
        }
    }


    bool find_unit_or_cons(const var &n, double *value, string *symbol)
    {
        *value = 0;
        if(symbol) *symbol = "";
	    
        if(unit::all_units_.empty()) unit::register_all_units_();
        if(cons::all_cons_.empty() ) cons::register_all_cons_();

        // Replace the multiplication sign by a space
        string name = replace(var("*"),var(" "),n).str();

        // Replace multiple spaces by one space
        {
            string::size_type double_space;
            while((double_space=name.find("  ")) != string::npos)
            {
                name.replace(double_space,2," ");
            }
        }
        

        // find the border between nominator and denominator: a / followed by space of
        // a letter (i.e. a unit)
	    
        string::size_type slash = string::npos;
        for(string::size_type i=0; i<name.size()-1; ++i)
        {
            if(name[i] == '/' && (name[i+1] == ' ' || isalpha(name[i+1])))
            {
                slash = i;
                break;
            }
        }

        string ss[2];
        ss[0] = name;
        ss[1] = "";
        if(slash != string::npos)
        {
            ss[0] = name.substr(0,slash);
            ss[1] = name.substr(slash+1,string::npos);
        }
        // now: ss[0] is the nominator, ss[1] is the denominator

        *value = 1;
        string symbol_local  = "";
        for(int i=0; i<=1; ++i) // take both the nominator and the denominator
        {
            if(i==1 && symbol_local == "")
            {
                warning::print("Error in the unit: " & n);
                *value = 0;
                if(symbol) *symbol = "";
                return false;
            }
            if(ss[i] == "") continue;
            if(i==1) symbol_local += '/';
            string e;
            istringstream str(ss[i]);
            while(str>>e)
            {
                string unit_symbol = "";
                double unit_value = 0;
                string power_symbol = "";
                double power_value  = 1;

                if(isdigit(e[0]))
                {
                    char *endptr = 0;
            
                    string::size_type powpos = e.find("^");
                    if(powpos != string::npos)
                    {
                        unit_symbol = e.substr(0,powpos);
                        unit_value = strtod(unit_symbol.c_str(),&endptr);
                        power_symbol = e.substr(powpos+1,string::npos);
                        power_value  = strtod(power_symbol.c_str(),&endptr);
                    }
                    else
                    {
                        unit_symbol = e;
                        unit_value = strtod(unit_symbol.c_str(),&endptr);
                        power_symbol = "";
                        power_value = 1;
                    }
                }
                else
                {
                    string unitname  = e;  // unit name
                    power_symbol = ""; // power
                    string::size_type powpos = e.find("^");
                    if(powpos != string::npos)
                    {
                        unitname = e.substr(0,powpos);
                        power_symbol = e.substr(powpos+1,string::npos);
                    }

                    map<string,unit_cons_rep>::iterator ui = unit::all_units_.find(unitname);
                    map<string,unit_cons_rep>::iterator ci = cons::all_cons_.find(unitname);

                    // If this is not identified neither as a unit, nor as a constant, issue warning and return
                    if(ui == unit::all_units_.end() && ci == cons::all_cons_.end())
                    {
                        warning::print(string("Unit '") + unitname + string("' not found"));
                        *value = 0;
                        if(symbol) *symbol = "";
                        return false;
                    }

                    if(ui != unit::all_units_.end())
                    {
                        if(ci != cons::all_cons_.end())
                        {
                            warning::print(var("The symbol '") & unitname & "' is ambiguous (it can refer to a unit or a constant). Using the unit!",
                                           "find_unit_or_cons(\"" & n & "\", ...)");
                        }
                        unit_symbol = (*ui).second.symbol;
                        unit_value  = (*ui).second.value;
                    }
                    else
                    {
                        unit_symbol = (*ci).second.symbol;
                        unit_value  = (*ci).second.value;
                    }


                    power_value = 1;
                    if(power_symbol != "")
                    {
                        string::size_type powslash = power_symbol.find('/');
                        if(powslash == string::npos) 
                        {
                            // no slash found, so simply convert it to double
                            power_value = strtod(power_symbol.c_str(),0);
                        }
                        else
                        {
                            // a slash found, so convert the nominator and denominator to double
                            // separately, and divide them
                            string p1 = power_symbol.substr(0,powslash);
                            string p2 = power_symbol.substr(powslash+1,string::npos);
                            power_value = strtod(p1.c_str(),0)/strtod(p2.c_str(),0);
                        }
                    }
                }
                *value *= ::pow( unit_value, (i==0?1:-1)*power_value);
                if(symbol_local != "" && symbol_local[symbol_local.size()-1] != '/') symbol_local += "\\,";
                symbol_local += "\\mbox{" + unit_symbol + "}";
                if(power_symbol != "") symbol_local += "^{" + power_symbol + "}";
            }
        }
        if(symbol) *symbol = symbol_local;
        return true;
    }

    std::string symbol_of_unit_or_cons(const var &name)
    {
        string sym="";
        double val=0;
        find_unit_or_cons(name,&val,&sym);
        return sym;
    }
    double value_of_unit_or_cons(const var &name)
    {
        double val=0;
        find_unit_or_cons(name,&val);
        return val;
    }

}
