#ifndef __BLOP_CONTAINER_H__
#define __BLOP_CONTAINER_H__

#include "box.h"

namespace blop
{

    /************************************************************

    A container is used to store grobs (or other containers).
    It always stores a reference to the objects, and does not
    copy them. 

    */


    class container : public box
    {
    public:
	// -------------------------------------------------------------------
	// a static vector, keeping the pointers of all existing containers
	static std::vector<blop::container *> &all();

    protected:
	std::vector<blop::grob*> content_;

    private:
	length cleft_  ,cright_,cwidth_ ,cxcenter_,
	       cbottom_,ctop_  ,cheight_,cycenter_;
	length lmargin_, rmargin_, bmargin_, tmargin_;

	box::history_values cxhistory_[2], cyhistory_[2];
	enum lmargin_history_values {Left, Lmargin, Cleft} lmargin_history_[2];
	enum rmargin_history_values {Right, Rmargin, Cright} rmargin_history_[2];
	enum bmargin_history_values {Bottom, Bmargin, Cbottom} bmargin_history_[2];
	enum tmargin_history_values {Top, Tmargin, Ctop} tmargin_history_[2];

	void update_c_x_(bool left_modified, bool right_modified);
	void update_c_y_(bool left_modified, bool right_modified);
	void update_x_(bool cleft_modified, bool cright_modified);
	void update_y_(bool cleft_modified, bool cright_modified);

	void set_cleft_(const length &, bool&, bool&);
	void set_cright_(const length &, bool&, bool&);
	void set_cwidth_(const length &, bool&, bool&);
	void set_cxcenter_(const length &, bool&, bool&);
	void set_cbottom_(const length &, bool&, bool&);
	void set_ctop_(const length &, bool&, bool&);
	void set_cheight_(const length &, bool&, bool&);
	void set_cycenter_(const length &, bool&, bool&);

	void set_lmargin_(const length &l);
	void set_rmargin_(const length &l);
	void set_bmargin_(const length &l);
	void set_tmargin_(const length &l);

	sym::position cx_lock_, cy_lock_;
	double caspect_;
    public:

	// ------------------------------------------------------------
	// Containers define their own coordinate system, and the
	// placement of the contents of the containers is relative
	// to this coordinate system. 
	// These member functions return the edges (and width,height)
	// of this own coordinate system, relative to the coord.sys.
	// of the parent of this container.

	virtual const length &cleft() const;
	virtual const length &cright() const;
	virtual const length &cwidth() const;
	virtual const length &cxcenter() const;
	virtual const length &cbottom() const;
	virtual const length &ctop() const;
	virtual const length &cheight() const;
	virtual const length &cycenter() const;

	virtual const length &lmargin() const { return lmargin_; }
	virtual const length &rmargin() const { return rmargin_; }
	virtual const length &bmargin() const { return bmargin_; }
	virtual const length &tmargin() const { return tmargin_; }

	virtual container &cleft(const length &);
	virtual container &cright(const length &);
	virtual container &cwidth(const length &);
	virtual container &cxcenter(const length &);
	virtual container &cbottom(const length &);
	virtual container &ctop(const length &);
	virtual container &cheight(const length &);
	virtual container &cycenter(const length &);

	virtual container &lmargin(const length &);
	virtual container &rmargin(const length &);
	virtual container &bmargin(const length &);
	virtual container &tmargin(const length &);
	virtual container &margins(const length &);

	// inherited from box, but overwritten here to return a container& and not a box&
	container &left(const length &);
	container &right(const length &);
	container &width(const length &);
	container &xcenter(const length &);
	container &bottom(const length &);
	container &top(const length &);
	container &height(const length  &);
	container &ycenter(const length &);

	virtual container &caspect(double r);
	virtual container &cxlock(sym::position);
	virtual container &cylock(sym::position);
	virtual sym::position cxlock() const { return cx_lock_; }
	virtual sym::position cylock() const { return cy_lock_; }

	void prepare_for_draw();

	void print(terminal *);

	container();
	virtual ~container();

	// ----------- Remove the given grob from all existing containers --------
	static void remove_from_all (grob *);


	// --------------- Remove the given grob from a container ----------------
	// return true if the object was contained in this container
	// false otherwise
	virtual bool remove(grob *);


	// ----------  Clear this container (remove all contents) ----------------
	virtual void clear();


	// ---------------- Add a grob into this container -----------------------
	virtual void add(grob *g);

	// --------------- Find an item by name ----------------------------------
	virtual grob* find(const var &name);

        // -------------- Find if an item is within this container  --------------
        bool find(const grob *p);


	bool modified() const;
	void modified(bool f);

	void cd();
	virtual void cd_specific() = 0;

	void print_hierarchy(int i=0);

	bool empty() const;


        template <typename T>
        T *get_last() const
        {
            for(int i=content_.size()-1; i>=0; --i)
            {
                if(T *t = dynamic_cast<T*>(content_[i])) return t;
            }
            return 0;
        }
    };

}

#ifdef __MAKECINT__
#pragma link off function blop::container::prepare_for_draw();
#endif

#endif
