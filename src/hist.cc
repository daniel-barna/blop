#include "hist.h"
#include "graph_drawer.h"
#include <algorithm>
#include "x11_ps.h"
#include <limits>

namespace blop
{


// Class for histogram construction. Also can be used to control histogram 
// filling from file.

    histopt::histopt()
        : dim_(0), x_(unset), y_(unset), condition_(unset), x_condition_(unset), y_condition_(unset), maxlines_(-1), buffer_size_(0), dont_clear_buffer_(false)
    {
    }


    histopt::histopt(const histopt &h_other)
        : dim_(h_other.dim_), min_(h_other.min_), max_(h_other.max_), bins_(h_other.bins_), 
          binsize_(h_other.binsize_), 
          axistitle_(h_other.axistitle_), rangetitle_(h_other.rangetitle_), legend_(h_other.legend_), 
          x_(h_other.x_), y_(h_other.y_), condition_(unset), x_condition_(h_other.x_condition_), y_condition_(h_other.y_condition_), 
          maxlines_(h_other.maxlines_), dont_clear_buffer_(h_other.dont_clear_buffer_)
    {
    }


    histopt::~histopt()
    {
    }


    histopt& histopt::min(const int axis, const double minvalue)
    {
        if ( axis<1 ) return *this;
        if(axis>dim_) dim_=axis;
        if ( (int)min_.size()<axis ) min_.resize(axis, unset);
        min_[axis-1]=minvalue;
        return *this;
    }


    double histopt::min(const int axis) const
    {
        if ( axis<1 || axis>(int)min_.size() ) return unset;
        return min_[axis-1];
    }

    double histopt::default_min_=0.0;
    bool histopt::default_min_isset_=false;

    void histopt::default_min(const double Min)
    {
        default_min_=Min;
        default_min_isset_=true;
        if ( default_min_isset_==true && default_max_isset_==true && default_bins_isset_==true ) default_binsize_=(default_max_-default_min_)/default_bins_;
        else if ( default_min_isset_==true && default_max_isset_==true && default_binsize_isset_==true ) { default_bins_=(int)((default_max_-default_min_)/default_binsize_); default_bins_=(int)((default_max_-default_min_)/default_binsize_*(1.0+0.001/default_bins_)); }
        else if ( default_min_isset_==true && default_binsize_isset_==true && default_bins_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_max_isset_==true && default_binsize_isset_==true && default_bins_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_min_isset_==true && default_max_isset_==true ) default_binsize_=(default_max_-default_min_)/default_bins_;
        else if ( default_min_isset_==true && default_binsize_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_min_isset_==true && default_bins_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_max_isset_==true && default_binsize_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_max_isset_==true && default_bins_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_binsize_isset_==true && default_bins_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_min_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_max_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_bins_isset_==true ) default_binsize_=(default_max_-default_min_)/default_bins_;
        else if ( default_binsize_isset_==true ) { default_bins_=(int)((default_max_-default_min_)/default_binsize_); default_bins_=(int)((default_max_-default_min_)/default_binsize_*(1.0+0.001/default_bins_)); }
        else default_binsize_=(default_max_-default_min_)/default_bins_;
    }



    histopt& histopt::max(const int axis, const double maxvalue)
    {
        if ( axis<1 ) return *this;
        if(axis>dim_) dim_=axis;
        if ( (int)max_.size()<axis ) max_.resize(axis, unset);
        max_[axis-1]=maxvalue;
        return *this;
    }


    double histopt::max(const int axis) const
    {
        if ( axis<1 || axis>(int)max_.size() ) return unset;
        return max_[axis-1];
    }


    double histopt::default_max_=10.0;
    bool histopt::default_max_isset_=false;

    void histopt::default_max(const double Max)
    {
        default_max_=Max;
        default_max_isset_=true;
        if ( default_min_isset_==true && default_max_isset_==true && default_bins_isset_==true ) default_binsize_=(default_max_-default_min_)/default_bins_;
        else if ( default_min_isset_==true && default_max_isset_==true && default_binsize_isset_==true ) { default_bins_=(int)((default_max_-default_min_)/default_binsize_); default_bins_=(int)((default_max_-default_min_)/default_binsize_*(1.0+0.001/default_bins_)); }
        else if ( default_min_isset_==true && default_binsize_isset_==true && default_bins_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_max_isset_==true && default_binsize_isset_==true && default_bins_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_min_isset_==true && default_max_isset_==true ) default_binsize_=(default_max_-default_min_)/default_bins_;
        else if ( default_min_isset_==true && default_binsize_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_min_isset_==true && default_bins_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_max_isset_==true && default_binsize_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_max_isset_==true && default_bins_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_binsize_isset_==true && default_bins_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_min_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_max_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_bins_isset_==true ) default_binsize_=(default_max_-default_min_)/default_bins_;
        else if ( default_binsize_isset_==true ) { default_bins_=(int)((default_max_-default_min_)/default_binsize_); default_bins_=(int)((default_max_-default_min_)/default_binsize_*(1.0+0.001/default_bins_)); }
        else default_binsize_=(default_max_-default_min_)/default_bins_;
    }


    histopt& histopt::bins(const int axis, const int nbins)
    {
        if ( axis<1 ) return *this;
        if(axis>dim_) dim_=axis;
        if ( (int)bins_.size()<axis ) bins_.resize(axis, 0);
        bins_[axis-1]=nbins;
        return *this;
    }


    int histopt::bins(const int axis) const
    {
        if ( axis<1 || axis>(int)bins_.size() ) return 0;
        return bins_[axis-1];
    }


    int histopt::default_bins_=100;
    bool histopt::default_bins_isset_=false;

    void histopt::default_bins(const int Bins)
    {
        default_bins_=Bins;
        default_bins_isset_=true;
        if ( default_min_isset_==true && default_max_isset_==true && default_bins_isset_==true ) default_binsize_=(default_max_-default_min_)/default_bins_;
        else if ( default_min_isset_==true && default_max_isset_==true && default_binsize_isset_==true ) { default_bins_=(int)((default_max_-default_min_)/default_binsize_); default_bins_=(int)((default_max_-default_min_)/default_binsize_*(1.0+0.001/default_bins_)); }
        else if ( default_min_isset_==true && default_binsize_isset_==true && default_bins_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_max_isset_==true && default_binsize_isset_==true && default_bins_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_min_isset_==true && default_max_isset_==true ) default_binsize_=(default_max_-default_min_)/default_bins_;
        else if ( default_min_isset_==true && default_binsize_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_min_isset_==true && default_bins_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_max_isset_==true && default_binsize_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_max_isset_==true && default_bins_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_binsize_isset_==true && default_bins_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_min_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_max_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_bins_isset_==true ) default_binsize_=(default_max_-default_min_)/default_bins_;
        else if ( default_binsize_isset_==true ) { default_bins_=(int)((default_max_-default_min_)/default_binsize_); default_bins_=(int)((default_max_-default_min_)/default_binsize_*(1.0+0.001/default_bins_)); }
        else default_binsize_=(default_max_-default_min_)/default_bins_;
    }


    histopt& histopt::binsize(const int axis, const double binsize)
    {
        if ( axis<1 ) return *this;
        if(axis>dim_) dim_=axis;
        if ( (int)binsize_.size()<axis ) binsize_.resize(axis, unset);
        binsize_[axis-1]=binsize;
        return *this;
    }


    double histopt::binsize(const int axis) const
    {
        if ( axis<1 || axis>(int)binsize_.size() ) return unset;
        return binsize_[axis-1];
    }


    double histopt::default_binsize_=0.1;
    bool histopt::default_binsize_isset_=false;

    void histopt::default_binsize(const double Binsize)
    {
        default_binsize_=Binsize;
        default_binsize_isset_=true;
        if ( default_min_isset_==true && default_max_isset_==true && default_bins_isset_==true ) default_binsize_=(default_max_-default_min_)/default_bins_;
        else if ( default_min_isset_==true && default_max_isset_==true && default_binsize_isset_==true ) { default_bins_=(int)((default_max_-default_min_)/default_binsize_); default_bins_=(int)((default_max_-default_min_)/default_binsize_*(1.0+0.001/default_bins_)); }
        else if ( default_min_isset_==true && default_binsize_isset_==true && default_bins_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_max_isset_==true && default_binsize_isset_==true && default_bins_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_min_isset_==true && default_max_isset_==true ) default_binsize_=(default_max_-default_min_)/default_bins_;
        else if ( default_min_isset_==true && default_binsize_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_min_isset_==true && default_bins_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_max_isset_==true && default_binsize_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_max_isset_==true && default_bins_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_binsize_isset_==true && default_bins_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_min_isset_==true ) default_max_=default_min_+default_binsize_*default_bins_;
        else if ( default_max_isset_==true ) default_min_=default_max_-default_binsize_*default_bins_;
        else if ( default_bins_isset_==true ) default_binsize_=(default_max_-default_min_)/default_bins_;
        else if ( default_binsize_isset_==true ) { default_bins_=(int)((default_max_-default_min_)/default_binsize_); default_bins_=(int)((default_max_-default_min_)/default_binsize_*(1.0+0.001/default_bins_)); }
        else default_binsize_=(default_max_-default_min_)/default_bins_;
    }


    histopt& histopt::axistitle(const int axis, const var &Title)
    {
        if ( axis<1 ) return *this;
        if(axis>dim_) dim_=axis;
        if ( (int)axistitle_.size()<axis ) axistitle_.resize(axis, (var)"");
        axistitle_[axis-1]=Title;
        return *this;
    }


    var histopt::axistitle(const int axis)
    {
        if ( axis<1 || axis>(int)axistitle_.size() ) return (var)"";
        return axistitle_[axis-1];
    }


    histopt& histopt::rangetitle(const var &Title)
    {
        rangetitle_=Title;
        return *this;
    }


    var histopt::rangetitle()
    {
        return rangetitle_;
    }


    histopt& histopt::legend(const var &Title)
    {
        legend_=Title;
        return *this;
    }


    var histopt::legend()
    {
        return legend_;
    }


    function histopt::default_x_=ARG(1);
    function histopt::default_y_=unset;
    function histopt::default_condition_=unset;
    function histopt::default_x_condition_=unset;
    function histopt::default_y_condition_=unset;
    int histopt::default_maxlines_=-1;

    histopt& histopt::x(const function &f) { x_ = f; return *this; }
    histopt& histopt::x(const function &f1, const function &f2) { x_ = function(f1,f2); return *this; }
    histopt& histopt::x(const function &f1, const function &f2, const function &f3) { x_ = function(f1,f2,f3); return *this; }
    void histopt::default_x(const function &f)
    { default_x_=f; }
    void histopt::default_x(const function &f1, const function &f2)
    { default_x_=function(f1,f2); }
    void histopt::default_x(const function &f1, const function &f2, const function &f3)
    { default_x_=function(f1,f2,f3); }
    histopt& histopt::y(const function &f) { y_ = f; return *this; }
    void histopt::default_y(const function &f)
    { default_y_=f; }


// Class for merging domain side.

    mergeopt_side::mergeopt_side()
        : min_(0.0), max_(0.0), merge_(0)
    {
    }


    mergeopt_side::mergeopt_side(const double Min, const double Max, const int Merge)
        : min_(Min), max_(Max), merge_(Merge)
    {
    }


    mergeopt_side::mergeopt_side(const mergeopt_side &m_other)
        : min_(m_other.min_), max_(m_other.max_), merge_(m_other.merge_)
    {
    }


    mergeopt_side::~mergeopt_side()
    {
    }


// Class for managing the merging of the bins of a multidimensional histogram.

    mergeopt& mergeopt::side(const double Min, const double Max, const int Merge)
    {
        push_back(mergeopt_side(Min, Max, Merge));
        return *this;
    }


// Some local tabularization tools for writing histogram files.

    static var tabs(const int n, const var &s)
    {
        var r="";
        int N=8;
        if ( n*8-s.length()>0 ) N=n*8-s.length();
        for ( int i=0 ; i<=(N-1)/8 ; ++i ) r&="\t";
        return r;
    }


    static var tabs(const int n, const double s)
    {
        var r="";
        int N=8;
        std::ostringstream str;
        str << s;
        if ( n*8-str.str().length()>0 ) N=n*8-str.str().length();
        for ( int i=0 ; i<=(N-1)/8 ; ++i ) r&="\t";
        return r;
    }


// Class for histograms. Axis indices are 1-based on the user side!

    hist &hist::dup()
    {
        hist *result = new hist(*this);
        if(parent_) parent_->add(result);
        return *result;
    }


    hist::hist()
        : plottable(), buffer_size_(0), dim_(0)
    {
        pointtype(*default_point_drawer_);
    }


    hist::hist(const histopt &hopt)
        : plottable(), dim_(0)
    {
        reinit(hopt);
        pointtype(*default_point_drawer_);
    }


    hist::hist(const double Min, const double Max, const int Bins, const var &Title, 
               const var &rTitle, const var &Legend)
        : plottable(), dim_(0)
    {
        reinit(histopt().min(1,Min).max(1,Max).bins(1,Bins).axistitle(1,Title).rangetitle(rTitle).legend(Legend));
        pointtype(*default_point_drawer_);
    }


    hist::hist(const double Min1, const double Max1, const int Bins1,
               const double Min2, const double Max2, const int Bins2)

        : plottable(), dim_(0)
    {
        reinit(histopt().min(1,Min1).max(1,Max1).bins(1,Bins1).min(2,Min2).max(2,Max2).bins(2,Bins2));
        pointtype(*default_point_drawer_);
    }

    hist::hist(const double Min1, const double Max1, const int Bins1, const var &Title1, 
               const double Min2, const double Max2, const int Bins2, const var &Title2, 
               const var &rTitle, const var &Legend)
        : plottable(), dim_(0)
    {
        reinit(histopt().min(1,Min1).max(1,Max1).bins(1,Bins1).axistitle(1,Title1).min(2,Min2).max(2,Max2).bins(2,Bins2).axistitle(2,Title2).rangetitle(rTitle).legend(Legend));
        pointtype(*default_point_drawer_);
    }


    hist::hist(double Min1, double Max1, int Bins1,
               double Min2, double Max2, int Bins2,
               double Min3, double Max3, int Bins3)
        : plottable(), dim_(0)
    {
        reinit(histopt().min(1,Min1).max(1,Max1).bins(1,Bins1)
               .min(2,Min2).max(2,Max2).bins(2,Bins2)
               .min(3,Min3).max(3,Max3).bins(3,Bins3));
        pointtype(*default_point_drawer_);
    }


    hist::hist(const double Min1, const double Max1, const int Bins1, const var &Title1, 
               const double Min2, const double Max2, const int Bins2, const var &Title2, 
               const double Min3, const double Max3, const int Bins3, const var &Title3, 
               const var &rTitle, const var &Legend)
        : plottable(), dim_(0)
    {
        reinit(histopt().min(1,Min1).max(1,Max1).bins(1,Bins1).axistitle(1,Title1).min(2,Min2).max(2,Max2).bins(2,Bins2).axistitle(2,Title2).min(3,Min3).max(3,Max3).bins(3,Bins3).axistitle(3,Title3).rangetitle(rTitle).legend(Legend));
        pointtype(*default_point_drawer_);
    }


    hist::hist(const hist &h_other)
        : plottable(h_other), buffer_size_(h_other.buffer_size_), dim_(h_other.dim_), 
          min_(h_other.min_), max_(h_other.max_), bins_(h_other.bins_), 
          binsize_(h_other.binsize_), axistitle_(h_other.axistitle_), 
          rangetitle_(h_other.rangetitle_), 
          mul_(h_other.mul_), ind_(h_other.ind_), 
          h_(h_other.h_), m_(h_other.m_), 
          sequence_(h_other.sequence_), invsequence_(h_other.invsequence_), 
          datapoint_(h_other.datapoint_)
    {
        pointtype(*default_point_drawer_);
    }


    hist::~hist()
    {
    }


    void hist::flush_buffer() const
    {
        if(buffer_.empty()) return;

        // First determine the range of the data in the buffer along all dimensions
        vector<double> Min(dim_,numeric_limits<double>::max()), Max(dim_,numeric_limits<double>::min());
        for(unsigned int i=0; i<buffer_.size(); i+=dim_+1)
        {
            for(int d=0; d<dim_; ++d)
            {
                if(buffer_[i+d] < Min[d]) Min[d] = buffer_[i+d];
                if(buffer_[i+d] > Max[d]) Max[d] = buffer_[i+d];
            }
        }

        histopt hopt;
        hopt.dont_clear_buffer_ = true;
        for(int d=0; d<dim_; ++d)
        {
            // Add a little margin around the actual limits
            const double range = Max[d]-Min[d];
            Min[d] -= 0.05*range;
            Max[d] += 0.05*range;

            if(min(d+1)!=unset) hopt.min(d+1,min(d+1));
            else hopt.min(d+1,Min[d]);
            if(max(d+1)!=unset) hopt.max(d+1,max(d+1));
            else hopt.max(d+1,Max[d]);
            if(bins(d+1)!=0) hopt.bins(d+1,bins(d+1));
            else if(binsize(d+1)!=unset) hopt.binsize(d+1,binsize(d+1));
            else hopt.bins(d+1,100);
        }

        reinit(hopt);

        // set buffer_size_ to zero to avoid (recursive) buffering in the fill(...) calls in the loop below
        // and fill directly to the histogram instead
        buffer_size_ = 0;

        point p(dim_);
        for(unsigned int i=0; i<buffer_.size(); i+=dim_+1)
        {
            for(int d=0; d<dim_; ++d)
            {
                p[d+1] = buffer_[i+d];
            }
            fill(p,buffer_[i+dim_]);
        }

        // clear the buffer and set size to 0 so that no further buffering will occur
        buffer_.clear();

    }

    point_drawer *hist::default_point_drawer_ = new autopoint;

    void hist::default_pointtype(const point_drawer &p)
    {
        delete default_point_drawer_;
        if(dynamic_cast<const autopoint *>(&p)) default_point_drawer_ = new autopoint();
        else default_point_drawer_ = p.clone();
    }

    graph_drawer * &hist::default_graph_drawer_1_()
    {
        static graph_drawer *d = sxyerrorbars().clip_errorbars(true).clone();
        return d;
    }
    graph_drawer * &hist::default_graph_drawer_2_()
    {
        static graph_drawer *d = 0;
        if(d == 0)
        {
            d=csboxes().grid_foreground(true).clone();
        }
        return d;
    }

    void hist::default_drawstyle_1(const graph_drawer &d)
    {
        delete default_graph_drawer_1_();
        default_graph_drawer_1_() = d.clone();
    }
    void hist::default_drawstyle_2(const graph_drawer &d)
    {
        delete default_graph_drawer_2_();
        default_graph_drawer_2_() = d.clone();
    }

    void hist::set_graph_drawer_() const
    {
        if ( graph_drawer_!=0 ) delete graph_drawer_;
        graph_drawer_=0;
        if ( dim_==1 )      graph_drawer_ = default_graph_drawer_1_()->clone();
        else if ( dim_==2 )
        {
            graph_drawer_ = default_graph_drawer_2_()->clone();
            if(csboxes *sc = dynamic_cast<csboxes*>(graph_drawer_))
            {
                if(rangetitle_.str() != "")
                {
                    sc->color_title(rangetitle_);
                }
            }
        }
        else graph_drawer_=new lines;
    }


    void hist::reinit_(const int dim, 
                       const std::vector<double> &dbegin, 
                       const std::vector<double> &dend, 
                       const std::vector<int> &dbins, 
                       const std::vector<var> &dtitle, 
                       const var &rtitle, 
                       const var &hlegend) const
    {
        int i, j;
        dim_=dim;
        min_=dbegin;
        max_=dend;
        bins_=dbins;
        binsize_.resize(dim_);
        for ( j=0 ; j<dim_ ; ++j ) binsize_[j]=(max_[j]-min_[j])/bins_[j];
        axistitle_=dtitle;
        rangetitle_=rtitle;
        legend_=hlegend;
        int sizeh=(dim_>0 ? 1 : 0);
        mul_.resize(dim_);
        ind_.resize(dim_);
        if ( dim_>0 )
        {
            mul_[dim_-1]=1;
            for ( j=dim_-1 ; j>=0 ; --j )
            {
                sizeh*=bins_[j];
                if ( j<dim_-1 ) mul_[j]=mul_[j+1]*bins_[j+1];
            }
        }
        h_.resize(sizeh);
        m_.resize(sizeh*dim_);
        sequence_.resize(sizeh);
        invsequence_.resize(sizeh);
        for ( i=0 ; i<sizeh ; ++i )
        {
            h_[i]=0.0;
            for ( j=0 ; j<dim_ ; ++j )
            {
                m_[i*dim_+j]=1;
            }
            sequence_[i]=i;
            invsequence_[i]=i;
        }
        datapoint_.resize(dim_*2+2);

        set_graph_drawer_();

    }

    void hist::reinit(const histopt &hopt)
    {
        const hist &h = *this;
        h.reinit(hopt);
    }

