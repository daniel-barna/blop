#include <algorithm>
#include <typeinfo>
#include <sstream>
#include "legendbox.h"
#include "graph_drawer.h"
#include "frame.h"
#include "exc.hh"

namespace blop
{
    length legendbox::default_borderwidth_(new length::base_id_t(terminal::LW));
    void legendbox::default_borderwidth(const length &l) { default_borderwidth_ = l; }
    bool legendbox::default_multi_ = false;
    void legendbox::default_multi(bool f) { default_multi_ = f; }
    color legendbox::default_bordercolor_(0,0,0);
    void legendbox::default_bordercolor(const color &c) { default_bordercolor_ = c; }
    color legendbox::default_fillcolor_(1,1,1);
    void legendbox::default_fillcolor(const color &c) { default_fillcolor_ = c; }
    bool legendbox::default_draw_border_ = true;
    void legendbox::default_draw_border(bool f) { default_draw_border_ = f; }
    bool legendbox::default_fill_ = true;
    void legendbox::default_fill(bool f) { default_fill_ = f; }
    length legendbox::default_bordersep_(new length::base_id_t(terminal::MM));
    void legendbox::default_bordersep(const length &l) { default_bordersep_ = l; }
    length legendbox::default_legend_sample_sep_ = lincombi(2,terminal::MM);
    void legendbox::default_legend_sample_sep(const length &l) { default_legend_sample_sep_ = l; }
    length legendbox::default_sample_sample_sep_(new length::base_id_t(terminal::MM));
    void legendbox::default_sample_sample_sep(const length &l) { default_sample_sample_sep_ = l; }
    length legendbox::default_sample_length_ = lincombi(4,terminal::MM);
    void legendbox::default_samplen( const length &l ) { default_sample_length_ = l; }
    length legendbox::default_row_sep_(new length::base_id_t(terminal::EX));
    void legendbox::default_row_sep(const length &l) { default_row_sep_ = l; }

    legendbox &legendbox::mknew(container &parent)
    {
	legendbox *l = new legendbox;
	l->autodel(true);
	parent.add(l);
	return *l;
    }

    void legendbox::print(terminal *term)
    {
	if(print_me_ < 2) return;
	if(entries_.empty()) return;

	{
	    bool something = false;
	    for(unsigned int i=0; i<entries_.size(); ++i)
	    {
                // If it is just a label, or if it's a graph to be printed, set the flag
		if(!skip_(entries_[i])) something = true;
	    }
	    // if there is nothing to print, simply return
	    if(!something) return;
	}

	term->open_layer(layer_);
	{
	    vector<terminal::coord> cc;

	    if(draw_border_ || fill_)
	    {
		cc.push_back(terminal::coord(left().termspecific_id(),
					     bottom().termspecific_id()));
		cc.push_back(terminal::coord(right().termspecific_id(),
					     bottom().termspecific_id()));
		cc.push_back(terminal::coord(right().termspecific_id(),
					     top().termspecific_id()));
		cc.push_back(terminal::coord(left().termspecific_id(),
					     top().termspecific_id()));
		cc.push_back(terminal::coord(left().termspecific_id(),
					     bottom().termspecific_id()));
	    }

	    if(fill_)
	    {
		term->set_color(fillcolor_);
		term->fill_polygon(cc);
	    }
	    if(draw_border_)
	    {
		term->set_linewidth(borderwidth_.termspecific_id());
		term->set_linestyle(sym::solid);
		term->set_color(bordercolor_);
		term->draw_lines(cc);
	    }
	}

        

	for(unsigned int i=0; i<entries_.size(); ++i)
	{
	    if(skip_(entries_[i])) continue;

            
            // Print the label (text) only if this is the first occurence within a multi-legend row
	    if(entries_[i]->multi_number == 1) entries_[i]->legend.print(term);

	    if(entries_[i]->multi_number >= 1 && entries_[i]->gr)
	    {
		graph_drawer *d = entries_[i]->gr->drawstyle();
		d->draw_sample(entries_[i]->sample_x,
			       entries_[i]->sample_y,
			       sample_length_,
			       entries_[i]->gr,
			       term);
	    }

	}


	term->close_layer(layer_);

    }