    void hist::reinit(const histopt &hopt) const
    {
        dim_=hopt.dim();
        // The following cases should never be true since hopt in principle automatically updates its dim_ variable
        // when any of the following options are set, but I leeave it here for the time being
        if ( (int)hopt.min_.size()>dim_ ) dim_=hopt.min_.size();
        if ( (int)hopt.max_.size()>dim_ ) dim_=hopt.max_.size();
        if ( (int)hopt.bins_.size()>dim_ ) dim_=hopt.bins_.size();
        if ( (int)hopt.binsize_.size()>dim_ ) dim_=hopt.binsize_.size();
        if ( (int)hopt.axistitle_.size()>dim_ ) dim_=hopt.axistitle_.size();

        if(!hopt.dont_clear_buffer_)
        {
            buffer_size_ = hopt.buffer_size();
            buffer_.clear();
        }
        if(hopt.buffer_size()>0) buffer_.reserve(hopt.buffer_size()*(dim_+1));

        if ( dim_<=0 )
        {
            warning::print("Dimension is not greater than zero! Creating empty histogram.", "void hist::reinit(const histopt&)");
            reinit_(0, std::vector<double>(0), std::vector<double>(0), std::vector<int>(0), std::vector<var>(0), (var)"", (var)"");
            return;
        }
        min_.resize(dim_,unset);
        max_.resize(dim_,unset);
        bins_.resize(dim_,0);
        binsize_.resize(dim_,unset);
        axistitle_.resize(dim_);
        ind_.resize(dim_);
        mul_.resize(dim_);
        mul_[dim_-1]=1;
        int sizeh=1;
        for ( int j=dim_-1 ; j>=0 ; --j )
        {
            if ( hopt.min(j+1)!=unset && hopt.max(j+1)!=unset && hopt.bins(j+1)!=0 )
            {
                min_[j]=hopt.min(j+1);
                max_[j]=hopt.max(j+1);
                bins_[j]=hopt.bins(j+1);
                binsize_[j]=(max_[j]-min_[j])/bins_[j];
            }
            else if ( hopt.min(j+1)!=unset && hopt.max(j+1)!=unset && hopt.binsize(j+1)!=unset )
            {
                min_[j]=hopt.min(j+1);
                max_[j]=hopt.max(j+1);
                bins_[j]=(int)((max_[j]-min_[j])/hopt.binsize(j+1));
                bins_[j]=(int)((max_[j]-min_[j])/hopt.binsize(j+1)*(1.0+0.001/bins_[j]));
                binsize_[j]=(max_[j]-min_[j])/bins_[j];
            }
            else if ( hopt.min(j+1)!=unset && hopt.binsize(j+1)!=unset && hopt.bins(j+1)!=0 )
            {
                min_[j]=hopt.min(j+1);
                max_[j]=min_[j]+hopt.binsize(j+1)*hopt.bins(j+1);
                bins_[j]=hopt.bins(j+1);
                binsize_[j]=(max_[j]-min_[j])/bins_[j];
            }
            else if ( hopt.max(j+1)!=unset && hopt.binsize(j+1)!=unset && hopt.bins(j+1)!=0 )
            {
                min_[j]=hopt.max(j+1)-hopt.binsize(j+1)*hopt.bins(j+1);
                max_[j]=hopt.max(j+1); 
                bins_[j]=hopt.bins(j+1);
                binsize_[j]=(max_[j]-min_[j])/bins_[j];
            }
            else if ( hopt.min(j+1)!=unset && hopt.max(j+1)!=unset )
            {
                min_[j]=hopt.min(j+1);
                max_[j]=hopt.max(j+1);
                bins_[j]=histopt::default_bins_;
                binsize_[j]=(max_[j]-min_[j])/bins_[j];
            }
            else if ( hopt.min(j+1)!=unset && hopt.binsize(j+1)!=unset )
            {
                min_[j]=hopt.min(j+1);
                //max_[j]=min_[j]+hopt.binsize(j+1)*histopt::default_bins_;
                //bins_[j]=histopt::default_bins_;
                binsize_[j]=hopt.binsize(j+1);
            }
            else if ( hopt.max(j+1)!=unset && hopt.binsize(j+1)!=unset )
            {
                //min_[j]=hopt.max(j+1)-hopt.binsize(j+1)*histopt::default_bins_;
                max_[j]=hopt.max(j+1); 
                //bins_[j]=histopt::default_bins_;
                binsize_[j]=hopt.binsize(j+1);
            }
            else if ( hopt.min(j+1)!=unset && hopt.bins(j+1)!=0 )
            {
                min_[j]=hopt.min(j+1);
                bins_[j]=hopt.bins(j+1);
            }
            else if ( hopt.max(j+1)!=unset && hopt.bins(j+1)!=0 )
            {
                //min_[j]=hopt.max_[j]-histopt::default_binsize_*hopt.bins_[j];
                max_[j]=hopt.max(j+1); 
                bins_[j]=hopt.bins(j+1);
            }
            else if ( hopt.binsize(j+1)!=unset && hopt.bins(j+1)!=0 )
            {
                //min_[j]=histopt::default_min_;
                ///max_[j]=min_[j]+hopt.binsize_[j]*hopt.bins_[j];
                bins_[j]=hopt.bins(j+1);
                binsize_[j] = hopt.binsize(j+1);
            }
            else if ( hopt.min(j+1)!=unset )
            {
                min_[j]=hopt.min(j+1);
            }
            else if ( hopt.max(j+1)!=unset )
            {
                max_[j]=hopt.max(j+1); 
            }
            else if ( hopt.binsize(j+1)!=unset )
            {
                binsize_[j] = hopt.binsize(j+1);
            }
            else if ( hopt.bins(j+1)!=0 )
            {
                bins_[j]=hopt.bins(j+1);
            }

            if(min_[j]==unset || max_[j]==unset || bins_[j]<=0 || binsize_[j]==unset)
            {
                if(buffer_size_<=0)
                {
                    warning::print("Some axis limits of a histogram were not specified. Automatic buffering with 1000 entries will be used","");
                    buffer_size_ = 1000;
                }
            }

            // If we have enough information to set up the indexint multiplicators (?) then do it now. Otherwise postpone later (after buffering)
            if(min_[j]!=unset && max_[j]!=unset && bins_[j]!=0)
            {
                if ( j<dim_-1 ) mul_[j]=mul_[j+1]*bins_[j+1];
                sizeh*=bins_[j];
            }
            else
            {
                sizeh = 0;
            }

            if ( j<(int)hopt.axistitle_.size() ) axistitle_[j]=hopt.axistitle_[j];
            else axistitle_[j]="";
        }
        rangetitle_=hopt.rangetitle_;
        legend_=hopt.legend_;
        if(sizeh>0)
        {
            h_.resize(sizeh);
            m_.resize(sizeh*dim_);
            sequence_.resize(sizeh);
            invsequence_.resize(sizeh);
            for ( int i=0 ; i<sizeh ; ++i )
            {
                h_[i]=0.0;
                for ( int j=0 ; j<dim_ ; ++j )
                {
                    m_[i*dim_+j]=1;
                }
                sequence_[i]=i;
                invsequence_[i]=i;
            }
        }
        datapoint_.resize(dim_*2+2);

        set_graph_drawer_();

    }


    void hist::reinit(const double Min, const double Max, const int Bins, const var &Title, 
                      const var &rTitle, const var &Legend)
    {
        reinit(histopt().min(1,Min).max(1,Max).bins(1,Bins).axistitle(1,Title).rangetitle(rTitle).legend(Legend));
    }


    void hist::reinit(const double Min1, const double Max1, const int Bins1, const var &Title1, 
                      const double Min2, const double Max2, const int Bins2, const var &Title2, 
                      const var &rTitle, const var &Legend)
    {
        reinit(histopt().min(1,Min1).max(1,Max1).bins(1,Bins1).axistitle(1,Title1).min(2,Min2).max(2,Max2).bins(2,Bins2).axistitle(2,Title2).rangetitle(rTitle).legend(Legend));
    }


    void hist::reinit(const double Min1, const double Max1, const int Bins1, const var &Title1, 
                      const double Min2, const double Max2, const int Bins2, const var &Title2, 
                      const double Min3, const double Max3, const int Bins3, const var &Title3, 
                      const var &rTitle, const var &Legend)
    {
        reinit(histopt().min(1,Min1).max(1,Max1).bins(1,Bins1).axistitle(1,Title1).min(2,Min2).max(2,Max2).bins(2,Bins2).axistitle(2,Title2).min(3,Min3).max(3,Max3).bins(3,Bins3).axistitle(3,Title3).rangetitle(rTitle).legend(Legend));
    }


    void hist::reinit(const hist &h_other)
    {
        plottable::copy_style(h_other);
        dim_=h_other.dim_;
        min_=h_other.min_;
        max_=h_other.max_;
        bins_=h_other.bins_;
        binsize_=h_other.binsize_;
        axistitle_=h_other.axistitle_;
        rangetitle_=h_other.rangetitle_;
        mul_=h_other.mul_;
        ind_=h_other.ind_;
        h_=h_other.h_;
        m_=h_other.m_;
        sequence_=h_other.sequence_;
        invsequence_=h_other.invsequence_;
        datapoint_=h_other.datapoint_;
    }


    const hist& hist::operator=(const hist &h_other)
    {
        if ( dim_!=h_other.dim_ )
        {
            axistitle_.resize(h_other.dim_);
            if ( graph_drawer_!=0 ) delete graph_drawer_;
            graph_drawer_=h_other.graph_drawer_->clone();
        }
        dim_=h_other.dim_;
        min_=h_other.min_;
        max_=h_other.max_;
        bins_=h_other.bins_;
        binsize_=h_other.binsize_;
        mul_=h_other.mul_;
        ind_=h_other.ind_;
        h_=h_other.h_;
        m_=h_other.m_;
        sequence_=h_other.sequence_;
        invsequence_=h_other.invsequence_;
        datapoint_=h_other.datapoint_;
        return *this;
    }