    void legendbox::prepare_for_draw()
    {
        update();

	if(print_me_ < 1) return;
	left().register_me();
	right().register_me();
	bottom().register_me();
	top().register_me();
	sample_length_.register_me();

	if(draw_border_) borderwidth_.register_me();

	for(unsigned int i=0; i<entries_.size(); ++i)
	{
	    if(skip_(entries_[i])) continue;
	    entries_[i]->legend.prepare_for_draw();
            if(entries_[i]->gr)
            {
                entries_[i]->sample_x.register_me();
                entries_[i]->sample_y.register_me();
            }
	}
    }

    legendbox &legendbox::multi(bool i)
    {
	multi_ = i;
	return *this; 
    }

    legendbox  &legendbox::add_line()
    {
        entry *e = new entry;
        e->line = true;
        e->gr = 0;
        e->multi_number = 1;
        entries_.push_back(e);
        return *this;
    }

    label &legendbox::add(const var &t)
    {
        entry *e = new entry;
        e->line = false;
        e->gr = 0;
        e->legend.text(t);
        e->multi_number = 1;
        entries_.push_back(e);
        return e->legend;
    }

    void legendbox::add(blop::plottable *g)
    {
	entry *e = new entry;
        e->line = false;
	e->gr = g;
	e->multi_number = 1;
	entries_.push_back(e);
    }

    bool legendbox::remove(blop::plottable *g)
    {
	for(unsigned int i=0; i<entries_.size(); ++i)
	{
	    if(entries_[i]->gr == g)
	    {
		delete entries_[i];
		entries_.erase(entries_.begin() + i);
		return true;
	    }
	}
	return false;
    }

    void legendbox::clear()
    {
	for(unsigned int i=0; i<entries_.size(); ++i)
	{
	    delete entries_[i];
	}
	entries_.clear();
    }

/*
    bool legendbox::skip_(plottable *g)
    {
	if(g == 0) return true;
	graph_drawer *d = g->drawstyle();
	if(d == 0 || !d->draws_sample()) return true;
	if(g->legend().str() == "") return true;
	return false;
    }
*/

    bool legendbox::skip_(legendbox::entry *e)
    {
        // never skip simple text entries (i.e. no graph)
        if(e->gr == 0) return false;

        plottable *g = e->gr;
	graph_drawer *d = g->drawstyle();
	if(d == 0 || !d->draws_sample()) return true;
	if(g->legend().str() == "") return true;
	return false;
    }