    const hist& hist::operator+=(const hist &h_other)
    {
        if ( dim_!=h_other.dim_ )
        {
            warning::print("Non-matching histograms!", "const hist& hist::operator+=(const hist&)");
            return *this;
        }
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( min_[j]!=h_other.min_[j] || 
                 max_[j]!=h_other.max_[j]  || 
                 bins_[j]!=h_other.bins_[j] )
            {
                warning::print("Non-matching histograms!", "const hist& hist::operator+=(const hist&)");
                return *this;
            }
        }
        bool skip;
        int hsize=h_.size();
        for ( int i=0 ; i<hsize ; ++i )
        {
            skip=false;
            for ( int j=0 ; j<dim_ ; ++j )
            {
                if ( m_[i*dim_+j]!=h_other.m_[i*dim_+j] )
                {
                    warning::print("Non-matching histograms!", "const hist& hist::operator+=(const hist&)");
                    return *this;
                }
                if ( m_[i*dim_+j]==0 ) { skip=true ; break; }
            }
            if ( skip==false ) h_[i]+=h_other.h_[i];
        }
        return *this;
    }


    const hist& hist::operator-=(const hist &h_other)
    {
        if ( dim_!=h_other.dim_ )
        {
            warning::print("Non-matching histograms!", "const hist& hist::operator-=(const hist&)");
            return *this;
        }
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( min_[j]!=h_other.min_[j] || 
                 max_[j]!=h_other.max_[j]  || 
                 bins_[j]!=h_other.bins_[j] )
            {
                warning::print("Non-matching histograms!", "hist::operator-=(const hist&)");
                return *this;
            }
        }
        bool skip;
        int hsize=h_.size();
        for ( int i=0 ; i<hsize ; ++i )
        {
            skip=false;
            for ( int j=0 ; j<dim_ ; ++j )
            {
                if ( m_[i*dim_+j]!=h_other.m_[i*dim_+j] )
                {
                    warning::print("Non-matching histograms!", "const hist& hist::operator-=(const hist&)");
                    return *this;
                }
                if ( m_[i*dim_+j]==0 ) { skip=true ; break; }
            }
            if ( skip==false ) h_[i]-=h_other.h_[i];
        }
        return *this;
    }


    const hist& hist::operator*=(const hist &h_other)
    {
        if ( dim_!=h_other.dim_ )
        {
            warning::print("Non-matching histograms!", "const hist& hist::operator*=(const hist&)");
            return *this;
        }
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( min_[j]!=h_other.min_[j] || 
                 max_[j]!=h_other.max_[j]  || 
                 bins_[j]!=h_other.bins_[j] )
            {
                warning::print("Non-matching histograms!", "const hist& hist::operator*=(const hist&)");
                return *this;
            }
        }
        bool skip;
        int hsize=h_.size();
        for ( int i=0 ; i<hsize ; ++i )
        {
            skip=false;
            for ( int j=0 ; j<dim_ ; ++j )
            {
                if ( m_[i*dim_+j]!=h_other.m_[i*dim_+j] )
                {
                    warning::print("Non-matching histograms!", "const hist& hist::operator*=(const hist&)");
                    return *this;
                }
                if ( m_[i*dim_+j]==0 ) { skip=true ; break; }
            }
            if ( skip==false ) h_[i]*=h_other.h_[i];
        }
        return *this;
    }


    const hist& hist::operator*=(const meas &a)
    {
        for ( unsigned int i=0 ; i<sequence_.size() ; ++i ) h_[sequence_[i]]*=a;
        return *this;
    }


    const hist& hist::operator*=(const double a)
    {
        for ( unsigned int i=0 ; i<sequence_.size() ; ++i ) h_[sequence_[i]]*=a;
        return *this;
    }


    const hist& hist::operator/=(const hist &h_other)
    {
        if ( dim_!=h_other.dim_ )
        {
            warning::print("Non-matching histograms!", "const hist& hist::operator/=(const hist&)");
            return *this;
        }
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( min_[j]!=h_other.min_[j] || 
                 max_[j]!=h_other.max_[j]  || 
                 bins_[j]!=h_other.bins_[j] )
            {
                warning::print("Non-matching histograms!", "const hist& hist::operator/=(const hist&)");
                return *this;
            }
        }
        bool skip;
        int hsize=h_.size();
        for ( int i=0 ; i<hsize ; ++i )
        {
            skip=false;
            for ( int j=0 ; j<dim_ ; ++j )
            {
                if ( m_[i*dim_+j]!=h_other.m_[i*dim_+j] )
                {
                    warning::print("Non-matching histograms!", "const hist& hist::operator/=(const hist&)");
                    return *this;
                }
                if ( m_[i*dim_+j]==0 ) { skip=true ; break; }
            }
            if ( skip==false ) h_[i]/=h_other.h_[i];
        }
        return *this;
    }


    const hist& hist::operator/=(const meas &a)
    {
        for ( unsigned int i=0 ; i<sequence_.size() ; ++i ) h_[sequence_[i]]/=a;
        return *this;
    }


    const hist& hist::operator/=(const double a)
    {
        for ( unsigned int i=0 ; i<sequence_.size() ; ++i ) h_[sequence_[i]]/=a;
        return *this;
    }


    hist operator+(const hist &h, const hist &h_other)
    {
        hist result=h;
        result+=h_other;
        result.legend((const var)"("&h.legend()&"+"&h_other.legend()&")");
        return result;
    }


    hist operator-(const hist &h, const hist &h_other)
    {
        hist result=h;
        result-=h_other;
        result.legend((const var)"("&h.legend()&"-"&h_other.legend()&")");
        return result;
    }


    hist operator-(const hist &h)
    {
        hist result=h;
        result.annulate();
        result-=h;
        result.legend((const var)"(-"&h.legend()&")");
        return result;
    }


    hist operator*(const hist &h, const hist &h_other)
    {
        hist result=h;
        result*=h_other;
        result.legend((const var)"("&h.legend()&"*"&h_other.legend()&")");
        return result;
    }


    hist operator*(const hist &h, const meas &a)
    {
        hist result=h;
        result*=a;
        result.legend((const var)"("&h.legend()&"*("&a.to_var()&"))");
        return result;
    }


    hist operator*(const hist &h, const double da)
    {
        hist result=h;
        result*=da;
        result.legend((const var)"("&h.legend()&"*("&da&"))");
        return result;
    }


    hist operator*(const meas &a, const hist &h)
    {
        hist result=h;
        result*=a;
        result.legend((const var)"(("&a.to_var()&")*"&h.legend()&")");
        return result;
    }


    hist operator*(const double da, const hist &h)
    {
        hist result=h;
        result*=da;
        result.legend((const var)"(("&da&")*"&h.legend()&")");
        return result;
    }


    hist operator/(const hist &h, const hist &h_other)
    {
        hist result=h;
        result/=h_other;
        result.legend((const var)"("&h.legend()&"/"&h_other.legend()&")");
        return result;
    }


    hist operator/(const hist &h, const meas &a)
    {
        hist result=h;
        result/=a;
        result.legend((const var)"("&h.legend()&"/("&a.to_var()&"))");
        return result;
    }


    hist operator/(const hist &h, const double da)
    {
        hist result=h;
        result/=da;
        result.legend((const var)"("&h.legend()&"/("&da&"))");
        return result;
    }


    hist operator/(const meas &a, const hist &h)
    {
        hist result=h;
        result/=a;
        result.transform(1.0/_1);
        result.legend((const var)"(("&a.to_var()&")/"&h.legend()&")");
        return result;
    }


    hist operator/(const double da, const hist &h)
    {
        hist result=h;
        result/=da;
        result.transform(1.0/_1);
        result.legend((const var)"(("&da&")*"&h.legend()&")");
        return result;
    }


    hist hist::project_out(const int axis) const
    {
        if ( dim_<=1 )
        {
            warning::print("Dimension has to be greater than 1 for projection!", "hist hist::project_out(const int)");
            return hist();
        }
        int Axis=axis-1;
        if ( Axis<0 || Axis>=dim_ )
        {
            warning::print("Axis index out of range in projection! (1-based indices!!!)", "hist hist::project_out(const int)");
            return hist();
        }
        hist result;
        bool skip=false;
        int i=0, iprime=0, iprimeprime=0;
        int j=0;
        result.dim_=dim_-1;
        result.min_=min_;
        result.min_.erase(result.min_.begin()+Axis);
        result.max_=max_;
        result.max_.erase(result.max_.begin()+Axis);
        result.bins_=bins_;
        result.bins_.erase(result.bins_.begin()+Axis);
        result.binsize_=binsize_;
        result.binsize_.erase(result.binsize_.begin()+Axis);
        result.axistitle_=axistitle_;
        result.axistitle_.erase(result.axistitle_.begin()+Axis);
        result.legend_=(const var)"project_out("&legend_&", "&(Axis+1)&")";
        result.mul_=mul_;
        result.mul_.erase(result.mul_.begin()+Axis);
        result.ind_=ind_;
        result.ind_.erase(result.ind_.begin()+Axis);
        result.datapoint_=datapoint_;
        result.datapoint_.erase(result.datapoint_.begin()+Axis);
        result.datapoint_.erase(result.datapoint_.begin()+result.dim_+1+Axis);
        result.mul_[result.dim_-1]=1;
        for ( j=result.dim_-1 ; j>=0 ; --j )
        {
            if ( j<result.dim_-1 ) result.mul_[j]=result.mul_[j+1]*result.bins_[j+1];
        }
        int hsize=result.mul_[0]*result.bins_[0];
        result.h_.resize(hsize);
        result.m_.resize(hsize*result.dim_);
        result.sequence_.clear();
        result.invsequence_.clear();
        for ( i=0 ; i<hsize ; ++i )
        {
            result.ind_[result.dim_-1]=i;
            for ( j=result.dim_-1 ; j>=0 ; --j )
            {
                if ( j<result.dim_-1 ) result.ind_[j]=result.ind_[j+1]/result.bins_[j+1];
            }
            int iprimebase=0;
            result.h_[i]=0.0;
            for ( j=0 ; j<result.dim_ ; ++j )
            {
                result.ind_[j]=result.ind_[j]%result.bins_[j];
                if ( j<Axis ) iprimebase+=mul_[j]*result.ind_[j];
                else iprimebase+=mul_[j+1]*result.ind_[j];
                result.m_[i*result.dim_+j]=0;
            }
            for ( j=0 ; j<result.dim_ ; ++j )
            {
                for ( int indprime=0 ; indprime<bins_[Axis] ; ++indprime )
                {
                    iprime=iprimebase+mul_[Axis]*indprime;
                    if ( j<Axis )
                    {
                        if ( m_[iprime*dim_+j]==0 ) { result.m_[i*result.dim_+j]=0; break; }
                        if ( m_[iprime*dim_+j]>result.m_[i*result.dim_+j] ) result.m_[i*result.dim_+j]=m_[iprime*dim_+j];
                    }
                    else
                    {
                        if ( m_[iprime*dim_+j+1]==0 ) { result.m_[i*result.dim_+j]=0; break; }
                        if ( m_[iprime*dim_+j+1]>result.m_[i*result.dim_+j] ) result.m_[i*result.dim_+j]=m_[iprime*dim_+j+1];
                    }
                }
            }
        }
        iprimeprime=0;
        for ( i=0 ; i<hsize ; ++i )
        {
            int indprime;
            int iprimeprimeprimebase;
            int iprimeprimeprime;
            meas h;
            skip=false;
            result.ind_[result.dim_-1]=i;
            iprimeprimeprimebase=0;
            for ( j=result.dim_-1 ; j>=0 ; --j )
            {
                if ( result.m_[i*result.dim_+j]==0 ) skip=true;
                if ( j<result.dim_-1 ) result.ind_[j]=result.ind_[j+1]/result.bins_[j+1];
                if ( j<Axis ) iprimeprimeprimebase+=mul_[j]*(result.ind_[j]%result.bins_[j]);
                else iprimeprimeprimebase+=mul_[j+1]*(result.ind_[j]%result.bins_[j]);
            }
            if ( skip==false )
            {
                for ( indprime=0 ; indprime<bins_[Axis] ; ++indprime )
                {
                    iprimeprimeprime=iprimeprimeprimebase+mul_[Axis]*indprime;
                    h=h_[iprimeprimeprime];
                    for ( j=0 ; j<dim_ ; ++j ) h*=(double)(m_[iprimeprimeprime*dim_+j]);
                    result.h_[i]+=h;
                }
                result.sequence_.push_back(i);
                result.invsequence_.push_back(iprimeprime);
                ++iprimeprime;
                continue;
            }
            for ( j=0 ; j<result.dim_ ; ++j )
            {
                result.ind_[j]=result.ind_[j]%result.bins_[j];
            }
            for ( j=0 ; j<result.dim_ ; ++j )
            {
                while ( result.ind_[j]>=0 )
                {
                    iprime=0;
                    for ( int jprime=0 ; jprime<result.dim_ ; ++jprime ) iprime+=result.ind_[jprime]*result.mul_[jprime];
                    if ( result.m_[iprime*result.dim_+j]==0 ) { --result.ind_[j]; } else { break; }
                }
            }
            if ( (unsigned int)(iprime)>=result.invsequence_.size() )
            {
                warning::print("Index inconsistency when projecting histogram!", "hist hist::project_out(const int)");
                return hist();
            }
            result.invsequence_.push_back(result.invsequence_[iprime]);
            for ( indprime=0 ; indprime<bins_[Axis] ; ++indprime )
            {
                iprimeprimeprime=iprimeprimeprimebase+mul_[Axis]*indprime;
                h=h_[iprimeprimeprime];
                for ( j=0 ; j<dim_ ; ++j ) h*=(double)(m_[iprimeprimeprime*dim_+j]);
                result.h_[iprime]+=h;
            }
        }
        hsize=result.sequence_.size();
        for ( i=0 ; i<hsize ; ++i )
        {
            for ( j=0 ; j<result.dim_ ; ++j ) result.h_[result.sequence_[i]]/=(double)(result.m_[result.sequence_[i]*result.dim_+j]);
        }
        return result;
    }


    hist hist::slice(const int axis, const double level) const
    {
        if ( dim_<=1 )
        {
            warning::print("Dimension has to be greater than 1 for slicing!", "hist hist::slice(const int, const double)");
            return hist();
        }
        int Axis=axis-1;
        if ( Axis<0 || Axis>=dim_ )
        {
            warning::print("Axis index out of range in slice! (1-based indices!!!)", "hist hist::slice(const int, const double)");
            return hist();
        }
        bool skip=false;
        int i=0, iprime=0, iprimeprime=0;
        int j=0;
        hist result;
        int Level=(int)((level-min_[Axis])/binsize_[Axis]*(1.0+0.001/bins_[Axis]));
        if ( Level<0 ) { Level=0; } if ( Level>bins_[Axis]-1 ) { Level=bins_[Axis]-1; }
        result.dim_=dim_-1;
        result.min_=min_;
        result.min_.erase(result.min_.begin()+Axis);
        result.max_=max_;
        result.max_.erase(result.max_.begin()+Axis);
        result.bins_=bins_;
        result.bins_.erase(result.bins_.begin()+Axis);
        result.binsize_=binsize_;
        result.binsize_.erase(result.binsize_.begin()+Axis);
        result.axistitle_=axistitle_;
        result.axistitle_.erase(result.axistitle_.begin()+Axis);
        result.legend_=(const var)"slice("&legend_&", "&(Axis+1)&", "&(level)&")";
        result.mul_=mul_;
        result.mul_.erase(result.mul_.begin()+Axis);
        result.ind_=ind_;
        result.ind_.erase(result.ind_.begin()+Axis);
        result.datapoint_=datapoint_;
        result.datapoint_.erase(result.datapoint_.begin()+Axis);
        result.datapoint_.erase(result.datapoint_.begin()+result.dim_+1+Axis);
        result.mul_[result.dim_-1]=1;
        for ( j=result.dim_-1 ; j>=0 ; --j )
        {
            if ( j<result.dim_-1 ) result.mul_[j]=result.mul_[j+1]*result.bins_[j+1];
        }
        int hsize=result.mul_[0]*result.bins_[0];
        result.h_.resize(hsize);
        result.m_.resize(hsize*result.dim_);
        result.sequence_.clear();
        result.invsequence_.clear();
        for ( i=0 ; i<hsize ; ++i )
        {
            result.ind_[result.dim_-1]=i;
            for ( j=result.dim_-1 ; j>=0 ; --j )
            {
                if ( j<result.dim_-1 ) result.ind_[j]=result.ind_[j+1]/result.bins_[j+1];
            }
            int iprimebase=0;
            result.h_[i]=0.0;
            for ( j=0 ; j<result.dim_ ; ++j )
            {
                result.ind_[j]=result.ind_[j]%result.bins_[j];
                if ( j<Axis ) iprimebase+=mul_[j]*result.ind_[j];
                else iprimebase+=mul_[j+1]*result.ind_[j];
                result.m_[i*result.dim_+j]=0;
            }
            for ( int indprime=Level ; indprime>=0 ; --indprime )
            {
                iprime=iprimebase+mul_[Axis]*indprime;
                if ( m_[iprime*dim_+Axis]>0 ) break;
            }
            result.h_[i]=h_[iprime];
            for ( j=0 ; j<result.dim_ ; ++j )
            {
                if ( j<Axis ) result.m_[i*result.dim_+j]=m_[iprime*dim_+j];
                else result.m_[i*result.dim_+j]=m_[iprime*dim_+j+1];
            }
        }
        iprimeprime=0;
        for ( i=0 ; i<hsize ; ++i )
        {
            skip=false;
            result.ind_[result.dim_-1]=i;
            for ( j=result.dim_-1 ; j>=0 ; --j )
            {
                if ( result.m_[i*result.dim_+j]==0 ) skip=true;
                if ( j<result.dim_-1 ) result.ind_[j]=result.ind_[j+1]/result.bins_[j+1];
            }
            if ( skip==false )
            {
                result.sequence_.push_back(i);
                result.invsequence_.push_back(iprimeprime);
                ++iprimeprime;
                continue;
            }
            for ( j=0 ; j<result.dim_ ; ++j )
            {
                result.ind_[j]=result.ind_[j]%result.bins_[j];
            }
            for ( j=0 ; j<result.dim_ ; ++j )
            {
                while ( result.ind_[j]>=0 )
                {
                    iprime=0;
                    for ( int jprime=0 ; jprime<result.dim_ ; ++jprime ) iprime+=result.ind_[jprime]*result.mul_[jprime];
                    if ( result.m_[iprime*result.dim_+j]==0 ) { --result.ind_[j]; } else { break; }
                }
            }
            if ( (unsigned int)(iprime)>=result.invsequence_.size() )
            {
                warning::print("Index inconsistency when slicing histogram!", "hist hist::slice(const int, const double)");
                return hist();
            }
            result.invsequence_.push_back(result.invsequence_[iprime]);
        }
        return result;
    }


    meas hist::integral() const
    {
        meas result(0.0, 0.0);
        bool skip;
        double m;
        for ( unsigned int i=0 ; i<h_.size() ; ++i )
        {
            skip=false;
            m=1.0;
            for ( int j=0 ; j<dim_ ; ++j )
            {
                if ( m_[i*dim_+j]==0 ) { skip=true ; break; }
                m*=m_[i*dim_+j];
            }
            if ( skip==false && h_[i].isok() ) result+=h_[i]*m;
        }
        return result;
    }


    meas integral(const hist &h)
    {
        return h.integral();
    }


    hist apply(const function &f, const hist& h)
    {
        hist result=h;
        result.transform(f);
        result.legend(f.sprint().replace("_1", h.legend()));
        return result;
    }


    void hist::transform(const function &f)
    {
        for ( unsigned int i=0 ; i<sequence_.size() ; ++i )
        {
//	h_[sequence_[i]].transform(f);
            h_[sequence_[i]] = f(h_[sequence_[i]]); 
        }
    }


    bool hist::isfinite() const
    {
        for ( unsigned int i=0 ; i<sequence_.size() ; ++i )
        {
            if ( meas::isfinite(h_[sequence_[i]])==false ) return false;
        }
        return true;
    }


    bool hist::isinf() const
    {
        for ( unsigned int i=0 ; i<sequence_.size() ; ++i )
        {
            if ( meas::isinf(h_[sequence_[i]])==true ) return true;
        }
        return false;
    }


    bool hist::isnan() const
    {
        for ( unsigned int i=0 ; i<sequence_.size() ; ++i )
        {
            if ( meas::isnan(h_[sequence_[i]])==true ) return true;
        }
        return false;
    }


    bool hist::isset() const
    {
        for ( unsigned int i=0 ; i<sequence_.size() ; ++i )
        {
            if ( meas::isset(h_[sequence_[i]])==false ) return false;
        }
        return true;
    }


    bool hist::isunset() const
    {
        for ( unsigned int i=0 ; i<sequence_.size() ; ++i )
        {
            if ( meas::isunset(h_[sequence_[i]])==true ) return true;
        }
        return false;
    }


    bool hist::isfinite(const hist &h)
    {
        return h.isfinite();
    }


    bool hist::isinf(const hist &h)
    {
        return h.isinf();
    }


    bool hist::isnan(const hist &h)
    {
        return h.isnan();
    }


    bool hist::isset(const hist &h)
    {
        return h.isset();
    }


    bool hist::isunset(const hist &h)
    {
        return h.isunset();
    }


    void hist::mkfinite(const double newvalue)
    {
        for ( unsigned int i=0 ; i<h_.size() ; ++i )
        {
            if ( meas::isfinite(h_[i])==false ) { h_[i].v=newvalue; h_[i].e=newvalue; }
        }
    }


    void hist::annulate(const var &reset)
    {
        if ( reset=="reset" )
        {
            if ( dim_>0 ) sequence_.resize(mul_[0]*bins_[0]);
            int hsize=h_.size();
            for ( int i=0 ; i<hsize ; ++i )
            {
                for ( int j=0 ; j<dim_ ; ++j )
                {
                    m_[i*dim_+j]=1;
                }
                h_[i]=0.0;
                invsequence_[i]=i;
                sequence_[i]=i;
            }
        }
        else
        {
            for ( unsigned int i=0 ; i<h_.size() ; ++i )
            {
                h_[i]=0.0;
            }
        }
    }


    unsigned int hist::bin(const point &pos) const
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram! Strange.",
                           "unsigned int hist::bin(const point&) const");
            return 0;
        }
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<(int)pos.size() )
            {
                ind_[j]=(int)((pos[j+1]-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; }
                if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        return invsequence_[i];
    }


    unsigned int hist::bin(const datapoint &pos) const
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram! Strange.", "unsigned int hist::bin(const point&) const");
            return 0;
        }
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<(int)pos.size() )
            {
                ind_[j]=(int)((pos[j].dbl()-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        return invsequence_[i];
    }


    unsigned int hist::bin(const double x) const
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram! Strange.", "unsigned int hist::bin(const double) const");
            return 0;
        }
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        return invsequence_[i];
    }


    unsigned int hist::bin(const double x, const double y) const
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram! Strange.", "unsigned int hist::bin(const double, const double) const");
            return 0;
        }
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<2 )
            {
                ind_[j]=(int)((y-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        return invsequence_[i];
    }


    unsigned int hist::bin(const double x, const double y, const double z) const
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram! Strange.", "unsigned int hist::bin(const double, const double, const double) const");
            return 0;
        }
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<2 )
            {
                ind_[j]=(int)((y-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<3 )
            {
                ind_[j]=(int)((z-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        return invsequence_[i];
    }


    unsigned int hist::bin(const double x, const double y, const double z, const double t) const
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram! Strange.", "unsigned int hist::bin(const double, const double, const double, const double) const");
            return 0;
        }
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<2 )
            {
                ind_[j]=(int)((y-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<3 )
            {
                ind_[j]=(int)((z-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<4 )
            {
                ind_[j]=(int)((t-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        return invsequence_[i];
    }


    unsigned int hist::bin(const var &x) const
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram! Strange.", "unsigned int hist::bin(const double) const");
            return 0;
        }
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x.dbl()-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        return invsequence_[i];
    }


    unsigned int hist::bin(const var &x, const var &y) const
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram! Strange.", "unsigned int hist::bin(const double, const double) const");
            return 0;
        }
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x.dbl()-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<2 )
            {
                ind_[j]=(int)((y.dbl()-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        return invsequence_[i];
    }


    unsigned int hist::bin(const var &x, const var &y, const var &z) const
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram! Strange.", "unsigned int hist::bin(const double, const double, const double) const");
            return 0;
        }
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x.dbl()-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<2 )
            {
                ind_[j]=(int)((y.dbl()-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<3 )
            {
                ind_[j]=(int)((z.dbl()-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        return invsequence_[i];
    }


    unsigned int hist::bin(const var &x, const var &y, const var &z, const var &t) const
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram! Strange.", "unsigned int hist::bin(const double, const double, const double, const double) const");
            return 0;
        }
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x.dbl()-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<2 )
            {
                ind_[j]=(int)((y.dbl()-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<3 )
            {
                ind_[j]=(int)((z.dbl()-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<4 )
            {
                ind_[j]=(int)((t.dbl()-min_[j])/binsize_[j]);
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        return invsequence_[i];
    }


    hist::cell hist::pos(const unsigned int i) const
    {
        cell pos(dim_);
        int j;
        for ( j=0 ; j<dim_ ; ++j ) { pos[j+1].v=0.0; pos[j+1].e=0.0; }
        if ( i<sequence_.size() )
        {
            ind_[dim_-1]=sequence_[i];
            for ( j=dim_-1 ; j>=0 ; --j )
            {
                if ( j<dim_-1 ) ind_[j]=ind_[j+1]/bins_[j+1];
            }
            for ( j=0 ; j<dim_ ; ++j )
            {
                ind_[j]=ind_[j]%bins_[j];
                pos[j+1].v=min_[j]+(ind_[j]+0.5*m_[sequence_[i]*dim_+j])*binsize_[j];
                pos[j+1].e=0.5*binsize_[j]*m_[sequence_[i]*dim_+j];
            }
            return pos;
        }
        else warning::print("Index out of range!", "cell hist::pos(const unsigned int) const");
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram! Strange.", "cell hist::pos(const unsigned int) const");
            return pos;
        }
        ind_[dim_-1]=sequence_[sequence_.size()-1];
        for ( j=dim_-1 ; j>=0 ; --j )
        {
            if ( j<dim_-1 ) ind_[j]=ind_[j+1]/bins_[j+1];
        }
        for ( j=0 ; j<dim_ ; ++j )
        {
            ind_[j]=ind_[j]%bins_[j];
            pos[j+1].v=min_[j]+(ind_[j]+0.5*m_[sequence_[sequence_.size()-1]*dim_+j])*binsize_[j];
            pos[j+1].e=0.5*binsize_[j]*m_[sequence_[sequence_.size()-1]*dim_+j];
        }
        return pos;
    }


    meas hist::pos(const unsigned int i, const int axis) const
    {
        meas pos(0.0, 0.0);
        if ( axis<1 || axis>dim_ )
        {
            warning::print("Axis index out of range! (1-based indices!!!)", "meas hist::pos(const unsigned int, const int) const");
            return pos;
        }
        int j;
        if ( i<sequence_.size() )
        {
            ind_[dim_-1]=sequence_[i];
            for ( j=dim_-1 ; j>=0 ; --j )
            {
                if ( j<dim_-1 ) ind_[j]=ind_[j+1]/bins_[j+1];
                if ( j<=axis-1 ) break;
            }
            for ( j=0 ; j<dim_ ; ++j )
            {
                ind_[j]=ind_[j]%bins_[j];
                if ( j>=axis-1 ) break;
            }
            pos.v=min_[axis-1]+(ind_[axis-1]+0.5*m_[sequence_[i]*dim_+axis-1])*binsize_[axis-1];
            pos.e=0.5*binsize_[axis-1]*m_[sequence_[i]*dim_+axis-1];
            return pos;
        }
        else warning::print("Index out of range!", "meas hist::pos(const unsigned int, const int) const");
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram! Strange.", "meas hist::pos(const unsigned int, const int) const");
            return pos;
        }
        ind_[dim_-1]=sequence_[sequence_.size()-1];
        for ( j=dim_-1 ; j>=0 ; --j )
        {
            if ( j<dim_-1 ) ind_[j]=ind_[j+1]/bins_[j+1];
            if ( j<=axis-1 ) break;
        }
        for ( j=0 ; j<dim_ ; ++j )
        {
            ind_[j]=ind_[j]%bins_[j];
            if ( j>=axis-1 ) break;
        }
        pos.v=min_[axis-1]+(ind_[axis-1]+0.5*m_[sequence_[sequence_.size()-1]*dim_+axis-1])*binsize_[axis-1];
        pos.e=0.5*binsize_[axis-1]*m_[sequence_[sequence_.size()-1]*dim_+axis-1];
        return pos;
    }

    double hist::binsize(int axis) const
    {
        --axis;
        if(axis<0 || (unsigned int)axis>=binsize_.size())
        {
            warning::print("axis index is out of range","hist::binsize(int axis)");
            return 0;
        }
        return binsize_[axis];
    }

    meas& hist::operator[](const unsigned int i)
    {
        if ( i<sequence_.size() ) return h_[sequence_[i]];
        else warning::print("Index out of range!", "meas& hist::operator[](const unsigned int)");
        if ( sequence_.empty() )
        {
            warning::print("Uninitialized histogram!", "meas& hist::operator[](const unsigned int)");
            static meas meas_null(unset, unset);
            return meas_null;
        }
        return h_[sequence_[sequence_.size()-1]];
    }


    const meas& hist::operator[](const unsigned int i) const
    {
        if ( i<sequence_.size() ) return h_[sequence_[i]];
        else warning::print("Index out of range!", "const meas& hist::operator[](const unsigned int) const");
        if ( sequence_.empty() )
        {
            warning::print("Uninitialized histogram!", "const meas& hist::operator[](const unsigned int) const");
            static meas meas_null(unset, unset);
            return meas_null;
        }
        return h_[sequence_[sequence_.size()-1]];
    }

    hist& hist::fill_range(double x1, double x2, double weight)
    {
        if(dim_ != 1)
        {
            warning::print("Not a 1-dimensional histogram",
                           "hist::fill_range(double,double,double)");
            return *this;
        }
        if(x1 < min_[0])
        {
            weight *= (x2-min_[0])/(x2-x1);
            x1 = min_[0];
        }
        if(x2 > max_[0])
        {
            weight *= (max_[0]-x1)/(x2-x1);
            x2 = max_[0];
        }
        const int bin_index_1 = std::max((int)((x1-min_[0])/binsize_[0]),0);
        const int bin_index_2 = std::min((int)((x2-min_[0])/binsize_[0]),bins_[0]-1);

        for(int bin_index = bin_index_1; bin_index <= bin_index_2; ++bin_index)
        {
            const double x1_in = std::max(min_[0]+bin_index    *binsize_[0], x1);
            const double x2_in = std::min(min_[0]+(bin_index+1)*binsize_[0], x2);
            const double x = min_[0]+(bin_index+0.5)*binsize_[0];
            fill(x, weight * (x2_in-x1_in)/(x2-x1));
        }
        return *this;
    }


    hist &hist::fill(const point &pos, const double weight)
    {
        const hist &h = *this;
        h.fill(pos,weight);
        return *this;
    }

    const hist& hist::fill(const point &pos, const double weight) const
    {
        // Redirect to buffer if needed
        if(buffer_size_>0)
        {
            for(int i=1; i<=dim_; ++i) buffer_.push_back(i<=(int)pos.size()?pos[i]:0.0);
            buffer_.push_back(weight);
            if((int)buffer_.size()>=buffer_size_*(dim_+1)) flush_buffer();
            return *this;
        }

        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::fill(const point&, const double)");
            return *this;
        }
        h_[sequence_[bin(pos)]].fill(weight);
        return *this;
    }


    hist& hist::remove(const point &pos)
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::remove(const point&)");
            return *this;
        }
        h_[sequence_[bin(pos)]].remove();
        return *this;
    }


    hist& hist::remove(const point &pos, const double weight)
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::remove(const point&, const double)");
            return *this;
        }
        h_[sequence_[bin(pos)]].remove(weight);
        return *this;
    }



    hist& hist::fill(const datapoint &pos, const var &weight) 
    {
        // Redirect to buffer if needed
        if(buffer_size_>0)
        {
            for(int i=0; i<dim_; ++i) buffer_.push_back(i<(int)pos.size()?pos[i].dbl():0.0);
            buffer_.push_back(weight);
            if((int)buffer_.size()>=buffer_size_*(dim_+1)) flush_buffer();
            return *this;
        }

        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::fill(const point&, const double)");
            return *this;
        }
        h_[sequence_[bin(pos)]].fill(weight);
        return *this;
    }


    hist& hist::remove(const datapoint &pos)
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::remove(const point&)");
            return *this;
        }
        h_[sequence_[bin(pos)]].remove();
        return *this;
    }


    hist& hist::remove(const datapoint &pos, const var &weight)
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::remove(const point&, const double)");
            return *this;
        }
        h_[sequence_[bin(pos)]].remove(weight);
        return *this;
    }


    hist& hist::fill(const double x)
    {
        if(dim_!=1)
        {
            warning::print("Histogram must be 1 dimensional","hist::fill(const double x)");
            return *this;
        }
        if(buffer_size_>0)
        {
            buffer_.push_back(x);
            buffer_.push_back(1.0);
            if((int)buffer_.size()>=buffer_size_*(dim_+1)) flush_buffer();
            return *this;
        }

        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!",
                           "hist& hist::fill(const double)");
            return *this;
        }
        h_[sequence_[bin(x)]].fill();
        return *this;
    }


    hist& hist::fill(const double x, const double y)
    {
        if(dim_!=1 && dim_!=2)
        {
            warning::print("Histogram must be 1 or 2 dimensional","hist::fill(const double x, const double y)");
            return *this; 
        }

        if(buffer_size_>0)
        {
            buffer_.push_back(x);
            buffer_.push_back(y);
            if(dim_==2) buffer_.push_back(1.0); // weight
            if((int)buffer_.size()>=buffer_size_*(dim_+1)) flush_buffer();
            return *this;
        }
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!",
                           "hist& hist::fill(const double, const double, const double)");
            return *this;
        }
        if(dim_==1) h_[sequence_[bin(x)]].fill(y);
        else        h_[sequence_[bin(x,y)]].fill();
        return *this;
    }


    hist& hist::fill(const double x, const double y, const double z)
    {
        if(dim_!=2 && dim_!=3)
        {
            warning::print("Histogram must be 2 or 3 dimensional","hist::fill(const double x, const double y, const double z)");
            return *this; 
        }
        if(buffer_size_>0)
        {
            buffer_.push_back(x);
            buffer_.push_back(y);
            buffer_.push_back(z);
            if(dim_==3) buffer_.push_back(1.0); // weight
            if((int)buffer_.size()>=buffer_size_*(dim_+1)) flush_buffer();
            return *this;
        }
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!",
                           "hist& hist::fill(const double, const double, const double)");
            return *this;
        }
        if(dim_==2) h_[sequence_[bin(x, y)]].fill(z);
        else        h_[sequence_[bin(x, y, z)]].fill();
        return *this;
    }


    hist& hist::fill(const double x, const double y, const double z, const double t)
    {
        if(dim_!=3 && dim_!=4)
        {
            warning::print("Histogram must be3 or 4 dimensional","hist::fill(const double x, const double y, const double z, const double w)");
            return *this;
        }
        if(buffer_size_>0)
        {
            buffer_.push_back(x);
            buffer_.push_back(y);
            buffer_.push_back(z);
            buffer_.push_back(t);
            if(dim_==4) buffer_.push_back(1.0); // weight
            if((int)buffer_.size()>=buffer_size_*(dim_+1)) flush_buffer();
            return *this;
        }
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::fill(const double, const double, const double, const double)");
            return *this;
        }
        if(dim_==3)  h_[sequence_[bin(x, y, z)]].fill(t);
        else         h_[sequence_[bin(x, y, z, t)]].fill();
        return *this;
    }


    hist& hist::remove(const double x)
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::remove(const double)");
            return *this;
        }
        h_[sequence_[bin(x)]].remove();
        return *this;
    }


    hist& hist::remove(const double x, const double y)
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::remove(const double, const double)");
            return *this;
        }
        if ( dim_<2 ) h_[sequence_[bin(x)]].remove(y);
        else h_[sequence_[bin(x, y)]].remove();
        return *this;
    }


    hist& hist::remove(const double x, const double y, const double z)
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::remove(const double, const double, const double)");
            return *this;
        }
        if ( dim_<2 ) h_[sequence_[bin(x)]].remove(y);
        if ( dim_<3 ) h_[sequence_[bin(x, y)]].remove(z);
        else h_[sequence_[bin(x, y, z)]].remove();
        return *this;
    }


    hist& hist::remove(const double x, const double y, const double z, const double t)
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::remove(const double, const double, const double, const double)");
            return *this;
        }
        if ( dim_<2 ) h_[sequence_[bin(x)]].remove(y);
        if ( dim_<3 ) h_[sequence_[bin(x, y)]].remove(z);
        if ( dim_<4 ) h_[sequence_[bin(x, y, z)]].remove(t);
        else h_[sequence_[bin(x, y, z, t)]].remove();
        return *this;
    }




    hist& hist::remove(const var &x)
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::remove(const double)");
            return *this;
        }
        h_[sequence_[bin(x)]].remove();
        return *this;
    }


    hist& hist::remove(const var &x, const var &y)
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!", "hist& hist::remove(const double, const double)");
            return *this;
        }
        if ( dim_<2 ) h_[sequence_[bin(x)]].remove(y);
        else h_[sequence_[bin(x, y)]].remove();
        return *this;
    }


    hist& hist::remove(const var &x, const var &y, const var &z)
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!",
                           "hist& hist::remove(const double, const double, const double)");
            return *this;
        }
        if ( dim_<2 ) h_[sequence_[bin(x)]].remove(y);
        if ( dim_<3 ) h_[sequence_[bin(x, y)]].remove(z);
        else h_[sequence_[bin(x, y, z)]].remove();
        return *this;
    }


    hist& hist::remove(const var &x, const var &y, const var &z, const var &t)
    {
        if ( sequence_.empty() )
        {
            warning::print("Empty indexrange in histogram!",
                           "hist& hist::remove(const double, const double, const double, const double)");
            return *this;
        }
        if ( dim_<2 ) h_[sequence_[bin(x)]].remove(y);
        if ( dim_<3 ) h_[sequence_[bin(x, y)]].remove(z);
        if ( dim_<4 ) h_[sequence_[bin(x, y, z)]].remove(t);
        else h_[sequence_[bin(x, y, z, t)]].remove();
        return *this;
    }


    hist& hist::merge(const mergeopt &Mergeopt)
    {
        if ( dim_<=0 )
        {
            warning::print("Dimension is not greater than zero when merging!", "hist& hist::merge(const mergeopt&)");
            return *this;
        }
        if ( sequence_.empty() )
        {
            warning::print("Empty histogram when merging!", "hist& hist::merge(const mergeopt&)");
            return *this;
        }
        std::vector<int> indmin(dim_);
        std::vector<int> indmax(dim_);
        int j, jprime;
        for ( j=0 ; j<dim_ ; ++j )
        {
            if ( j<(int)Mergeopt.size() )
            {
                indmin[j]=(int)((Mergeopt[j].min_-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( indmin[j]<0 ) { indmin[j]=0; } if ( indmin[j]>bins_[j]-1 ) { indmin[j]=bins_[j]-1; }
                indmax[j]=(int)((Mergeopt[j].max_-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( indmax[j]<0 ) { indmax[j]=0; } if ( indmax[j]>bins_[j]-1 ) { indmax[j]=bins_[j]-1; }
            }
            else
            {
                indmin[j]=0;
                indmax[j]=1;
            }
        }
        int iprime=0;
        for ( j=0 ; j<dim_ ; ++j )
        {
            while ( indmin[j]>=0 )
            {
                iprime=0;
                for ( jprime=0 ; jprime<dim_ ; ++jprime ) iprime+=indmin[jprime]*mul_[jprime];
                if ( m_[iprime*dim_+j]==0 ) { --indmin[j]; } else { break; }
            }
            while ( indmax[j]>=0 )
            {
                iprime=0;
                for ( jprime=0 ; jprime<dim_ ; ++jprime ) iprime+=indmax[jprime]*mul_[jprime];
                if ( m_[iprime*dim_+j]==0 ) { --indmax[j]; } else { break; }
            }
        }
        std::vector<int> nbins(dim_);
        int iprimemax=1;
        for ( j=0 ; j<dim_ ; ++j )
        {
            if ( indmax[j]+m_[iprime*dim_+j]==bins_[j] ) indmax[j]=bins_[j];
            if ( indmax[j]<=indmin[j] )
            {
                warning::print("Maximum is not greater than minimum when merging!", "hist& hist::merge(const mergeopt&)");
                return *this;
            }
            if ( j<(int)Mergeopt.size() )
            {
                nbins[j]=(Mergeopt[j].merge_>0 && Mergeopt[j].merge_<=indmax[j]-indmin[j] ? Mergeopt[j].merge_ : indmax[j]-indmin[j]);
            }
            else nbins[j]=1;
            iprimemax*=(indmax[j]-indmin[j]);
        }
        sequence_.clear();
        invsequence_.clear();
        int i;
        int iprimeprime;
        meas h;
        int m=0;
        for ( iprime=0 ; iprime<iprimemax ; ++iprime )
        {
            iprimeprime=0;
            ind_[dim_-1]=iprime;
            for ( jprime=dim_-1 ; jprime>=0 ; --jprime )
            {
                if ( jprime<dim_-1 ) ind_[jprime]=ind_[jprime+1]/(indmax[jprime+1]-indmin[jprime+1]);
                iprimeprime+=mul_[jprime]*(indmin[jprime]+ind_[jprime]%(indmax[jprime]-indmin[jprime]));
            }
            for ( jprime=0 ; jprime<dim_ ; ++jprime ) h_[iprimeprime]*=(double)(m_[iprimeprime*dim_+jprime]);
        }
        for ( j=0 ; j<dim_ ; ++j )
        {
            for ( iprime=0 ; iprime<iprimemax ; ++iprime )
            {
                iprimeprime=0;
                ind_[dim_-1]=iprime;
                for ( jprime=dim_-1 ; jprime>=0 ; --jprime )
                {
                    if ( jprime<dim_-1 ) ind_[jprime]=ind_[jprime+1]/(indmax[jprime+1]-indmin[jprime+1]);
                    iprimeprime+=mul_[jprime]*(indmin[jprime]+ind_[jprime]%(indmax[jprime]-indmin[jprime]));
                }
                i=0;
                for ( jprime=0 ; jprime<dim_ ; ++jprime )
                {
                    ind_[jprime]=ind_[jprime]%(indmax[jprime]-indmin[jprime]);
                    if ( jprime!=j ) i+=mul_[jprime]*(indmin[jprime]+ind_[jprime]);
                    else
                    {
                        i+=mul_[jprime]*(indmin[jprime]+(ind_[jprime]/nbins[jprime])*nbins[jprime]);
                        h=h_[iprimeprime];
                        m=m_[iprimeprime*dim_+jprime];
                    }
                }
                h_[iprimeprime]=0.0;
                h_[i]+=h;
                m_[iprimeprime*dim_+j]=0;
                m_[i*dim_+j]+=m;
            }
        }
        for ( iprime=0 ; iprime<iprimemax ; ++iprime )
        {
            iprimeprime=0;
            ind_[dim_-1]=iprime;
            for ( jprime=dim_-1 ; jprime>=0 ; --jprime )
            {
                if ( jprime<dim_-1 ) ind_[jprime]=ind_[jprime+1]/(indmax[jprime+1]-indmin[jprime+1]);
                iprimeprime+=mul_[jprime]*(indmin[jprime]+ind_[jprime]%(indmax[jprime]-indmin[jprime]));
            }
            for ( jprime=0 ; jprime<dim_ ; ++jprime )
            {
                if ( m_[iprimeprime*dim_+jprime]>0 ) h_[iprimeprime]/=(double)(m_[iprimeprime*dim_+jprime]);
            }
        }
        int hsize=h_.size();
        iprimeprime=0;
        bool skip;
        for ( i=0 ; i<hsize ; ++i )
        {
            skip=false;
            ind_[dim_-1]=i;
            for ( j=dim_-1 ; j>=0 ; --j )
            {
                if ( m_[i*dim_+j]==0 ) skip=true;
                if ( j<dim_-1 ) ind_[j]=ind_[j+1]/bins_[j+1];
            }
            if ( skip==false )
            {
                sequence_.push_back(i);
                invsequence_.push_back(iprimeprime);
                ++iprimeprime;
                continue;
            }
            for ( j=0 ; j<dim_ ; ++j )
            {
                ind_[j]=ind_[j]%bins_[j];
            }
            for ( j=0 ; j<dim_ ; ++j )
            {
                while ( ind_[j]>=0 )
                {
                    iprime=0;
                    for ( jprime=0 ; jprime<dim_ ; ++jprime ) iprime+=ind_[jprime]*mul_[jprime];
                    if ( m_[iprime*dim_+j]==0 ) { --ind_[j]; } else { break; }
                }
            }
            if ( (unsigned int)(iprime)>=invsequence_.size() )
            {
                warning::print("Index inconsistency when merging histogram! Clearing histogram.", "hist& hist::merge(const mergeopt&)");
                reinit_(0, std::vector<double>(0), std::vector<double>(0), std::vector<int>(0), std::vector<var>(0), (var)"", (var)"");
                return *this;
            }
            invsequence_.push_back(invsequence_[iprime]);
        }
        return *this;
    }


    hist& hist::merge(const double Min, const double Max, const int Merge)
    {
        merge(mergeopt().side(Min, Max, Merge));
        return *this;
    }


    hist& hist::merge(const double Min1, const double Max1, const int Merge1, const double Min2, const double Max2, const int Merge2)
    {
        merge(mergeopt().side(Min1, Max1, Merge1).side(Min2, Max2, Merge2));
        return *this;
    }


    hist& hist::merge(const double Min1, const double Max1, const int Merge1, const double Min2, const double Max2, const int Merge2, const double Min3, const double Max3, const int Merge3)
    {
        merge(mergeopt().side(Min1, Max1, Merge1).side(Min2, Max2, Merge2).side(Min3, Max3, Merge3));
        return *this;
    }


    hist& hist::killbin(const unsigned int i)
    {
        if ( i<sequence_.size() ) { h_[sequence_[i]].v=unset; h_[sequence_[i]].e=unset; return *this; }
        else warning::print("Index out of range!", "hist& hist::killbin(const unsigned int)");
        return *this;
    }


    hist& hist::killpoint(const point &pos)
    {
        if ( dim_<=0 ) return *this;
        if ( sequence_.empty() ) return *this;
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<(int)pos.size() )
            {
                ind_[j]=(int)((pos[j+1]-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        killbin(invsequence_[i]);
        return *this;
    }


    hist& hist::killpoint(const datapoint &pos)
    {
        if ( dim_<=0 ) return *this;
        if ( sequence_.empty() ) return *this;
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<(int)pos.size() )
            {
                ind_[j]=(int)((pos[j].dbl()-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        killbin(invsequence_[i]);
        return *this;
    }


    hist& hist::killpoint(const double x)
    {
        if ( dim_<=0 ) return *this;
        if ( sequence_.empty() ) return *this;
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        killbin(invsequence_[i]);
        return *this;
    }


    hist& hist::killpoint(const double x, const double y)
    {
        if ( dim_<=0 ) return *this;
        if ( sequence_.empty() ) return *this;
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<2 )
            {
                ind_[j]=(int)((y-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        killbin(invsequence_[i]);
        return *this;
    }


    hist& hist::killpoint(const double x, const double y, const double z)
    {
        if ( dim_<=0 ) return *this;
        if ( sequence_.empty() ) return *this;
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<2 )
            {
                ind_[j]=(int)((y-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<3 )
            {
                ind_[j]=(int)((z-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        killbin(invsequence_[i]);
        return *this;
    }


    hist& hist::killpoint(const var &x)
    {
        if ( dim_<=0 ) return *this;
        if ( sequence_.empty() ) return *this;
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x.dbl()-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        killbin(invsequence_[i]);
        return *this;
    }


    hist& hist::killpoint(const var &x, const var &y)
    {
        if ( dim_<=0 ) return *this;
        if ( sequence_.empty() ) return *this;
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x.dbl()-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<2 )
            {
                ind_[j]=(int)((y.dbl()-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        killbin(invsequence_[i]);
        return *this;
    }


    hist& hist::killpoint(const var &x, const var &y, const var &z)
    {
        if ( dim_<=0 ) return *this;
        if ( sequence_.empty() ) return *this;
        int i=0;
        for ( int j=0 ; j<dim_ ; ++j )
        {
            if ( j<1 )
            {
                ind_[j]=(int)((x.dbl()-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<2 )
            {
                ind_[j]=(int)((y.dbl()-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else if ( j<3 )
            {
                ind_[j]=(int)((z.dbl()-min_[j])/binsize_[j]*(1.0+0.001/bins_[j]));
                if ( ind_[j]<0 ) { ind_[j]=0; } if ( ind_[j]>bins_[j]-1 ) { ind_[j]=bins_[j]-1; }
            }
            else ind_[j]=0;
            i+=mul_[j]*ind_[j];
        }
        killbin(invsequence_[i]);
        return *this;
    }


    hist hist::interpolate(const hist &other_scheme, const std::string &method) const
    {
        if ( dim_!=1 || other_scheme.dim_!=1 )
        {
            warning::print("Interpolation: 1d-histograms only!", "hist& hist::interpolate(const hist&, const std::string&) const");
            return hist();
        }
        hist result=other_scheme;
        result.annulate();
        result.axistitle_=axistitle_;
        result.rangetitle_=rangetitle_;
        result.legend_=legend_;
        std::vector<double> x, y, ey;
        for ( unsigned int i=0 ; i<size() ; ++i )
        {
            x.push_back(pos(i, 1).v);
            y.push_back(hist::operator[](i).v);
            ey.push_back(hist::operator[](i).e);
        }
        function y_func, ey_func;
        if ( method=="spline" )
        {
            y_func=function::interpolate_spline(x, y);
            ey_func=function::interpolate_spline(x, ey);
        }
        else
        {
            y_func=function::interpolate_linear(x, y);
            ey_func=function::interpolate_linear(x, ey);
        }
        for ( unsigned int i=0 ; i<result.size() ; ++i )
        {
            double pos=result.pos(i, 1).v;
            double val=y_func(pos).dbl();
            double err=ey_func(pos).dbl();
            result[i].v=val;
            result[i].e=err;
        }
        return result;
    }


    int hist::dim() const
    {
        return dim_;
    }


    hist& hist::axistitle(const int axis, const var &title)
    {
        if ( axis<1 || axis>dim_ )
        {
            warning::print("Axis index out of range! (1-based indices!!!)", "hist& hist::axistitle(const int, const var&)");
            return *this;
        }
        else axistitle_[axis-1]=title;
        return *this;
    }


    var hist::axistitle(const int axis) const
    {
        if ( axis<1 || axis>dim_ )
        {
            warning::print("Axis index out of range! (1-based indices!!!)", "var hist::axistitle(const int)");
            return "";
        }
        return axistitle_[axis-1];
    }


    hist& hist::rangetitle(const var &title)
    {
        rangetitle_=title;
        if ( dynamic_cast<cboxes*>(graph_drawer_) )
        {
            cboxes *graphd_cbox_tmp=0;
            if ( rangetitle_.str()!="" ) graphd_cbox_tmp=new cboxes(dynamic_cast<cboxes*>(graph_drawer_)->title(rangetitle_));
            else graphd_cbox_tmp=new cboxes(*(dynamic_cast<cboxes*>(graph_drawer_)));
            if ( graph_drawer_!=0 ) delete graph_drawer_;
            graph_drawer_=0;
            graph_drawer_=new cboxes(*graphd_cbox_tmp);
            delete graphd_cbox_tmp;
        }
        else if ( dynamic_cast<csboxes*>(graph_drawer_) )
        {
            csboxes *graphd_scbox_tmp=0;
            if ( rangetitle_.str()!="" ) graphd_scbox_tmp=new csboxes(dynamic_cast<csboxes*>(graph_drawer_)->title(rangetitle_));
            else graphd_scbox_tmp=new csboxes(*(dynamic_cast<csboxes*>(graph_drawer_)));
            if ( graph_drawer_!=0 ) delete graph_drawer_;
            graph_drawer_=0;
            graph_drawer_=new csboxes(*graphd_scbox_tmp);
            delete graphd_scbox_tmp;
        }
        else if ( dynamic_cast<isolines*>(graph_drawer_) )
        {
            isolines *graphd_isolines_tmp=0;
//	if ( rangetitle_.str()!="" ) graphd_isolines_tmp=new graphd_isolines(dynamic_cast<graphd_isolines*>(graph_drawer_)->title(rangetitle_));
//	else
            graphd_isolines_tmp=new isolines(*(dynamic_cast<isolines*>(graph_drawer_)));
            if ( graph_drawer_!=0 ) delete graph_drawer_;
            graph_drawer_=0;
            graph_drawer_=new isolines(*graphd_isolines_tmp);
            delete graphd_isolines_tmp;
        }
        return *this;
    }


    var hist::rangetitle() const
    {
        return rangetitle_;
    }


    hist& hist::legend(const var &title)
    {
        legend_=title;
        return *this;
    }


    var hist::legend() const
    {
        return legend_;
    }


    int hist::bins(const int axis) const
    {
        if ( axis<1 || axis>dim_ )
        {
            warning::print("Axis index out of range! (1-based indices!!!)", "int hist::bins(const int)");
            return 0;
        }
        return bins_[axis-1];
    }


    hist::write_many::write_many()
        : output_(NULL), isFlushed_(false)
    {
    }


    hist::write_many::write_many(ostream &output)
        : output_(&output), isFlushed_(false)
    {
    }


    hist::write_many::write_many(const var &datafilename)
        : dataFileName_(datafilename), output_(NULL), isFlushed_(false)
    {
    }


    hist::write_many& hist::write_many::reinit(ostream &output)
    {
        dataFileName_="";
        output_=&output;
        oHistos_.clear();
        isFlushed_=false;
        return *this;
    }


    hist::write_many& hist::write_many::reinit(const var &datafilename)
    {
        dataFileName_=datafilename;
        output_=NULL;
        oHistos_.clear();
        isFlushed_=false;
        return *this;
    }


    hist::write_many& hist::write_many::operator<<(hist &h)
    {
        oHistos_.push_back(&h);
        isFlushed_=false;
        return *this;
    }


    hist::write_many& hist::write_many::put(hist &h)
    {
        oHistos_.push_back(&h);
        isFlushed_=false;
        return *this;
    }


    hist::write_many& hist::write_many::flush()
    {
        int numHist=oHistos_.size();
        if ( numHist==0 ) { isFlushed_=true; return *this; }
        bool close=false;
        if ( output_==NULL ) { output_=openout(dataFileName_); close=true; }
        if ( output_==NULL )
        {
            warning::print("File was not opened successfully!", "hist::write_many& hist::write_many::flush()");
            isFlushed_=true;
            return *this;
        }
        for ( int k=1 ; k<numHist ; ++k )
        {
            if ( oHistos_[k]->dim_!=oHistos_[0]->dim_ )
            {
                warning::print("Non-matching histograms!", "hist::write& hist::write::flush()");
                if ( close==true ) { delete output_; output_=NULL; }
                isFlushed_=true;
                return *this;
            }
            for ( int j=0 ; j<oHistos_[0]->dim_ ; ++j )
            {
                if ( oHistos_[k]->min_[j]!=oHistos_[0]->min_[j] || 
                     oHistos_[k]->max_[j]!=oHistos_[0]->max_[j] || 
                     oHistos_[k]->bins_[j]!=oHistos_[0]->bins_[j] )
                {
                    warning::print("Non-matching histograms!", "hist::write& hist::write::flush()");
                    if ( close==true ) { delete output_; output_=NULL; }
                    isFlushed_=true;
                    return *this;
                }
            }
        }

        (*output_) << "# dimension:" << tabs(2, "# dimension:") << oHistos_[0]->dim_ << std::endl;
        for ( int j=0 ; j<oHistos_[0]->dim_ ; ++j )
        {
            (*output_) << "# minimum(" << j+1 << "):" << tabs(2, (const var)"# minimum("&(const var)(j+1)&"):") << oHistos_[0]->min_[j] << tabs(2, oHistos_[0]->min_[j]) << std::endl;
            (*output_) << "# maximum(" << j+1 << "):" << tabs(2, (const var)"# maximum("&(const var)(j+1)&"):") << oHistos_[0]->max_[j] << tabs(2, oHistos_[0]->max_[j]) << std::endl;
            (*output_) << "# bins(" << j+1 << "):" << tabs(2, (const var)"# bins("&(const var)(j+1)&"):") << oHistos_[0]->bins_[j] << tabs(2, oHistos_[0]->bins_[j]) << std::endl;
        }
        (*output_) << "# legend:" << tabs(4+(oHistos_[0]->dim_-1)*6, "# legend:");
        for ( int k=0 ; k<numHist ; ++k )
        {
            (*output_) << "\t\t" << "`" << oHistos_[k]->legend_ << "`" << tabs(4, (const var)"`"&oHistos_[k]->legend_&"`");
        }
        (*output_) << std::endl;
        (*output_) << "#";
        for ( int j=0 ; j<oHistos_[0]->dim_ ; ++j )
        {
            if ( j==0 ) { (*output_) << " " << "`" << oHistos_[0]->axistitle_[j] << "`" << tabs(4, (const var)"`"&oHistos_[0]->axistitle_[j]&"`"); }
            else { (*output_) << "\t\t" << "`" << oHistos_[0]->axistitle_[j] << "`" << tabs(4, (const var)"`"&oHistos_[0]->axistitle_[j]&"`"); }
        }
        for ( int k=0 ; k<numHist ; ++k )
        {
            (*output_) << "\t\t" << "`" << oHistos_[k]->rangetitle_ << "`" << tabs(4, (const var)"`"&oHistos_[k]->rangetitle_&"`");
        }
        (*output_) << std::endl;

        bool skip;
        meas pos;
        var sval, seval;
        int hsize=oHistos_[0]->h_.size();
        int i;
        int j;
        int k;
        int nendl;
        int l;
        for ( i=0 ; i<hsize ; ++i )
        {
            skip=false;
            oHistos_[0]->ind_[oHistos_[0]->dim_-1]=i;
            for ( j=oHistos_[0]->dim_-1 ; j>=0 ; --j )
            {
                for ( k=1 ; k<numHist ; ++k )
                {
                    if ( oHistos_[k]->m_[i*oHistos_[0]->dim_+j]!=oHistos_[0]->m_[i*oHistos_[0]->dim_+j] )
                    {
                        warning::print("Non-matching histograms!", "hist::write& hist::write::flush()");
                        if ( close==true ) { delete output_; output_=NULL; }
                        isFlushed_=true;
                        return *this;
                    }
                }
                if ( oHistos_[0]->m_[i*oHistos_[0]->dim_+j]==0 ) { skip=true; break; }
                if ( j<oHistos_[0]->dim_-1 ) oHistos_[0]->ind_[j]=oHistos_[0]->ind_[j+1]/oHistos_[0]->bins_[j+1];
            }
            if ( skip==true ) continue;
            for ( j=0 ; j<oHistos_[0]->dim_ ; ++j )
            {
                oHistos_[0]->ind_[j]=oHistos_[0]->ind_[j]%oHistos_[0]->bins_[j];
                pos.v=oHistos_[0]->min_[j]+(oHistos_[0]->ind_[j]+0.5*oHistos_[0]->m_[i*oHistos_[0]->dim_+j])*oHistos_[0]->binsize_[j];
                pos.e=0.5*oHistos_[0]->binsize_[j]*oHistos_[0]->m_[i*oHistos_[0]->dim_+j];
                if ( j==0 ) { (*output_) << "  " << pos.v << tabs(2, pos.v) << " +/- "  << pos.e << tabs(2, (const var)" +/- "&pos.e); }
                else { (*output_) << "\t\t" << pos.v << tabs(2, pos.v) << " +/- "  << pos.e << tabs(2, (const var)" +/- "&pos.e); }
            }
            for ( k=0 ; k<numHist ; ++k )
            {
                if ( oHistos_[k]->h_[i].v==unset ) sval="unset";
                else sval=oHistos_[k]->h_[i].v;
                if ( oHistos_[k]->h_[i].e==unset ) seval="unset";
                else seval=oHistos_[k]->h_[i].e;
                (*output_) << "\t\t" << sval << tabs(2, sval) 
                           << " +/- "  << seval << tabs(2, (const var)" +/- "&seval);
            }
            nendl=1;
            for ( j=oHistos_[0]->dim_-1 ; j>=0 ; --j )
            {
                if ( oHistos_[0]->ind_[j]+oHistos_[0]->m_[i*oHistos_[0]->dim_+j]!=oHistos_[0]->bins_[j] ) break;
                ++nendl;
            }
            for ( l=0 ; l<nendl ; ++l ) (*output_) << std::endl;
        }
        (*output_) << "# end" << std::endl;
        if ( close==true ) { delete output_; output_=NULL; }
        isFlushed_=true;
        return *this;
    }


    hist::write_many::~write_many()
    {
        if ( isFlushed_==false ) flush();
    }


    hist& hist::write(ostream &output)
    {
        write_many(output)<<(*this);
        return *this;
    }


    hist& hist::write(const var &datafilename)
    {
        write_many(datafilename)<<(*this);
        return *this;
    }


    hist& hist::fill_from(istream &input, const histopt &hopt)
    {
        if(buffer_size_>0) warning::print("hist::fill_from(istream&,const histopt &) does not handle buffering yet","hist::fill_from");
        function condition=unset;
        if ( hopt.x_condition_.initialized() ) condition.append(hopt.x_condition_);
        else if ( histopt::default_x_condition_.initialized() ) condition.append(histopt::default_x_condition_);
        if ( hopt.y_condition_.initialized() ) condition.append(hopt.y_condition_);
        else if ( histopt::default_y_condition_.initialized() ) condition.append(histopt::default_y_condition_);
        function filter=unset;
        if ( hopt.x_.initialized() ) filter.append(hopt.x_);
        else if ( histopt::default_x_.initialized() ) filter.append(histopt::default_x_condition_);
        if ( hopt.y_.initialized() ) filter.append(hopt.y_condition_);
        else if ( histopt::default_y_.initialized() ) filter.append(histopt::default_y_condition_);
        string line, buff;
        vector<var> col;
        datapoint pt;
        hist::point pos;
        int lines=0;
        int columns=0;
        int columns1=0;
        double weight=1.0;
        bool skip;
        std::istringstream iss;
        while ( getline(input, line) )
        {
            if ( hopt.maxlines_>=0 && lines>=hopt.maxlines_ ) break;
            else if ( histopt::default_maxlines_>=0 && lines>=histopt::default_maxlines_ ) break;
            iss.str(line);
            col.clear();
            while ( (iss>>buff) ) { col.push_back((var)(buff)); }
            iss.str("");
            iss.clear();
            if ( col.empty() ) continue;
            if ( col[0][0]=='#' ) continue;
            if ( hopt.condition().initialized() ) if ( hopt.condition().eval(col).dbl()==0.0 ) continue;
            if ( condition.initialized() ) if ( condition.eval(col).dbl()==0.0 ) continue;
            if ( filter.initialized() ) columns=filter.components();
            else columns=col.size();
            if ( columns<=0 ) continue;
            pt.resize(columns);
            if ( filter.initialized() ) filter.meval(col, pt);
            else for( int i=0 ; i<columns; ++i ) pt[i]=col[i];
            if ( hopt.y_.initialized() ) columns1=columns-1;
            else if ( histopt::default_y_.initialized() ) columns1=columns-1;
            else columns1=columns;
            if ( columns1<=0 ) continue;
            pos.resize(columns1);
            skip=false;
            for ( int i=0 ; i<columns1 ; ++i )
            {
                if ( ignore::it(pt[i])==true ) { skip=true; break; }
                pos[i+1]=pt[i].dbl();
            }
            if ( skip==true ) continue;
            if ( columns1<columns ) weight=pt[columns1].dbl();
            else weight=1.0;
            this->fill(pos, weight);
            ++lines;
        }
        return *this;
    }


    hist& hist::fill_from(const var &datafilename, const histopt &hopt)
    {
        istream *input=openin(datafilename);
        fill_from(*input, hopt);
        delete input;
        return *this;
    }


    hist& hist::fill_from(const dgraph &dg, const histopt &hopt)
    {
        if(buffer_size_>0) warning::print("hist::fill_from(const dgraph&, const histopt &) does not handle buffering yet","hist::fill_from");
        function condition=unset;
        if ( hopt.x_condition_.initialized() ) condition.append(hopt.x_condition_);
        else if ( histopt::default_x_condition_.initialized() ) condition.append(histopt::default_x_condition_);
        if ( hopt.y_condition_.initialized() ) condition.append(hopt.y_condition_);
        else if ( histopt::default_y_condition_.initialized() ) condition.append(histopt::default_y_condition_);
        function filter=unset;
        if ( hopt.x_.initialized() ) filter.append(hopt.x_);
        else if ( histopt::default_x_.initialized() ) filter.append(histopt::default_x_condition_);
        if ( hopt.y_.initialized() ) filter.append(hopt.y_condition_);
        else if ( histopt::default_y_.initialized() ) filter.append(histopt::default_y_condition_);
        datapoint pt;
        hist::point pos;
        int columns=0;
        int columns1=0;
        double weight=1.0;
        bool skip;
        for ( int j=0 ; j<(int)dg.size() ; ++j )
        {
            if ( hopt.maxlines_>=0 && j>=hopt.maxlines_ ) break;
            else if ( histopt::default_maxlines_>=0 && j>=histopt::default_maxlines_ ) break;
            if ( dg.get(j)->empty() ) continue;
            if ( hopt.condition().initialized() ) if ( hopt.condition().eval((*dg.get(j))).dbl()==0.0 ) continue;
            if ( condition.initialized() ) if ( condition.eval((*dg.get(j))).dbl()==0.0 ) continue;
            if ( filter.initialized() ) columns=filter.components();
            else columns=dg.get(j)->size();
            if ( columns<=0 ) continue;
            pt.resize(columns);
            if ( filter.initialized() ) filter.meval((*dg.get(j)), pt);
            else for( int i=0 ; i<columns; ++i ) pt[i]=(*dg.get(j))[i];
            if ( hopt.y_.initialized() ) columns1=columns-1;
            else if ( histopt::default_y_.initialized() ) columns1=columns-1;
            else columns1=columns;
            if ( columns1<=0 ) continue;
            pos.resize(columns1);
            skip=false;
            for( int i=0 ; i<columns1; ++i )
            {
                if ( ignore::it(pt[i])==true ) { skip=true; break; }
                pos[i+1]=pt[i].dbl();
            }
            if ( skip==true ) continue;
            if ( columns1<columns ) weight=pt[columns1].dbl();
            else weight=1.0;
            this->fill(pos, weight);
        }
        return *this;
    }


    hist::read_many::read_many()
        : input_(NULL), isFlushed_(false)
    {
    }


    hist::read_many::read_many(istream &input)
        : input_(NULL), isFlushed_(false)
    {
        reinit(input);
    }


    hist::read_many::read_many(const var &datafilename)
        : input_(NULL), isFlushed_(false)
    {
        reinit(datafilename);
    }


    hist::read_many& hist::read_many::reinit(istream &input)
    {
        dataFileName_="";
        input_=&input;
        iHistos_.clear();
        isFlushed_=false;
        return *this;
    }


    hist::read_many& hist::read_many::reinit(const var &datafilename)
    {
        if ( input_!=NULL ) { delete input_; input_=NULL; }
        dataFileName_=datafilename;
        iHistos_.clear();
        isFlushed_=false;
        return *this;
    }


    hist::read_many& hist::read_many::operator>>(hist &h)
    {
        int number_of_histo=iHistos_.size()+1;
        iHistos_.push_back(std::pair<hist*, int>(&h, number_of_histo));
        isFlushed_=false;
        return *this;
    }


    hist::read_many& hist::read_many::operator>>(const int number_of_histo)
    {
        if ( iHistos_.size()>0 ) iHistos_[iHistos_.size()-1].second=number_of_histo;
        isFlushed_=false;
        return *this;
    }


    hist::read_many& hist::read_many::get(hist &h, const int number_of_histo)
    {
        iHistos_.push_back(std::pair<hist*, int>(&h, number_of_histo));
        isFlushed_=false;
        return *this;
    }


    hist::read_many& hist::read_many::flush()
    {
        int numHist=iHistos_.size();
        if ( numHist==0 ) { isFlushed_=true; return *this; }
        bool close=false;
        if ( input_==NULL ) { input_=openin(dataFileName_); close=true; }
        if ( input_==NULL )
        {
            warning::print("File was not opened successfully!", "hist::read_many& hist::read_many::flush()");
            isFlushed_=true;
            return *this;
        }
        std::string linebuff, buff;
        std::istringstream iss;

        if ( ! std::getline(*input_, linebuff) )
        {
            warning::print("Could not read histogram!", "hist::read_many& hist::read_many::flush()");
            if ( close==true ) { delete input_; input_=NULL; }
            isFlushed_=true;
            return *this;
        }
        iss.str(linebuff);
        int dim;
        if ( !(iss>>buff>>buff>>dim) )
        {
            warning::print("Could not read histogram!", "hist::read_many& hist::read_many::flush()");
            if ( close==true ) { delete input_; input_=NULL; }
            isFlushed_=true;
            return *this;
        }
        iss.str("");
        iss.clear();

        std::vector<double> dbegin(dim);
        std::vector<double> dend(dim);
        std::vector<int> dbins(dim);
        for ( int j=0 ; j<dim ; ++j )
        {
            if ( ! std::getline(*input_, linebuff) )
            {
                warning::print("Could not read histogram!", "hist::read_many& hist::read_many::flush()");
                if ( close==true ) { delete input_; input_=NULL; }
                isFlushed_=true;
                return *this;
            }
            iss.str(linebuff);
            if ( !(iss>>buff>>buff>>dbegin[j]) )
            {
                warning::print("Could not read histogram!", "hist::read_many& hist::read_many::flush()");
                if ( close==true ) { delete input_; input_=NULL; }
                isFlushed_=true;
                return *this;
            }
            iss.str("");
            iss.clear();
            if ( ! std::getline(*input_, linebuff) )
            {
                warning::print("Could not read histogram!", "hist::read_many& hist::read_many::flush()");
                if ( close==true ) { delete input_; input_=NULL; }
                isFlushed_=true;
                return *this;
            }
            iss.str(linebuff);
            if ( !(iss>>buff>>buff>>dend[j]) )
            {
                warning::print("Could not read histogram!", "hist::read_many& hist::read_many::flush()");
                if ( close==true ) { delete input_; input_=NULL; }
                isFlushed_=true;
                return *this;
            }
            iss.str("");
            iss.clear();
            if ( ! std::getline(*input_, linebuff) )
            {
                warning::print("Could not read histogram!", "hist::read_many& hist::read_many::flush()");
                if ( close==true ) { delete input_; input_=NULL; }
                isFlushed_=true;
                return *this;
            }
            iss.str(linebuff);
            if ( !(iss>>buff>>buff>>dbins[j]) )
            {
                warning::print("Could not read histogram!", "hist::read_many& hist::read_many::flush()");
                if ( close==true ) { delete input_; input_=NULL; }
                isFlushed_=true;
                return *this;
            }
            iss.str("");
            iss.clear();
        }

        if ( ! std::getline(*input_, linebuff) )
        {
            warning::print("Could not read histogram!", "hist::read_many& hist::read_many::flush()");
            if ( close==true ) { delete input_; input_=NULL; }
            isFlushed_=true;
            return *this;
        }
        int beginmarker=0;
        int endmarker=0;
        int nHist=0;
        std::vector<var> htitle;
        for ( unsigned int l=0 ; l<linebuff.length() ; ++l )
        {
            if ( linebuff[l]=='`' )
            {
                if ( endmarker==beginmarker ) { beginmarker=l; endmarker=l+1; }
                else { endmarker=l; htitle.push_back(linebuff.substr(beginmarker+1, endmarker-beginmarker-1)); ++nHist; endmarker=beginmarker; }
            }
        }

        if ( ! std::getline(*input_, linebuff) )
        {
            warning::print("Could not read histogram!", "hist::read_many& hist::read_many::flush()");
            if ( close==true ) { delete input_; input_=NULL; }
            isFlushed_=true;
            return *this;
        }
        beginmarker=0;
        endmarker=0;
        int nHist1=0;
        std::vector<var> dtitle;
        std::vector<var> rtitle;
        for ( unsigned int l=0 ; l<linebuff.length() ; ++l )
        {
            if ( linebuff[l]=='`' )
            {
                if ( endmarker==beginmarker ) { beginmarker=l; endmarker=l+1; }
                else
                {
                    endmarker=l;
                    if ( nHist1<dim ) dtitle.push_back(linebuff.substr(beginmarker+1, endmarker-beginmarker-1));
                    else rtitle.push_back(linebuff.substr(beginmarker+1, endmarker-beginmarker-1));
                    ++nHist1;
                    endmarker=beginmarker;
                }
            }
        }
        if ( nHist1-dim!=nHist )
        {
            warning::print("Could not read histogram!", "hist::read_many& hist::read_many::flush()");
            if ( close==true ) { delete input_; input_=NULL; }
            isFlushed_=true;
            return *this;
        }
        int NHist=(nHist<numHist ? nHist : numHist);
        if ( NHist<=0 )
        {
            warning::print("No histograms in file which can be read!", "hist::read_many& hist::read_many::flush()");
            if ( close==true ) { delete input_; input_=NULL; }
            isFlushed_=true;
            return *this;
        }

        int iok=-1;
        for ( int k=0 ; k<numHist ; ++k )
        {
            if ( iHistos_[k].second<1 )
            {
                warning::print("Histogram index has to be greater than zero!", "hist::read_many& hist::read_many::flush()");
                continue;
            }
            if ( iHistos_[k].second>nHist )
            {
                warning::print("There are not that many histograms in the file as was requested!", "hist::read_many& hist::read_many::flush()");
                continue;
            }
            iHistos_[k].first->reinit_(dim, dbegin, dend, dbins, dtitle, rtitle[iHistos_[k].second-1], htitle[iHistos_[k].second-1]);
            iHistos_[k].first->sequence_.clear();
            iHistos_[k].first->invsequence_.clear();
            if ( iok==-1 ) iok=k;
        }
        if ( iok==-1 )
        {
            warning::print("Specified histograms do not exist in file!", "hist::read_many& hist::read_many::flush()");
            if ( close==true ) { delete input_; input_=NULL; }
            isFlushed_=true;
            return *this;
        }

        meas pos;
        std::vector<int> indprime(dim);
        std::vector<int> m(dim);
        meas val;
        int i=0, iprime=0, iprimemax=0, iprimeprime=0;
        int j=0, jprime=0;
        int k=0;
        bool skip=false;
        while ( getline(*input_, linebuff) )
        {
            if ( linebuff.length()==0 ) continue;
            if ( linebuff=="# end" ) break;
            if ( linebuff[0]=='#' ) continue;
            iss.str(linebuff);
            i=0;
            for ( j=0 ; j<dim ; ++j )
            {
                if ( !(iss>>pos) )
                {
                    warning::print("Could not read histogram! Clearing histograms.", "hist::read_many& hist::read_many::flush()");
                    for ( int kprime=0 ; kprime<numHist ; ++kprime )
                    {
                        if ( iHistos_[kprime].second<1 ) continue;
                        if ( iHistos_[kprime].second>nHist ) continue;
                        iHistos_[kprime].first->reinit_(0, std::vector<double>(0), std::vector<double>(0), std::vector<int>(0), std::vector<var>(0), (var)"", (var)"");
                    }
                    if ( close==true ) { delete input_; input_=NULL; }
                    isFlushed_=true;
                    return *this;
                }
                iHistos_[iok].first->ind_[j]=(int)((pos.v-pos.e+0.5*iHistos_[iok].first->binsize_[j]-dbegin[j])/iHistos_[iok].first->binsize_[j]);
                m[j]=(int)(pos.e/iHistos_[iok].first->binsize_[j]*2.0*(1.0+0.001/dbins[j]));
                i+=iHistos_[iok].first->mul_[j]*iHistos_[iok].first->ind_[j];
            }
            for ( k=0 ; k<nHist ; ++k )
            {
                if ( !(iss>>val) )
                {
                    warning::print("Could not read histogram! Clearing histograms.", "hist::read_many& hist::read_many::flush()");
                    for ( int kprime=0 ; kprime<numHist ; ++kprime )
                    {
                        if ( iHistos_[kprime].second<1 ) continue;
                        if ( iHistos_[kprime].second>nHist ) continue;
                        iHistos_[kprime].first->reinit_(0, std::vector<double>(0), std::vector<double>(0), std::vector<int>(0), std::vector<var>(0), (var)"", (var)"");
                    }
                    if ( close==true ) { delete input_; input_=NULL; }
                    isFlushed_=true;
                    return *this;
                }
                for ( int kprime=0 ; kprime<numHist ; ++kprime )
                {
                    if ( iHistos_[kprime].second<1 ) continue;
                    if ( iHistos_[kprime].second>nHist ) continue;
                    if ( iHistos_[kprime].second-1==k ) iHistos_[kprime].first->h_[i]=val;
                }
            }
            iprimemax=1;
            for ( j=0 ; j<dim ; ++j ) iprimemax*=m[j];
            for ( iprime=0 ; iprime<iprimemax ; ++iprime )
            {
                iprimeprime=0;
                indprime[dim-1]=iprime;
                for ( j=dim-1 ; j>=0 ; --j )
                {
                    if ( j<dim-1 ) indprime[j]=indprime[j+1]/m[j+1];
                    iprimeprime+=iHistos_[iok].first->mul_[j]*(iHistos_[iok].first->ind_[j]+indprime[j]%m[j]);
                }
                for ( j=0 ; j<dim ; ++j )
                {
                    indprime[j]=indprime[j]%m[j];
                    if ( indprime[j]==0 )
                    {
                        for ( k=0 ; k<numHist ; ++k )
                        {
                            if ( iHistos_[k].second<1 ) continue;
                            if ( iHistos_[k].second>nHist ) continue;
                            iHistos_[k].first->m_[iprimeprime*dim+j]=m[j];
                        }
                    }
                    else
                    {
                        for ( k=0 ; k<numHist ; ++k )
                        {
                            if ( iHistos_[k].second<1 ) continue;
                            if ( iHistos_[k].second>nHist ) continue;
                            iHistos_[k].first->m_[iprimeprime*dim+j]=0;
                        }
                    }
                }
            }
            iss.str("");
            iss.clear();
        }
        if ( close==true ) { delete input_; input_=NULL; close=false; }
        int hsize=iHistos_[iok].first->h_.size();
        iprimeprime=0;
        for ( i=0 ; i<hsize ; ++i )
        {
            skip=false;
            iHistos_[iok].first->ind_[dim-1]=i;
            for ( j=dim-1 ; j>=0 ; --j )
            {
                if ( iHistos_[iok].first->m_[i*dim+j]==0 ) skip=true;
                if ( j<dim-1 ) iHistos_[iok].first->ind_[j]=iHistos_[iok].first->ind_[j+1]/iHistos_[iok].first->bins_[j+1];
            }
            if ( skip==false )
            {
                for ( k=0 ; k<numHist ; ++k )
                {
                    if ( iHistos_[k].second<1 ) continue;
                    if ( iHistos_[k].second>nHist ) continue;
                    iHistos_[k].first->sequence_.push_back(i);
                    iHistos_[k].first->invsequence_.push_back(iprimeprime);
                }
                ++iprimeprime;
                continue;
            }
            for ( j=0 ; j<dim ; ++j )
            {
                iHistos_[iok].first->ind_[j]=iHistos_[iok].first->ind_[j]%iHistos_[iok].first->bins_[j];
            }
            for ( j=0 ; j<dim ; ++j )
            {
                while ( iHistos_[iok].first->ind_[j]>=0 )
                {
                    iprime=0;
                    for ( jprime=0 ; jprime<dim ; ++jprime ) iprime+=iHistos_[iok].first->ind_[jprime]*iHistos_[iok].first->mul_[jprime];
                    if ( iHistos_[iok].first->m_[iprime*dim+j]==0 ) { --(iHistos_[iok].first->ind_[j]); } else { break; }
                }
            }
            if ( (unsigned int)(iprime)>=iHistos_[iok].first->invsequence_.size() )
            {
                warning::print("Index inconsistency when reading histogram! Clearing histograms.", "hist::read_many& hist::read_many::flush()");
                for ( k=0 ; k<numHist ; ++k )
                {
                    if ( iHistos_[k].second<1 ) continue;
                    if ( iHistos_[k].second>nHist ) continue;
                    iHistos_[k].first->reinit_(0, std::vector<double>(0), std::vector<double>(0), std::vector<int>(0), std::vector<var>(0), (var)"", (var)"");
                }
                isFlushed_=true;
                return *this;
            }
            for ( k=0 ; k<numHist ; ++k )
            {
                if ( iHistos_[k].second<1 ) continue;
                if ( iHistos_[k].second>nHist ) continue;
                iHistos_[k].first->invsequence_.push_back(iHistos_[iok].first->invsequence_[iprime]);
            }
        }
        isFlushed_=true;
        return *this;
    }


    hist::read_many::~read_many()
    {
        if ( isFlushed_==false ) flush();
    }


    int hist::read_many::dim(istream &input)
    {
        std::string linebuff, buff;
        std::istringstream iss;
        if ( ! std::getline(input, linebuff) )
        {
            warning::print("Could not read histogram dimension!", "unsigned int hist::read_many::dim(const var&)");
            return 0;
        }
        iss.str(linebuff);
        int d;
        if ( !(iss>>buff>>buff>>d) )
        {
            warning::print("Could not read histogram dimension!", "unsigned int hist::read_many::dim(const var&)");
            return 0;
        }
        iss.str("");
        iss.clear();
        return d;
    }


    int hist::read_many::dim(const var &datafilename)
    {
        int d;
        istream *input=openin(datafilename);
        d=dim(*input);
        delete input;
        return d;
    }


    int hist::read_many::numhists(istream &input)
    {
        std::string linebuff, buff;
        std::istringstream iss;
        if ( ! std::getline(input, linebuff) )
        {
            warning::print("Could not read number of histograms!", "unsigned int hist::read_many::numhists(const var&)");
            return 0;
        }
        iss.str(linebuff);
        int d;
        if ( !(iss>>buff>>buff>>d) )
        {
            warning::print("Could not read number of histograms!", "unsigned int hist::read_many::numhists(const var&)");
            return 0;
        }
        iss.str("");
        iss.clear();
        double dbegindend;
        int dbins;
        for ( int j=0 ; j<d ; ++j )
        {
            if ( ! std::getline(input, linebuff) )
            {
                warning::print("Could not read number of histograms!", "unsigned int hist::read_many::numhists(const var&)");
                return 0;
            }
            iss.str(linebuff);
            if ( !(iss>>buff>>buff>>dbegindend) )
            {
                warning::print("Could not read number of histograms!", "unsigned int hist::read_many::numhists(const var&)");
                return 0;
            }
            iss.str("");
            iss.clear();
            if ( ! std::getline(input, linebuff) )
            {
                warning::print("Could not read number of histograms!", "unsigned int hist::read_many::numhists(const var&)");
                return 0;
            }
            iss.str(linebuff);
            if ( !(iss>>buff>>buff>>dbegindend) )
            {
                warning::print("Could not read number of histograms!", "unsigned int hist::read_many::numhists(const var&)");
                return 0;
            }
            iss.str("");
            iss.clear();
            if ( ! std::getline(input, linebuff) )
            {
                warning::print("Could not read number of histograms!", "unsigned int hist::read_many::numhists(const var&)");
                return 0;
            }
            iss.str(linebuff);
            if ( !(iss>>buff>>buff>>dbins) )
            {
                warning::print("Could not read number of histograms!", "unsigned int hist::read_many::numhists(const var&)");
                return 0;
            }
            iss.str("");
            iss.clear();
        }
        if ( ! std::getline(input, linebuff) )
        {
            warning::print("Could not read number of histograms!", "unsigned int hist::read_many::numhists(const var&)");
            return 0;
        }
        int beginmarker=0;
        int endmarker=0;
        int nHist=0;
        for ( unsigned int l=0 ; l<linebuff.length() ; ++l )
        {
            if ( linebuff[l]=='`' )
            {
                if ( endmarker==beginmarker ) { beginmarker=l; endmarker=l+1; }
                else { endmarker=l; ++nHist; endmarker=beginmarker; }
            }
        }
        return nHist;
    }


    int hist::read_many::numhists(const var &datafilename)
    {
        int nhists;
        istream *input=openin(datafilename);
        nhists=numhists(*input);
        delete input;
        return nhists;
    }


    hist& hist::read(istream &input, int number_of_histo)
    {
        read_many(input)>>(*this)>>number_of_histo;
        return *this;
    }


    hist& hist::read(const var &datafilename, int number_of_histo)
    {
        read_many(datafilename)>>(*this)>>number_of_histo;
        return *this;
    }


    function hist::x_hint() const
    {
        return (dim_>0 ? ARG(1) : function(unset));
    }


    function hist::dx_hint() const
    {
        return (dim_>0 ? ARG(dim_+2) : function(unset));
    }


    function hist::x1_hint() const
    {
        return (dim_>0 ? ARG(1)-ARG(dim_+2)/2 : function(unset)); // added the /2 D.B.
    }


    function hist::x2_hint() const
    {
        return (dim_>0 ? ARG(1)+ARG(dim_+2)/2 : function(unset)); // added the /2 D.B.
    }


    function hist::y_hint() const
    {
        return (dim_>0 ? ARG(2) : function(unset));
    }


    function hist::dy_hint() const
    {
        return (dim_>0 ? ARG(dim_+3) : function(unset));
    }


    function hist::y1_hint() const
    {
        return (dim_>0 ? ARG(2)-ARG(dim_+3)/2 : function(unset)); // added the /2 D.B.
    }


    function hist::y2_hint() const
    {
        return (dim_>0 ? ARG(2)+ARG(dim_+3)/2 : function(unset)); // added the /2 D.B.
    }


    function hist::z_hint() const
    {
        return (dim_>1 ? ARG(3) : function(unset));
    }


    function hist::dz_hint() const
    {
        return (dim_>1 ? ARG(dim_+4) : function(unset));
    }


    function hist::z1_hint() const
    {
        return (dim_>1 ? ARG(3)-ARG(dim_+4)/2 : function(unset)); // added the /2 D.B.
    }


    function hist::z2_hint() const
    {
        return (dim_>1 ? ARG(3)+ARG(dim_+4)/2 : function(unset)); // added the /2 D.B.
    }




    const datapoint* hist::get(int i) const
    {
        if(!buffer_.empty()) flush_buffer();
        bool skip;
        int iprime=sequence_[i];
        int j;
        skip=false;
        ind_[dim_-1]=iprime;
        for ( j=dim_-1 ; j>=0 ; --j )
        {
            if ( m_[iprime*dim_+j]==0 ) { skip=true; break; }
            if ( j<dim_-1 ) ind_[j]=ind_[j+1]/bins_[j+1];
        }
        if ( skip==true )
        {
            warning::print("Index inconsistency!", "datapoint* hist::get(const int) const");
            return &datapoint_;
        }
        for ( j=0 ; j<dim_ ; ++j )
        {
            ind_[j]=ind_[j]%bins_[j];
            datapoint_[j]=min_[j]+(ind_[j]+0.5*m_[iprime*dim_+j])*binsize_[j];
            datapoint_[dim_+1+j]=binsize_[j]*m_[iprime*dim_+j]; // removed a 0.5 factor! D.B.
        }
        datapoint_[dim_]=h_[iprime].v;
        datapoint_[2*dim_+1]=h_[iprime].e;
        return &datapoint_;
    }


    hist::size_type hist::size() const
    {
        return sequence_.size();
    }


    double hist::max(int j) const
    {
        if ( j==0 )
        {
            warning::print("Index out of range! (1-based indices!!!)", "double hist::max(const int) const");
            return unset;
        }
        else if ( j<=dim_ ) return max_[j-1];
        else if ( j==dim_+1 )
        {
            double v=0.0, vmax=unset;
            bool firstflag=true;
            unsigned int i, iprime;
            for ( i=0 ; i<sequence_.size() ; ++i )
            {
                iprime=sequence_[i];
                if ( std::isfinite(h_[iprime].v)==0 ) continue;
                if ( h_[iprime].v==unset ) continue;
                v=h_[iprime].v;
                if ( firstflag==true ) { vmax=v; firstflag=false; }
                if ( v>vmax ) vmax=v;
            }
            return vmax;
        }
        else if ( j<=dim_*2+1 )
        {
            double d=0.0, dmax=0.0;
            unsigned int i=0, iprime=0;
            for ( i=0 ; i<sequence_.size() ; ++i )
            {
                iprime=sequence_[i];
                d=0.5*binsize_[j-dim_-2]*m_[iprime*dim_+(j-dim_-2)];
                if ( i==0 ) dmax=d;
                if ( d>dmax ) dmax=d;
            }
            return dmax;
        }
        else if ( j==dim_*2+2 )
        {
            double e=0.0, emax=unset;
            bool firstflag=true;
            unsigned int i=0, iprime=0;
            for ( i=0 ; i<sequence_.size() ; ++i )
            {
                iprime=sequence_[i];
                if ( std::isfinite(h_[iprime].e)==0 ) continue;
                if ( h_[iprime].e==unset ) continue;
                e=h_[iprime].e;
                if ( firstflag==true ) { emax=e; firstflag=false; }
                if ( e>emax ) emax=e;
            }
            return emax;
        }
        else 
        {
            warning::print("Index out of range! (1-based indices!!!)", "double hist::max(const int) const");
        }
        return unset;
    }


    var hist::max(const function &f, function fret) const
    {
        const bool return_different = fret.initialized();
        if(!fret.initialized()) fret = f;
        int j=0;
        for ( int jtmp=1 ; jtmp<=dim_ ; ++jtmp )
        {
            if ( f.equals(ARG(jtmp)) ) { j=jtmp; break; }
        }
        if ( j==0 )
        {
            double max_val=unset;
            var    max_pos="";
            for ( unsigned int i=0 ; i<sequence_.size() ; ++i )
            {
                const double v=f.eval_dbl(*get(i));
                if ( i==0 || v>max_val )
                {
                    max_val = v;
                    max_pos = fret.eval(*get(i));
                }
            }
            return max_pos;
        }
        else if ( j<=dim_ )
        {
            if(return_different) warning::print("Should update the code for returning the value of a different expression at the minimum","hist::max(const function &f, function)");
            return max_[j-1];
        }
        else if ( j==dim_+1 )
        {
            if(return_different) warning::print("Should update the code for returning the value of a different expression at the minimum","hist::max(const function &f, function)");
            double v=0.0, vmax=unset;
            bool firstflag=true;
            unsigned int i, iprime;
            for ( i=0 ; i<sequence_.size() ; ++i )
            {
                iprime=sequence_[i];
                if ( std::isfinite(h_[iprime].v)==0 ) continue;
                if ( h_[iprime].v==unset ) continue;
                v=h_[iprime].v;
                if ( firstflag==true ) { vmax=v; firstflag=false; }
                if ( v>vmax ) vmax=v;
            }
            return vmax;
        }
        else if ( j<=dim_*2+1 )
        {
            if(return_different) warning::print("Should update the code for returning the value of a different expression at the minimum","hist::max(const function &f, function)");
            double d=0.0, dmax=0.0;
            unsigned int i=0, iprime=0;
            for ( i=0 ; i<sequence_.size() ; ++i )
            {
                iprime=sequence_[i];
                d=0.5*binsize_[j-dim_-2]*m_[iprime*dim_+(j-dim_-2)];
                if ( i==0 ) dmax=d;
                if ( d>dmax ) dmax=d;
            }
            return dmax;
        }
        else if ( j==dim_*2+2 )
        {
            if(return_different) warning::print("Should update the code for returning the value of a different expression at the minimum","hist::max(const function &f, function)");
            double e=0.0, emax=unset;
            bool firstflag=true;
            unsigned int i=0, iprime=0;
            for ( i=0 ; i<sequence_.size() ; ++i )
            {
                iprime=sequence_[i];
                if ( std::isfinite(h_[iprime].e)==0 ) continue;
                if ( h_[iprime].e==unset ) continue;
                e=h_[iprime].e;
                if ( firstflag==true ) { emax=e; firstflag=false; }
                if ( e>emax ) emax=e;
            }
            return emax;
        }
        else 
        {
            warning::print("Index out of range! (1-based indices!!!)", "double hist::max(const int) const");
        }
        return unset;
    }


    double hist::min(int j) const
    {
        if ( j==0 )
        {
            warning::print("Index out of range! (1-based indices!!!)", "double hist::min(const int) const");
            return unset;
        }
        else if ( j<=dim_ ) return min_[j-1];
        else if ( j==dim_+1 )
        {
            double v=0.0, vmin=unset;
            bool firstflag=true;
            unsigned int i=0, iprime=0;
            for ( i=0 ; i<sequence_.size() ; ++i )
            {
                iprime=sequence_[i];
                if ( std::isfinite(h_[iprime].v)==0 ) continue;
                if ( h_[iprime].v==unset ) continue;
                v=h_[iprime].v;
                if ( firstflag==true ) { vmin=v; firstflag=false; }
                if ( vmin<v ) vmin=v;
            }
            return vmin;
        }
        else if ( j<=dim_*2+1 )
        {
            double d=0.0, dmin=0.0;
            unsigned int i=0, iprime=0;
            for ( i=0 ; i<sequence_.size() ; ++i )
            {
                iprime=sequence_[i];
                d=0.5*binsize_[j-dim_-2]*m_[iprime*dim_+(j-dim_-2)];
                if ( i==0 ) dmin=d;
                if ( d>dmin ) dmin=d;
            }
            return dmin;
        }
        else if ( j==dim_*2+2 )
        {
            double e=0.0, emin=unset;
            bool firstflag=true;
            unsigned int i=0, iprime=0;
            for ( i=0 ; i<sequence_.size() ; ++i )
            {
                iprime=sequence_[i];
                if ( std::isfinite(h_[iprime].e)==0 ) continue;
                if ( h_[iprime].e==unset ) continue;
                e=h_[iprime].e;
                if ( firstflag==true ) { emin=e; firstflag=false; }
                if ( e>emin ) emin=e;
            }
            return emin;
        }
        else
        {
            warning::print("Index out of range! (1-based indices!!!)", "double hist::min(const int) const");
        }
        return unset;
    }


    var hist::min(const function &f, function fret) const
    {
        const bool return_different = fret.initialized();
        if(!fret.initialized()) fret = f;
        int j=0;
        for ( int jtmp=1; jtmp<=dim_ ; ++jtmp )
        {
            if ( f.equals(ARG(jtmp)) ) { j=jtmp; break; }
        }
        if ( j==0 )
        {
            double min_val=unset;
            var    min_pos="";
            for (unsigned int i=0 ; i<sequence_.size() ; ++i )
            {
                const double v=f.eval_dbl(*get(i));
                if ( i==0 || v<min_val )
                {
                    min_val = v;
                    min_pos = fret.eval(*get(i));
                }
            }
            return min_pos;
        }
        else if ( j<=dim_ )
        {
            if(return_different) warning::print("Should update the code for returning the value of a different expression at the minimum","hist::min(const function &f, function)");
            return min_[j-1];
        }
        else if ( j==dim_+1 )
        {
            if(return_different) warning::print("Should update the code for returning the value of a different expression at the minimum","hist::min(const function &f, function)");
            double v=0.0, vmin=unset;
            bool firstflag=true;
            unsigned int i=0, iprime=0;
            for ( i=0 ; i<sequence_.size() ; ++i )
            {
                iprime=sequence_[i];
                if ( std::isfinite(h_[iprime].v)==0 ) continue;
                if ( h_[iprime].v==unset ) continue;
                v=h_[iprime].v;
                if ( firstflag==true ) { vmin=v; firstflag=false; }
                if ( vmin<v ) vmin = v;
            }
            return vmin;
        }
        else if ( j<=dim_*2+1 )
        {
            if(return_different) warning::print("Should update the code for returning the value of a different expression at the minimum","hist::min(const function &f, function)");
            double d=0.0, dmin=0.0;
            unsigned int i=0, iprime=0;
            for ( i=0 ; i<sequence_.size() ; ++i )
            {
                iprime=sequence_[i];
                d=0.5*binsize_[j-dim_-2]*m_[iprime*dim_+(j-dim_-2)];
                if ( i==0 ) dmin=d;
                if ( d>dmin ) dmin=d;
            }
            return dmin;
        }
        else if ( j==dim_*2+2 )
        {
            if(return_different) warning::print("Should update the code for returning the value of a different expression at the minimum","hist::min(const function &f, function)");
            double e=0.0, emin=unset;
            bool firstflag=true;
            unsigned int i=0, iprime=0;
            for ( i=0 ; i<sequence_.size() ; ++i )
            {
                iprime=sequence_[i];
                if ( std::isfinite(h_[iprime].e)==0 ) continue;
                if ( h_[iprime].e==unset ) continue;
                e=h_[iprime].e;
                if ( firstflag==true ) { emin=e; firstflag=false; }
                if ( e>emin ) emin=e;
            }
            return emin;
        }
        else
        {
            warning::print("Index out of range! (1-based indices!!!)", "double hist::min(const int) const");
        }
        return unset;
    }


    int hist::columns() const
    {
        return dim_*2+2;
    }

    void hist::prepare_for_draw(axis *,axis *, frame *f, int count)
    {
        flush_buffer();
        if ( dim_<1 || dim_>2 ) warning::print("Only can draw 1 or 2 dimensional histograms!", "void hist::prepare_for_draw(axis*, axis*, frame*)");
        if ( graph_drawer_ ) graph_drawer_->prepare_for_draw(this, f, count);
    }


    dgraph hist::to_dgraph() const
    {
        dgraph result;
        for ( unsigned int i=0 ; i<size() ; ++i ) result.add(*get(i));
        result.copy_style(*this);
        return result;
    }


    hist::qplot::qplot(mpps &bm)
        : blopMpps_(&bm), globaltitle_("__NULL"), rangemin_(false, 0.0), rangemax_(false, 0.0), 
          isomin_(false, 0.0), isomax_(false, 0.0), 
          rangelog_(false, false), isolog_(false, false), doubleplot_(false), isFlushed_(false)
    {
    }


    hist::qplot::qplot(const var &blopepsname)
        : blopMpps_(NULL), blopEpsName_(blopepsname), globaltitle_("__NULL"), rangemin_(false, 0.0), rangemax_(false, 0.0), 
          isomin_(false, 0.0), isomax_(false, 0.0), 
          rangelog_(false, false), isolog_(false, false), doubleplot_(false), isFlushed_(false)
    {
    }


    hist::qplot::qplot()
        : blopMpps_(NULL), globaltitle_("__NULL"), rangemin_(false, 0.0), rangemax_(false, 0.0), 
          isomin_(false, 0.0), isomax_(false, 0.0), 
          rangelog_(false, false), isolog_(false, false), doubleplot_(false), isFlushed_(false)
    {
    }


    hist::qplot::qplot(mpps &bm, hist &h1, hist &h2)
        : blopMpps_(&bm), globaltitle_("__NULL"), rangemin_(false, 0.0), rangemax_(false, 0.0), 
          isomin_(false, 0.0), isomax_(false, 0.0), 
          rangelog_(false, false), isolog_(false, false), doubleplot_(true), isFlushed_(false)
    {
        oStuff_.push_back(oStuff_t(&h1, unset, "__NULL"));
        oStuff_.push_back(oStuff_t(&h2, unset, "__NULL"));
    }


    hist::qplot::qplot(const var &blopepsname, hist &h1, hist &h2)
        : blopMpps_(NULL), blopEpsName_(blopepsname), globaltitle_("__NULL"), rangemin_(false, 0.0), rangemax_(false, 0.0), 
          isomin_(false, 0.0), isomax_(false, 0.0), 
          rangelog_(false, false), isolog_(false, false), doubleplot_(true), isFlushed_(false)
    {
        oStuff_.push_back(oStuff_t(&h1, unset, "__NULL"));
        oStuff_.push_back(oStuff_t(&h2, unset, "__NULL"));
    }


    hist::qplot::qplot(hist &h1, hist &h2)
        : blopMpps_(NULL), globaltitle_("__NULL"), rangemin_(false, 0.0), rangemax_(false, 0.0), 
          isomin_(false, 0.0), isomax_(false, 0.0), 
          rangelog_(false, false), isolog_(false, false), doubleplot_(true), isFlushed_(false)
    {
        oStuff_.push_back(oStuff_t(&h1, unset, "__NULL"));
        oStuff_.push_back(oStuff_t(&h2, unset, "__NULL"));
    }


    hist::qplot::qplot(mpps &bm, hist &h, const function &f, const var &legend)
        : blopMpps_(&bm), globaltitle_("__NULL"), rangemin_(false, 0.0), rangemax_(false, 0.0), 
          isomin_(false, 0.0), isomax_(false, 0.0), 
          rangelog_(false, false), isolog_(false, false), doubleplot_(true), isFlushed_(false)
    {
        oStuff_.push_back(oStuff_t(&h, unset, "__NULL"));
        oStuff_.push_back(oStuff_t(NULL, f, legend));
    }


    hist::qplot::qplot(const var &blopepsname, hist &h, const function &f, const var &legend)
        : blopMpps_(NULL), blopEpsName_(blopepsname), globaltitle_("__NULL"), rangemin_(false, 0.0), rangemax_(false, 0.0), 
          isomin_(false, 0.0), isomax_(false, 0.0), 
          rangelog_(false, false), isolog_(false, false), doubleplot_(true), isFlushed_(false)
    {
        oStuff_.push_back(oStuff_t(&h, unset, "__NULL"));
        oStuff_.push_back(oStuff_t(NULL, f, legend));
    }


    hist::qplot::qplot(hist &h, const function &f, const var &legend)
        : blopMpps_(NULL), globaltitle_("__NULL"), rangemin_(false, 0.0), rangemax_(false, 0.0), 
          isomin_(false, 0.0), isomax_(false, 0.0), 
          rangelog_(false, false), isolog_(false, false), doubleplot_(true), isFlushed_(false)
    {
        oStuff_.push_back(oStuff_t(&h, unset, "__NULL"));
        oStuff_.push_back(oStuff_t(NULL, f, legend));
    }


    hist::qplot::qplot(mpps &bm, const function &f1, const function &f2, const var &legend1, const var &legend2)
        : blopMpps_(&bm), globaltitle_("__NULL"), rangemin_(false, 0.0), rangemax_(false, 0.0), 
          isomin_(false, 0.0), isomax_(false, 0.0), 
          rangelog_(false, false), isolog_(false, false), doubleplot_(true), isFlushed_(false)
    {
        oStuff_.push_back(oStuff_t(NULL, f1, legend1));
        oStuff_.push_back(oStuff_t(NULL, f2, legend2));
    }


    hist::qplot::qplot(const var &blopepsname, const function &f1, const function &f2, const var &legend1, const var &legend2)
        : blopMpps_(NULL), blopEpsName_(blopepsname), globaltitle_("__NULL"), rangemin_(false, 0.0), rangemax_(false, 0.0), 
          isomin_(false, 0.0), isomax_(false, 0.0), 
          rangelog_(false, false), isolog_(false, false), doubleplot_(true), isFlushed_(false)
    {
        oStuff_.push_back(oStuff_t(NULL, f1, legend1));
        oStuff_.push_back(oStuff_t(NULL, f2, legend2));
    }


    hist::qplot::qplot(const function &f1, const function &f2, const var &legend1, const var &legend2)
        : blopMpps_(NULL), globaltitle_("__NULL"), rangemin_(false, 0.0), rangemax_(false, 0.0), 
          isomin_(false, 0.0), isomax_(false, 0.0), 
          rangelog_(false, false), isolog_(false, false), doubleplot_(true), isFlushed_(false)
    {
        oStuff_.push_back(oStuff_t(NULL, f1, legend1));
        oStuff_.push_back(oStuff_t(NULL, f2, legend2));
    }


    hist::qplot& hist::qplot::reinit(mpps &bm)
    {
        blopMpps_=&bm;
        blopEpsName_="";
        globaltitle_="__NULL";
        min_.clear();
        max_.clear();
        rangemin_.first=false; rangemin_.second=0.0;
        rangemax_.first=false; rangemax_.second=0.0;
        isomin_.first=false; isomin_.second=0.0;
        isomax_.first=false; isomax_.second=0.0;
        log_.clear();
        rangelog_.first=false; rangelog_.second=false;
        isolog_.first=false; isolog_.second=false;
        oStuff_.clear();
        doubleplot_=false;
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::reinit(const var &blopepsname)
    {
        blopMpps_=NULL;
        blopEpsName_=blopepsname;
        globaltitle_="__NULL";
        min_.clear();
        max_.clear();
        rangemin_.first=false; rangemin_.second=0.0;
        rangemax_.first=false; rangemax_.second=0.0;
        isomin_.first=false; isomin_.second=0.0;
        isomax_.first=false; isomax_.second=0.0;
        log_.clear();
        rangelog_.first=false; rangelog_.second=false;
        isolog_.first=false; isolog_.second=false;
        oStuff_.clear();
        doubleplot_=false;
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::reinit()
    {
        blopMpps_=NULL;
        blopEpsName_="";
        globaltitle_="__NULL";
        min_.clear();
        max_.clear();
        rangemin_.first=false; rangemin_.second=0.0;
        rangemax_.first=false; rangemax_.second=0.0;
        isomin_.first=false; isomin_.second=0.0;
        isomax_.first=false; isomax_.second=0.0;
        log_.clear();
        rangelog_.first=false; rangelog_.second=false;
        isolog_.first=false; isolog_.second=false;
        oStuff_.clear();
        doubleplot_=false;
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::reinit(mpps &bm, hist &h1, hist &h2)
    {
        blopMpps_=&bm;
        blopEpsName_="";
        globaltitle_="__NULL";
        min_.clear();
        max_.clear();
        rangemin_.first=false; rangemin_.second=0.0;
        rangemax_.first=false; rangemax_.second=0.0;
        isomin_.first=false; isomin_.second=0.0;
        isomax_.first=false; isomax_.second=0.0;
        log_.clear();
        rangelog_.first=false; rangelog_.second=false;
        isolog_.first=false; isolog_.second=false;
        oStuff_.clear();
        doubleplot_=true;
        isFlushed_=false;
        oStuff_.push_back(oStuff_t(&h1, unset, "__NULL"));
        oStuff_.push_back(oStuff_t(&h2, unset, "__NULL"));
        return *this;
    }


    hist::qplot& hist::qplot::reinit(const var &blopepsname, hist &h1, hist &h2)
    {
        blopMpps_=NULL;
        blopEpsName_=blopepsname;
        globaltitle_="__NULL";
        min_.clear();
        max_.clear();
        rangemin_.first=false; rangemin_.second=0.0;
        rangemax_.first=false; rangemax_.second=0.0;
        isomin_.first=false; isomin_.second=0.0;
        isomax_.first=false; isomax_.second=0.0;
        log_.clear();
        rangelog_.first=false; rangelog_.second=false;
        isolog_.first=false; isolog_.second=false;
        oStuff_.clear();
        doubleplot_=true;
        isFlushed_=false;
        oStuff_.push_back(oStuff_t(&h1, unset, "__NULL"));
        oStuff_.push_back(oStuff_t(&h2, unset, "__NULL"));
        return *this;
    }


    hist::qplot& hist::qplot::reinit(hist &h1, hist &h2)
    {
        blopMpps_=NULL;
        blopEpsName_="";
        globaltitle_="__NULL";
        min_.clear();
        max_.clear();
        rangemin_.first=false; rangemin_.second=0.0;
        rangemax_.first=false; rangemax_.second=0.0;
        isomin_.first=false; isomin_.second=0.0;
        isomax_.first=false; isomax_.second=0.0;
        log_.clear();
        rangelog_.first=false; rangelog_.second=false;
        isolog_.first=false; isolog_.second=false;
        oStuff_.clear();
        doubleplot_=true;
        isFlushed_=false;
        oStuff_.push_back(oStuff_t(&h1, unset, "__NULL"));
        oStuff_.push_back(oStuff_t(&h2, unset, "__NULL"));
        return *this;
    }


    hist::qplot& hist::qplot::reinit(mpps &bm, hist &h, const function &f, const var &legend)
    {
        blopMpps_=&bm;
        blopEpsName_="";
        globaltitle_="__NULL";
        min_.clear();
        max_.clear();
        rangemin_.first=false; rangemin_.second=0.0;
        rangemax_.first=false; rangemax_.second=0.0;
        isomin_.first=false; isomin_.second=0.0;
        isomax_.first=false; isomax_.second=0.0;
        log_.clear();
        rangelog_.first=false; rangelog_.second=false;
        isolog_.first=false; isolog_.second=false;
        oStuff_.clear();
        doubleplot_=true;
        isFlushed_=false;
        oStuff_.push_back(oStuff_t(&h, unset, "__NULL"));
        oStuff_.push_back(oStuff_t(NULL, f, legend));
        return *this;
    }


    hist::qplot& hist::qplot::reinit(const var &blopepsname, hist &h, const function &f, const var &legend)
    {
        blopMpps_=NULL;
        blopEpsName_=blopepsname;
        globaltitle_="__NULL";
        min_.clear();
        max_.clear();
        rangemin_.first=false; rangemin_.second=0.0;
        rangemax_.first=false; rangemax_.second=0.0;
        isomin_.first=false; isomin_.second=0.0;
        isomax_.first=false; isomax_.second=0.0;
        log_.clear();
        rangelog_.first=false; rangelog_.second=false;
        isolog_.first=false; isolog_.second=false;
        oStuff_.clear();
        doubleplot_=true;
        isFlushed_=false;
        oStuff_.push_back(oStuff_t(&h, unset, "__NULL"));
        oStuff_.push_back(oStuff_t(NULL, f, legend));
        return *this;
    }


    hist::qplot& hist::qplot::reinit(hist &h, const function &f, const var &legend)
    {
        blopMpps_=NULL;
        blopEpsName_="";
        globaltitle_="__NULL";
        min_.clear();
        max_.clear();
        rangemin_.first=false; rangemin_.second=0.0;
        rangemax_.first=false; rangemax_.second=0.0;
        isomin_.first=false; isomin_.second=0.0;
        isomax_.first=false; isomax_.second=0.0;
        log_.clear();
        rangelog_.first=false; rangelog_.second=false;
        isolog_.first=false; isolog_.second=false;
        oStuff_.clear();
        doubleplot_=true;
        isFlushed_=false;
        oStuff_.push_back(oStuff_t(&h, unset, "__NULL"));
        oStuff_.push_back(oStuff_t(NULL, f, legend));
        return *this;
    }


    hist::qplot& hist::qplot::reinit(mpps &bm, const function &f1, const function &f2, const var &legend1, const var &legend2)
    {
        blopMpps_=&bm;
        blopEpsName_="";
        globaltitle_="__NULL";
        min_.clear();
        max_.clear();
        rangemin_.first=false; rangemin_.second=0.0;
        rangemax_.first=false; rangemax_.second=0.0;
        isomin_.first=false; isomin_.second=0.0;
        isomax_.first=false; isomax_.second=0.0;
        log_.clear();
        rangelog_.first=false; rangelog_.second=false;
        isolog_.first=false; isolog_.second=false;
        oStuff_.clear();
        doubleplot_=true;
        isFlushed_=false;
        oStuff_.push_back(oStuff_t(NULL, f1, legend1));
        oStuff_.push_back(oStuff_t(NULL, f2, legend2));
        return *this;
    }


    hist::qplot& hist::qplot::reinit(const var &blopepsname, const function &f1, const function &f2, const var &legend1, const var &legend2)
    {
        blopMpps_=NULL;
        blopEpsName_=blopepsname;
        globaltitle_="__NULL";
        min_.clear();
        max_.clear();
        rangemin_.first=false; rangemin_.second=0.0;
        rangemax_.first=false; rangemax_.second=0.0;
        isomin_.first=false; isomin_.second=0.0;
        isomax_.first=false; isomax_.second=0.0;
        log_.clear();
        rangelog_.first=false; rangelog_.second=false;
        isolog_.first=false; isolog_.second=false;
        oStuff_.clear();
        doubleplot_=true;
        isFlushed_=false;
        oStuff_.push_back(oStuff_t(NULL, f1, legend1));
        oStuff_.push_back(oStuff_t(NULL, f2, legend2));
        return *this;
    }


    hist::qplot& hist::qplot::reinit(const function &f1, const function &f2, const var &legend1, const var &legend2)
    {
        blopMpps_=NULL;
        blopEpsName_="";
        globaltitle_="__NULL";
        min_.clear();
        max_.clear();
        rangemin_.first=false; rangemin_.second=0.0;
        rangemax_.first=false; rangemax_.second=0.0;
        isomin_.first=false; isomin_.second=0.0;
        isomax_.first=false; isomax_.second=0.0;
        log_.clear();
        rangelog_.first=false; rangelog_.second=false;
        isolog_.first=false; isolog_.second=false;
        oStuff_.clear();
        doubleplot_=true;
        isFlushed_=false;
        oStuff_.push_back(oStuff_t(NULL, f1, legend1));
        oStuff_.push_back(oStuff_t(NULL, f2, legend2));
        return *this;
    }


    hist::qplot& hist::qplot::title(const var &globaltitle)
    {
        globaltitle_=globaltitle;
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::min(const int axis, const double value)
    {
        min_.push_back(std::pair<int, double>(axis, value));
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::max(const int axis, const double value)
    {
        max_.push_back(std::pair<int, double>(axis, value));
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::rangemin(const double value)
    {
        rangemin_.first=true; rangemin_.second=value;
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::rangemax(const double value)
    {
        rangemax_.first=true; rangemax_.second=value;
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::isomin(const double value)
    {
        isomin_.first=true; isomin_.second=value;
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::isomax(const double value)
    {
        isomax_.first=true; isomax_.second=value;
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::log(const int axis, const bool flag)
    {
        log_.push_back(std::pair<int, double>(axis, flag));
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::rangelog(const bool flag)
    {
        rangelog_.first=true; rangelog_.second=flag;
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::isolog(const bool flag)
    {
        isolog_.first=true; isolog_.second=flag;
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::operator<<(hist &h)
    {
        if ( doubleplot_==true )
        {
            warning::print("Cannot append histogram in double-plot mode!", "hist::qplot& hist::qplot::operator<<(hist&)");
            isFlushed_=false;
            return *this;
        }
        oStuff_.push_back(oStuff_t(&h, unset, "__NULL"));
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::put(hist &h)
    {
        if ( doubleplot_==true )
        {
            warning::print("Cannot append histogram in double-plot mode!", "hist::qplot& hist::qplot::put(hist&)");
            isFlushed_=false;
            return *this;
        }
        oStuff_.push_back(oStuff_t(&h, unset, "__NULL"));
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::operator<<(const function &f)
    {
        if ( doubleplot_==true )
        {
            warning::print("Cannot append function in double-plot mode!", "hist::qplot& hist::qplot::operator<<(const function&)");
            isFlushed_=false;
            return *this;
        }
        oStuff_.push_back(oStuff_t(NULL, f, "__NULL"));
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::operator<<(const var &legend)
    {
        if ( doubleplot_==true )
        {
            warning::print("Cannot append function legend in double-plot mode!", "hist::qplot& hist::qplot::operator<<(const var&)");
            isFlushed_=false;
            return *this;
        }
        if ( oStuff_.empty() )
        {
            warning::print("Cannot append function legend: no function appended, yet!", "hist::qplot& hist::qplot::operator<<(const var&)");
            isFlushed_=false;
            return *this;
        }
        int i=oStuff_.size()-1;
        while ( i>=0 ) { if ( oStuff_[i].h==NULL ) { break; } --i; }
        if ( i<0 )
        {
            warning::print("Cannot append function legend: no function appended, yet!", "hist::qplot& hist::qplot::operator<<(const var&)");
            isFlushed_=false;
            return *this;
        }
        oStuff_[i].l=legend;
        isFlushed_=false;
        return *this;
    }


    hist::qplot& hist::qplot::put(const function &f, const var &legend)
    {
        if ( doubleplot_==true )
        {
            warning::print("Cannot append function in double-plot mode!", "hist::qplot& hist::qplot::put(const function&)");
            isFlushed_=false;
            return *this;
        }
        oStuff_.push_back(oStuff_t(NULL, f, legend));
        isFlushed_=false;
        return *this;
    }


    color hist::qplot::color_sequence(const int i)
    {
        static color color_array[]={red, green, blue, magenta, cyan, sienna, orange, pink};
        return color_array[::abs(i)%8];
    }


    color hist::qplot::color_mapping(const double value, const double min, const double max)
    {
        const double d=(value-min)/(max-min);
        return color((::fabs(2.0*d-0.5)>1.0 ? 1.0 : ::fabs(2.0*d-0.5)), ::sin(M_PI*d), ::cos(M_PI/2.0*d));
    }


    hist::qplot& hist::qplot::flush()
    {
        int numStuff=oStuff_.size();
        if ( numStuff==0 )
        {
            warning::print("Empty plot!", "hist::qplot& hist::qplot::flush()");
            isFlushed_=true;
            return *this;
        }
        int k;
        int dim=0;
        for ( k=0 ; k<numStuff ; ++k )
        {
            if ( oStuff_[k].h!=NULL )
            {
                if ( k!=0 && oStuff_[k].h->dim_!=dim )
                {
                    warning::print("Dimensions of histograms do not match!", "hist::qplot& hist::qplot::flush()");
                    isFlushed_=true;
                    return *this;
                }
                dim=oStuff_[k].h->dim_;
            }
            else
            {
                if ( k!=0 && oStuff_[k].f.nargs()!=dim )
                {
                    warning::print("Argument number of functions do not match!", "hist::qplot& hist::qplot::flush()");
                    isFlushed_=true;
                    return *this;
                }
                dim=oStuff_[k].f.nargs();
            }
            if ( dim<1 || dim>3 )
            {
                warning::print("Dimension has to be 1, 2, or 3 !", "hist::qplot& hist::qplot::flush()");
                isFlushed_=true;
                return *this;
            }
        }
        int j;
        if ( globaltitle_.str()!="__NULL" ) frame::current().title(globaltitle_);
        if ( dim==1 )
        {
            for ( j=0 ; j<(int)min_.size() ; ++j )
            {
                if ( min_[j].first==1 ) frame::current().x1axis()->min(min_[j].second);
            }
            for ( j=0 ; j<(int)max_.size() ; ++j )
            {
                if ( max_[j].first==1 ) frame::current().x1axis()->max(max_[j].second);
            }
            if ( rangemin_.first==true ) frame::current().y1axis()->min(rangemin_.second);
            if ( rangemax_.first==true ) frame::current().y1axis()->max(rangemax_.second);
            bool switchlinx=false;
            for ( j=0 ; j<(int)log_.size() ; ++j )
            {
                if ( log_[j].first==1 ) { frame::current().x1axis()->logscale(log_[j].second); switchlinx=true; }
            }
            bool switchliny=false;
            if ( rangelog_.first==true ) { frame::current().y1axis()->logscale(rangelog_.second); switchliny=true; }
            int iok=-1;
            for ( k=0 ; k<numStuff ; ++k )
            {
                if ( oStuff_[k].h!=NULL ) { iok=k; break; }
            }
            if ( iok>=0 )
            {
                if ( oStuff_[iok].h->axistitle_[0].str()!="" ) frame::current().x1axis()->title(oStuff_[iok].h->axistitle_[0]);
                if ( oStuff_[iok].h->rangetitle_.str()!="" ) frame::current().y1axis()->title(oStuff_[iok].h->rangetitle_);
            }
            std::vector<fgraph> fg;
            for ( k=0 ; k<numStuff ; ++k )
            {
                if ( oStuff_[k].h!=NULL )
                {
                    oStuff_[k].h->drawstyle(sxyerrorbars().clip_errorbars(true));
                    oStuff_[k].h->allcolor(color_sequence(k));
                    oStuff_[k].h->pointtype(fsquare());
                    frame::current().add(oStuff_[k].h);
                }
                else
                {
                    fg.push_back(function(_1, oStuff_[k].f(_1)));
                    fg[fg.size()-1].drawstyle(lines());
                    fg[fg.size()-1].allcolor(color_sequence(k));
                    if ( oStuff_[k].l.str()!="__NULL" ) fg[fg.size()-1].legend(oStuff_[k].l);
                    frame::current().add(&fg[fg.size()-1]);
                }
            }
            if ( blopMpps_!=NULL ) blopMpps_->print();
            else if ( blopEpsName_.str()!="" ) blopeps::print(blopEpsName_);
            else x11_ps::print();
            frame::current().clear_graphs();
            frame::current().x1axis()->clear_autosettings();
            frame::current().x2axis()->clear_autosettings();
            frame::current().y1axis()->clear_autosettings();
            frame::current().y2axis()->clear_autosettings();
            if ( switchlinx ) frame::current().x1axis()->logscale(false);
            if ( switchliny ) { frame::current().y1axis()->logscale(false); }
        }
        else if ( dim==2 )
        {
            for ( j=0 ; j<(int)min_.size() ; ++j )
            {
                if ( min_[j].first==1 ) frame::current().x1axis()->min(min_[j].second);
                if ( min_[j].first==2 ) frame::current().y1axis()->min(min_[j].second);
            }
            for ( j=0 ; j<(int)max_.size() ; ++j )
            {
                if ( max_[j].first==1 ) frame::current().x1axis()->max(max_[j].second);
                if ( max_[j].first==2 ) frame::current().y1axis()->max(max_[j].second);
            }
            bool switchlinx=false, switchliny=false;
            for ( j=0 ; j<(int)log_.size() ; ++j )
            {
                if ( log_[j].first==1 ) { frame::current().x1axis()->logscale(log_[j].second); switchlinx=true; }
                if ( log_[j].first==2 ) { frame::current().y1axis()->logscale(log_[j].second); switchliny=true; }
            }
            int iok=-1;
            for ( k=0 ; k<numStuff ; ++k )
            {
                if ( oStuff_[k].h!=NULL ) { iok=k; break; }
            }
            if ( iok>=0 )
            {
                if ( oStuff_[iok].h->axistitle_[0].str()!="" ) frame::current().x1axis()->title(oStuff_[iok].h->axistitle_[0]);
                if ( oStuff_[iok].h->axistitle_[1].str()!="" ) frame::current().y1axis()->title(oStuff_[iok].h->axistitle_[1]);
            }
            double rmin, rmax;
            bool rlog;
            if ( rangemin_.first==true ) rmin=rangemin_.second; else rmin=unset;
            if ( rangemax_.first==true ) rmax=rangemax_.second; else rmax=unset;
            if ( rangelog_.first==true) rlog=rangelog_.second; else rlog=false;
            double cmin, cmax;
            bool clog;
            if ( isomin_.first==true ) cmin=isomin_.second; else cmin=unset;
            if ( isomax_.first==true ) cmax=isomax_.second; else cmax=unset;
            if ( isolog_.first==true) clog=isolog_.second; else clog=false;
            if ( doubleplot_==false )
            {
                for ( k=0 ; k<numStuff ; ++k )
                {
                    fgraph *fg=0;
                    if ( oStuff_[k].h!=NULL )
                    {
                        if ( oStuff_[k].h->rangetitle_.str()!="" ) oStuff_[k].h->drawstyle(csboxes()(color_mapping)(rmin, rmax).title(oStuff_[k].h->rangetitle_).grid_foreground(true).color_logscale(rlog));
                        else oStuff_[k].h->drawstyle(csboxes()(color_mapping)(rmin, rmax).grid_foreground(true).color_logscale(rlog));
                        frame::current().add(oStuff_[k].h);
                    }
                    else
                    {
                        if ( fg!=0 ) { delete fg; fg=0; }
                        fg=new fgraph(function(_1, _2, oStuff_[k].f(_1, _2)));
                        fg->drawstyle(cboxes()(color_mapping)(rmin, rmax).grid_foreground(true).color_logscale(rlog));
                        if ( oStuff_[k].l.str()!="__NULL" ) fg->legend(oStuff_[k].l);
                        frame::current().add(fg);
                    }
                    if ( blopMpps_!=NULL ) blopMpps_->print();
                    else if ( blopEpsName_.str()!="" ) blopeps::print(blopEpsName_&k);
                    else
                    {
                        x11_ps::print();
                        if ( k<numStuff-1 )
                        {
                            std::cerr << "Press ENTER for next plot.\n";
                            std::string buff;
                            std::getline(std::cin, buff);
                        }
                    }
                    frame::current().clear_graphs();
                    if ( fg!=0 ) { delete fg; fg=0; }
                    frame::current().x1axis()->clear_autosettings();
                    frame::current().x2axis()->clear_autosettings();
                    frame::current().y1axis()->clear_autosettings();
                    frame::current().y2axis()->clear_autosettings();
                }
                if ( switchlinx ) frame::current().x1axis()->logscale(false);
                if ( switchliny ) frame::current().y1axis()->logscale(false);
            }
            else
            {
                fgraph *fg0=0;
                fgraph *fg1=0;
                for ( k=0 ; k<numStuff ; ++k )
                {
                    if ( oStuff_[k].h!=NULL )
                    {		    
                        if ( k==0 )
                        {
                            if ( oStuff_[k].h->rangetitle_.str()!="" ) oStuff_[k].h->drawstyle(csboxes()(color_mapping)(rmin, rmax).title(oStuff_[k].h->rangetitle_).grid_foreground(true).color_logscale(rlog));
                            else oStuff_[k].h->drawstyle(csboxes()(color_mapping)(rmin, rmax).grid_foreground(true).color_logscale(rlog));
                        }
                        else
                        {
                            if ( oStuff_[k].h->rangetitle_.str()!="" )
                                oStuff_[k].h->drawstyle(isolines().min(cmin).max(cmax)
                                                        //.title(oStuff_[k].h->rangetitle_)
                                                        .logscale(clog));
                            else oStuff_[k].h->drawstyle(isolines().min(cmin).max(cmax)
                                                         .logscale(clog));
                        }
                        frame::current().add(oStuff_[k].h);
                    }
                    else
                    {
                        if ( k==0 )
                        {
                            if ( fg0!=0 ) { delete fg0 ; fg0=0; }
                            fg0=new fgraph(function(_1, _2, oStuff_[k].f(_1, _2)));
                            fg0->drawstyle(cboxes()(color_mapping)(rmin, rmax).grid_foreground(true).color_logscale(rlog));
                            if ( oStuff_[k].l.str()!="__NULL" ) fg0->legend(oStuff_[k].l);
                            frame::current().add(fg0);
                        }
                        else
                        {
                            if ( fg1!=0 ) { delete fg1 ; fg1=0; }
                            fg1=new fgraph(function(_1, _2, oStuff_[k].f(_1, _2)));
                            fg1->drawstyle(isolines().min(cmin).max(cmax).logscale(clog));
                            if ( oStuff_[k].l.str()!="__NULL" ) fg1->legend(oStuff_[k].l);
                            frame::current().add(fg1);
                        }
                    }
                }
                if ( blopMpps_!=NULL ) blopMpps_->print();
                else if ( blopEpsName_.str()!="" ) blopeps::print(blopEpsName_);
                else x11_ps::print();
                frame::current().clear_graphs();
                if ( fg0!=0 ) { delete fg0; fg0=0; }
                if ( fg1!=0 ) { delete fg1; fg1=0; }
                frame::current().x1axis()->clear_autosettings();
                frame::current().x2axis()->clear_autosettings();
                frame::current().y1axis()->clear_autosettings();
                frame::current().y2axis()->clear_autosettings();
                if ( switchlinx ) frame::current().x1axis()->logscale(false);
                if ( switchliny ) frame::current().y1axis()->logscale(false);
            }
        }
        else if ( dim==3)
        {
            for ( j=0 ; j<(int)min_.size() ; ++j )
            {
                if ( min_[j].first==2 ) frame::current().x1axis()->min(min_[j].second);
                if ( min_[j].first==3 ) frame::current().y1axis()->min(min_[j].second);
            }
            for ( j=0 ; j<(int)max_.size() ; ++j )
            {
                if ( max_[j].first==2 ) frame::current().x1axis()->max(max_[j].second);
                if ( max_[j].first==3 ) frame::current().y1axis()->max(max_[j].second);
            }
            bool switchlinx=false, switchliny=false;
            for ( j=0 ; j<(int)log_.size() ; ++j )
            {
                if ( log_[j].first==2 ) { frame::current().x1axis()->logscale(log_[j].second); switchlinx=true; }
                if ( log_[j].first==3 ) { frame::current().y1axis()->logscale(log_[j].second); switchliny=true; }
            }
            int iok=-1;
            for ( k=0 ; k<numStuff ; ++k )
            {
                if ( oStuff_[k].h!=NULL ) { iok=k; break; }
            }
            if ( iok>=0 )
            {
                if ( oStuff_[iok].h->axistitle_[1].str()!="" ) frame::current().x1axis()->title(oStuff_[iok].h->axistitle_[1]);
                if ( oStuff_[iok].h->axistitle_[2].str()!="" ) frame::current().y1axis()->title(oStuff_[iok].h->axistitle_[2]);
            }
            double rmin, rmax;
            bool rlog;
            if ( rangemin_.first==true ) rmin=rangemin_.second; else rmin=unset;
            if ( rangemax_.first==true ) rmax=rangemax_.second; else rmax=unset;
            if ( rangelog_.first==true) rlog=rangelog_.second; else rlog=false;
            double cmin, cmax;
            bool clog;
            if ( isomin_.first==true ) cmin=isomin_.second; else cmin=unset;
            if ( isomax_.first==true ) cmax=isomax_.second; else cmax=unset;
            if ( isolog_.first==true) clog=isolog_.second; else clog=false;
            if ( doubleplot_==false )
            {
                for ( k=0 ; k<numStuff ; ++k )
                {
                    if ( oStuff_[k].h!=NULL )
                    {
                        for ( int i=0 ; i<oStuff_[k].h->bins_[0] ; ++i )
                        {
                            double pos=(oStuff_[k].h->max_[0]-oStuff_[k].h->min_[0])/oStuff_[k].h->bins_[0]*(i+0.5)+oStuff_[k].h->min_[0];
                            bool skip=false;
                            for ( j=0 ; j<(int)min_.size() ; ++j )
                            {
                                if ( min_[j].first==1 ) if ( pos<min_[j].second ) { skip=true; break; }
                            }
                            if ( skip==true ) continue;
                            for ( j=0 ; j<(int)max_.size() ; ++j )
                            {
                                if ( max_[j].first==1 ) if ( pos>max_[j].second ) { skip=true; break; }
                            }
                            if ( skip==true ) continue;
                            hist *ohist=new hist(oStuff_[k].h->slice(1, pos));
                            ohist->legend_=oStuff_[k].h->legend_&" at axis("&1&")="&pos;
                            if ( oStuff_[k].h->rangetitle_.str()!="" ) ohist->drawstyle(csboxes()(color_mapping)(rmin, rmax).title(oStuff_[k].h->rangetitle_).grid_foreground(true).color_logscale(rlog));
                            else ohist->drawstyle(csboxes()(color_mapping)(rmin, rmax).grid_foreground(true).color_logscale(rlog));
                            frame::current().add(ohist);
                            if ( blopMpps_!=NULL ) blopMpps_->print();
                            else if ( blopEpsName_.str()!="" ) blopeps::print(blopEpsName_&k);
                            else
                            {
                                x11_ps::print();
                                if ( k<numStuff-1 || i<oStuff_[k].h->bins_[0]-1 )
                                {
                                    std::cerr << "Press ENTER for next plot.\n";
                                    std::string buff;
                                    std::getline(std::cin, buff);
                                }
                            }
                            frame::current().clear_graphs();
                            delete ohist; ohist=0;
                            frame::current().x1axis()->clear_autosettings();
                            frame::current().x2axis()->clear_autosettings();
                            frame::current().y1axis()->clear_autosettings();
                            frame::current().y2axis()->clear_autosettings();
                        }
                    }
                    else
                    {
                        int bins0=10;
                        double defmin=0.0, defmax=10.0;
                        for ( int i=0 ; i<bins0 ; ++i )
                        {
                            double pos, min=unset, max=unset;
                            for ( j=0 ; j<(int)min_.size() ; ++j )
                            {
                                if ( min_[j].first==1 ) min=min_[j].second;
                            }
                            for ( j=0 ; j<(int)max_.size() ; ++j )
                            {
                                if ( max_[j].first==1 ) max=max_[j].second;
                            }
                            if ( min==unset && max==unset ) { min=defmin; max=defmax; }
                            else if ( min==unset && max!=unset ) { min=max-(defmax-defmin); }
                            else if ( min!=unset && max==unset ) { max=min+(defmax-defmin); }
                            pos=(max-min)/bins0*(i+0.5)+min;
                            fgraph *ofunc=new fgraph(function(_1, _2, oStuff_[k].f(function(pos), _1, _2)));
                            if ( oStuff_[k].l.str()!="__NULL" ) ofunc->legend(oStuff_[k].l&" at axis("&1&")="&pos);
                            ofunc->drawstyle(cboxes()(color_mapping)(rmin, rmax).grid_foreground(true).color_logscale(rlog));
                            frame::current().add(ofunc);
                            if ( blopMpps_!=NULL ) blopMpps_->print();
                            else if ( blopEpsName_.str()!="" ) blopeps::print(blopEpsName_&k);
                            else
                            {
                                x11_ps::print();
                                if ( k<numStuff-1 || i<bins0-1 )
                                {
                                    std::cerr << "Press ENTER for next plot.\n";
                                    std::string buff;
                                    std::getline(std::cin, buff);
                                }
                            }
                            frame::current().clear_graphs();
                            delete ofunc; ofunc=0;
                            frame::current().x1axis()->clear_autosettings();
                            frame::current().x2axis()->clear_autosettings();
                            frame::current().y1axis()->clear_autosettings();
                            frame::current().y2axis()->clear_autosettings();
                        }
                    }
                }
                if ( switchlinx ) frame::current().x1axis()->logscale(false);
                if ( switchliny ) frame::current().y1axis()->logscale(false);
            }
            else
            {
                if ( oStuff_[0].h!=NULL || oStuff_[1].h!=NULL )
                {
                    int kok=0;
                    int kother=1;
                    if ( oStuff_[0].h!=NULL ) { kok=0; kother=1; }
                    else { kok=1; kother=0; }
                    for ( int i=0 ; i<oStuff_[kok].h->bins_[0] ; ++i )
                    {
                        double pos=(oStuff_[kok].h->max_[0]-oStuff_[kok].h->min_[0])/oStuff_[kok].h->bins_[0]*(i+0.5)+oStuff_[kok].h->min_[0];
                        bool skip=false;
                        for ( j=0 ; j<(int)min_.size() ; ++j )
                        {
                            if ( min_[j].first==1 ) if ( pos<min_[j].second ) { skip=true; break; }
                        }
                        if ( skip==true ) continue;
                        for ( j=0 ; j<(int)max_.size() ; ++j )
                        {
                            if ( max_[j].first==1 ) if ( pos>max_[j].second ) { skip=true; break; }
                        }
                        if ( skip==true ) continue;
                        hist *ohist=new hist(oStuff_[kok].h->slice(1, pos));
                        ohist->legend_=oStuff_[kok].h->legend_&" at axis("&1&")="&pos;
                        if ( kok==0 )
                        {
                            if ( oStuff_[kok].h->rangetitle_.str()!="" ) ohist->drawstyle(csboxes()(color_mapping)(rmin, rmax).title(oStuff_[kok].h->rangetitle_).grid_foreground(true).color_logscale(rlog));
                            else ohist->drawstyle(csboxes()(color_mapping)(rmin, rmax).grid_foreground(true).color_logscale(rlog));
                        }
                        else
                        {
                            if ( oStuff_[kok].h->rangetitle_.str()!="" )
                                ohist->drawstyle(isolines().min(cmin).max(cmax)
                                                 //.title(oStuff_[kok].h->rangetitle_)
                                                 .logscale(clog));
                            else ohist->drawstyle(isolines().min(cmin).max(cmax).logscale(clog));
                        }
                        hist *ohistother=0;
                        hist *ofuncother=0;
                        if ( oStuff_[kother].h!=NULL )
                        {
                            hist *ohistother=new hist(oStuff_[kother].h->slice(1, pos));
                            ohistother->legend_=oStuff_[kother].h->legend_&" at axis("&1&")="&pos;
                            if ( kother==0 )
                            {
                                if ( oStuff_[kother].h->rangetitle_.str()!="" ) ohistother->drawstyle(csboxes()(color_mapping)(rmin, rmax).title(oStuff_[kother].h->rangetitle_).grid_foreground(true).color_logscale(rlog));
                                else ohistother->drawstyle(csboxes()(color_mapping)(rmin, rmax).grid_foreground(true).color_logscale(rlog));
                            }
                            else
                            {
                                if ( oStuff_[kother].h->rangetitle_.str()!="" )
                                    ohistother->drawstyle(isolines().min(cmin).max(cmax)
                                                          //.title(oStuff_[kother].h->rangetitle_)
                                                          .logscale(clog));
                                else ohistother->drawstyle(isolines().min(cmin).max(cmax).logscale(clog));
                            }
                        }
                        else
                        {
                            fgraph *ofuncother=new fgraph(function(_1, _2, oStuff_[kother].f(function(pos), _1, _2)));
                            if ( oStuff_[kother].l.str()!="__NULL" ) ofuncother->legend(oStuff_[kother].l&" at axis("&1&")="&pos);
                            if ( kother==0 )
                            {
                                ofuncother->drawstyle(cboxes()(color_mapping)(rmin, rmax).grid_foreground(true).color_logscale(rlog));
                            }
                            else
                            {
                                ohistother->drawstyle(isolines().min(cmin).max(cmax).logscale(clog));
                            }
                        }
                        if ( kok==0 )
                        {
                            frame::current().add(ohist);
                            if ( ohistother!=0 ) frame::current().add(ohistother);
                            else frame::current().add(ofuncother);
                        }
                        else
                        {
                            if ( ohistother!=0 ) frame::current().add(ohistother);
                            else frame::current().add(ofuncother);
                            frame::current().add(ohist);
                        }
                        if ( blopMpps_!=NULL ) blopMpps_->print();
                        else if ( blopEpsName_.str()!="" ) blopeps::print(blopEpsName_&k);
                        else
                        {
                            x11_ps::print();
                            if ( i<oStuff_[kok].h->bins_[0]-1 )
                            {
                                std::cerr << "Press ENTER for next plot.\n";
                                std::string buff;
                                std::getline(std::cin, buff);
                            }
                        }
                        frame::current().clear_graphs();
                        delete ohist; ohist=0;
                        if ( ohistother!=0 ) { delete ohistother; ohistother=0; }
                        if ( ofuncother!=0 ) { delete ofuncother; ofuncother=0; }
                        frame::current().x1axis()->clear_autosettings();
                        frame::current().x2axis()->clear_autosettings();
                        frame::current().y1axis()->clear_autosettings();
                        frame::current().y2axis()->clear_autosettings();
                    }
                    if ( switchlinx ) frame::current().x1axis()->logscale(false);
                    if ( switchliny ) frame::current().y1axis()->logscale(false);
                }
                else
                {
                    int bins0=10;
                    double defmin=0.0, defmax=10.0;
                    for ( int i=0 ; i<bins0 ; ++i )
                    {
                        double pos, min=unset, max=unset;
                        for ( j=0 ; j<(int)min_.size() ; ++j )
                        {
                            if ( min_[j].first==1 ) min=min_[j].second;
                        }
                        for ( j=0 ; j<(int)max_.size() ; ++j )
                        {
                            if ( max_[j].first==1 ) max=max_[j].second;
                        }
                        if ( min==unset && max==unset ) { min=defmin; max=defmax; }
                        else if ( min==unset && max!=unset ) { min=max-(defmax-defmin); }
                        else if ( min!=unset && max==unset ) { max=min+(defmax-defmin); }
                        pos=(max-min)/bins0*(i+0.5)+min;
                        fgraph *ofunc=new fgraph(function(_1, _2, oStuff_[0].f(function(pos), _1, _2)));
                        if ( oStuff_[0].l.str()!="__NULL" ) ofunc->legend(oStuff_[0].l&" at axis("&1&")="&pos);
                        ofunc->drawstyle(cboxes()(color_mapping)(rmin, rmax).grid_foreground(true).color_logscale(rlog));
                        frame::current().add(ofunc);
                        fgraph *ofuncother=new fgraph(function(_1, _2, oStuff_[1].f(function(pos), _1, _2)));
                        if ( oStuff_[1].l.str()!="__NULL" ) ofuncother->legend(oStuff_[1].l&" at axis("&1&")="&pos);
                        ofuncother->drawstyle(isolines().min(cmin).max(cmax).logscale(clog));
                        frame::current().add(ofuncother);
                        if ( blopMpps_!=NULL ) blopMpps_->print();
                        else if ( blopEpsName_.str()!="" ) blopeps::print(blopEpsName_&k);
                        else
                        {
                            x11_ps::print();
                            if ( i<bins0-1 )
                            {
                                std::cerr << "Press ENTER for next plot.\n";
                                std::string buff;
                                std::getline(std::cin, buff);
                            }
                        }
                        frame::current().clear_graphs();
                        delete ofunc; ofunc=0;
                        delete ofuncother; ofuncother=0;
                        frame::current().x1axis()->clear_autosettings();
                        frame::current().x2axis()->clear_autosettings();
                        frame::current().y1axis()->clear_autosettings();
                        frame::current().y2axis()->clear_autosettings();
                    }
                    if ( switchlinx ) frame::current().x1axis()->logscale(false);
                    if ( switchliny ) frame::current().y1axis()->logscale(false);
                }
            }
        }
        isFlushed_=true;
        return *this;
    }


    hist::qplot::~qplot()
    {
        if ( isFlushed_==false ) flush();
    }


    hist& mkhist(istream &input, const histopt &hopt)
    {
        hist *h=new hist(hopt);
        h->fill_from(input, hopt);
        return *h;
    }


    hist& mkhist(const var &filename, const histopt &hopt)
    {
        hist *h=new hist(hopt);
        h->fill_from(filename, hopt);
        return *h;
    }


    hist& mkhist(const dgraph &dg, const histopt &hopt)
    {
        hist *h=new hist(hopt);
        h->fill_from(dg, hopt);
        return *h;
    }


    hist& histplot(istream &input, const histopt &hopt)
    {
        hist &h=mkhist(input, hopt);
        h.autodel(true);
        frame::current().clear_graphs();
        frame::current().add(&h);
        return h;
    }


    hist& mhistplot(istream &input, const histopt &hopt)
    {
        hist &h=mkhist(input, hopt);
        h.autodel(true);
        frame::current().add(&h);
        return h;
    }


    hist& histplot(const var &filename, const histopt &hopt)
    {
        hist &h=mkhist(filename, hopt);
        h.autodel(true);
        frame::current().clear_graphs();
        frame::current().add(&h);
        return h;
    }


    hist& mhistplot(const var &filename, const histopt &hopt)
    {
        hist &h=mkhist(filename, hopt);
        h.autodel(true);
        frame::current().add(&h);
        return h;
    }


    hist& histplot(const dgraph &dg, const histopt &hopt)
    {
        hist &h=mkhist(dg, hopt);
        h.autodel(true);
        frame::current().clear_graphs();
        frame::current().add(&h);
        return h;
    }


    hist& mhistplot(const dgraph &dg, const histopt &hopt)
    {
        hist &h=mkhist(dg, hopt);
        h.autodel(true);
        frame::current().add(&h);
        return h;
    }


}