    void legendbox::update()
    {
	vector<var> different_labels;

	// collect those which should be printed
	vector<legendbox::entry*> entries;
	for(unsigned int i=0; i<entries_.size(); ++i)
	{
	    if(!skip_(entries_[i])) entries.push_back(entries_[i]);
	}

	max_multisample_ = 1;
	for(unsigned int i=0; i<entries.size(); ++i)
	{
	    int prev = -1;
	    if(multi_ && entries[i]->gr)
	    {
                
		// check if the legend of the ith graph coincides with any
		// of the preceding graphs' legends
		for(unsigned int j=0; j<i; ++j)
		{

                    // Only check those which have a graph.
                    if(entries[j]->gr == 0) continue;
                    
		    if(entries[i]->gr->legend().str() ==
		       entries[j]->gr->legend().str())
		    {
			if(prev < 0) prev = j;

			plottable *g1 = entries[i]->gr;
			plottable *g2 = entries[j]->gr;
			    
			string p1, p2, l1, l2;
			{
			    ostringstream ost;
			    ost<<g1->pointsize();
			    p1 = ost.str();
			}
			{
			    ostringstream ost;
			    ost<<g2->pointsize();
			    p2 = ost.str();
			}
			{
			    ostringstream ost;
			    ost<<g1->linewidth();
			    l1 = ost.str();
			}
			{
			    ostringstream ost;
			    ost<<g2->linewidth();
			    l2 = ost.str();
			}

			if(typeid(*(g1->drawstyle())) == typeid(*(g2->drawstyle())) &&
			   g1->fill()      == g2->fill()      &&
			   (!g1->fill() || (g1->fillcolor() == g2->fillcolor())) &&
			   (!g1->drawstyle()->uses_linecolor() || (g1->linecolor() == g2->linecolor())) &&
			   (!g1->drawstyle()->uses_linestyle() || (g1->linestyle() == g2->linestyle())) &&
                           (!g1->drawstyle()->uses_linewidth() || (l1 == l2)) && 
			   (!g1->drawstyle()->draws_points() || equals(g1->pointtype(), g2->pointtype())) &&
			   (!g1->drawstyle()->uses_pointcolor() || (g1->pointcolor() == g2->pointcolor()))  &&
			   (!g1->drawstyle()->uses_pointsize() || (p1 == p2)))
			{
			    entries[i]->multi_number = 0;
			}
			else
                        {
                            entries[i]->multi_number = entries[j]->multi_number+1;
                        }
		    }
		}
	    }

	    if(entries[i]->multi_number > max_multisample_)
		max_multisample_ = entries[i]->multi_number;

            if(entries[i]->gr)
            {
                entries[i]->legend.text(entries[i]->gr->legend());
                entries[i]->legend.textcolor(entries[i]->gr->legendcolor());

                entries[i]->sample_y = !entries[i]->legend.bottom() + 0.5 * !entries[i]->legend.height();
                entries[i]->sample_x = !legend_xpos_ + !legend_sample_sep_ +
                    (entries[i]->multi_number - 1) * !sample_sample_sep_ +
                    (entries[i]->multi_number - 1 + 0.5) * !sample_length_;
            }

            // Now set the multi_number of the legend titles (or labels)
	    if(prev < 0) // does not match any previous entry, or it is a standalone label without a graph
	    {
                if(entries[i]->gr) different_labels.push_back(entries[i]->gr->legend());
                else different_labels.push_back(entries[i]->legend.text());
                
		entries[i]->legend.x(!legend_xpos_,sym::right);
		if(i == 0)
		{
		    if(draw_border_) entries[i]->legend.y(!top() - !bordersep_,sym::top);
		    else entries[i]->legend.y(!top(),sym::top);
		}
		else
		{
		    entries[i]->legend.y(!entries[i-1]->legend.bottom() - !row_sep_, sym::top);
		}
	    }
            // Otherwise, if this entry matches a previous one, we will in fact not print the label (only the first
            // occurence), but the legend needs to be updated, since following labels will rely on this label's
            // position
	    else 
	    {
		entries[i]->legend = entries[prev]->legend;
	    }   
	}

	if(different_labels.empty()) return;

	max_legendwidth_ = maxwidth(different_labels);
	if(draw_border_)
	{
	    legend_xpos_ = !left() + !bordersep_ + !max_legendwidth_;
	    calculated_width_ =
		!max_legendwidth_ +
		2*!bordersep_ +
		!legend_sample_sep_ +
		max_multisample_*!sample_length_ +
		(max_multisample_ - 1) * !sample_sample_sep_;
	    calculated_height_ = 2*!bordersep_ + (different_labels.size() - 1) * !row_sep_;
	}
	else
	{
	    legend_xpos_ = !left() + !max_legendwidth_;
	    calculated_width_ =
		!max_legendwidth_ +
		!legend_sample_sep_ +
		max_multisample_*!sample_length_ +
		(max_multisample_ - 1) * !sample_sample_sep_;
	    calculated_height_ = (different_labels.size() - 1) * !row_sep_;
	}

	for(unsigned int i=0; i<entries.size(); ++i)
	{
	    if(entries[i]->multi_number == 1)
		//calculated_height_ += blop::height(entries[i]->gr->legend());
		calculated_height_ += entries[i]->legend.height();
                
	}

    }

    legendbox &legendbox::width(length len)
    {
	if(!len.initialized()) box::width(!calculated_width_);
	else box::width(len);
	return *this;
    }

    legendbox &legendbox::left(const length &l)
    {
	box::left(l);
	return *this;
    }
    legendbox &legendbox::right(const length &l)
    {
	box::right(l);
	return *this;
    }
    legendbox &legendbox::xcenter(const length &l)
    {
	box::xcenter(l);
	return *this;
    }
    legendbox &legendbox::bottom(const length &l)
    {
	box::bottom(l);
	return *this;
    }
    legendbox &legendbox::top(const length &l)
    {
	box::top(l);
	return *this;
    }
    legendbox &legendbox::ycenter(const length &l)
    {
	box::ycenter(l);
	return *this;
    }


    legendbox &legendbox::height(length len)
    {
	if(!len.initialized()) box::height(!calculated_height_);
	else box::height(len);
	return *this;
    }

    legendbox &legendbox::borderwidth(const length &l)
    {
	borderwidth_ = l;
	return *this;
    }

    const length &legendbox::borderwidth() const
    {
	return borderwidth_;
    }


    legendbox::~legendbox()
    {
	std::vector<blop::legendbox*>::iterator i = find(all().begin(), all().end(), this);
	if(i == all().end())
	    err("Unregistered legendbox (programming error)");
	all().erase(i);
    }

    legendbox::legendbox()
	: multi_(default_multi_), max_multisample_(0)
    {
	name("legendbox");
	layer("legend");

	all().push_back(this);

	box::width(!calculated_width_);
	box::height(!calculated_height_);

	bordercolor_ = default_bordercolor_;
	fillcolor_ = default_fillcolor_;

	draw_border_ = default_draw_border_;
	fill_ = default_fill_;

	borderwidth_ = default_borderwidth_;
	legend_xpos_ = !left() + !bordersep_ + !max_legendwidth_;
	bordersep_ = default_bordersep_;
	legend_sample_sep_ = default_legend_sample_sep_;
	sample_sample_sep_ = default_sample_sample_sep_;
	sample_length_ = default_sample_length_;
	row_sep_ = default_row_sep_;
    }

    legendbox &legendbox::bordercolor(const color &c)
    {
	bordercolor_ = c;
	draw_border_ = true;
	return *this;
    }
    const color &legendbox::bordercolor() const
    {
	return bordercolor_;
    }

    legendbox &legendbox::fillcolor(const color &c)
    {
	fillcolor_ = c;
	fill_ = true;
	return *this;
    }
    const color &legendbox::fillcolor() const
    {
	return fillcolor_;
    }

    legendbox &legendbox::draw_border(bool i)
    {
	draw_border_ = i;
	return *this;
    }
    bool legendbox::draw_border() const
    {
	return draw_border_;
    }

    legendbox &legendbox::fill(bool i)
    {
	fill_ = i;
	return *this;
    }
    bool legendbox::fill() const
    {
	return fill_;
    }

    legendbox &legendbox::samplen(const length &l)
    {
	sample_length_ = l;
	return *this;
    }

    const length &legendbox::samplen() const
    {
	return sample_length_;
    }

    legendbox &legendbox::bordersep(const length &l)
    {
	bordersep_ = l;
	return *this;
    }

    const length &legendbox::bordersep() const
    {
	return bordersep_;
    }

    legendbox &legendbox::legend_sample_sep(const length &l)
    {
	legend_sample_sep_ = l;
	return *this;
    }
    const length &legendbox::legend_sample_sep() const
    {
	return legend_sample_sep_;
    }

    legendbox &legendbox::sample_sample_sep(const length &l)
    {
	sample_sample_sep_ = l;
	return *this;
    }
    const length &legendbox::sample_sample_sep() const
    {
	return sample_sample_sep_;
    }

    legendbox &legendbox::row_sep(const length &l)
    {
	row_sep_ = l;
	return *this;
    }
    const length &legendbox::row_sep() const
    {
	return row_sep_;
    }

    std::vector<blop::legendbox*> &legendbox::all()
    {
	static vector<legendbox*> *f = new vector<legendbox*>;
	return *f;
    }

    void legendbox::remove_from_all(plottable *p)
    {
	for(unsigned int i=0; i<all().size(); ++i)
	{
	    all()[i]->remove(p);
	}
    }
    
}
