#include "mcontainer.h"
#include "warning.h"

namespace blop
{
    int mcontainer::ind(int i, int j) const
    {
	--i;
	--j;
	if(i < 0) i = 0;
	if(i >= nx_) i = nx_-1;
	if(j < 0) j = 0;
	if(j >= ny_) j = ny_-1;
	int result = i * ny_ + j;
	return result;
    }

    mcontainer &mcontainer::direction(int i)
    {
	direction_ = i;
        cd_first();
	return *this;
    }

    void mcontainer::init_(int xdim, int ydim, int dir)
    {
	direction_ = dir;

	nx_ = xdim;
	ny_ = ydim;

	current_ix_ = 1;
	current_iy_ = 1;
    }

    mcontainer::mcontainer(int xdim, int ydim, int dir)
    {
	init_(xdim,ydim,dir);
    }

    mcontainer::mcontainer() : direction_(0), nx_(0), ny_(0), current_ix_(0), current_iy_(0)
    {
    }

    void mcontainer::get_gridsize_(int n, int &xdim, int &ydim)
    {
	xdim = ydim = std::floor(std::sqrt((double)n));
	while(xdim*ydim<n)
	{
	    ++ydim;
	    if(xdim*ydim>=n) break;
	    ++xdim;
	}
    }

    mcontainer &mcontainer::cd_first()
    {
	if     (direction_&right_down)   cd(1,  ny_);
	else if(direction_&right_up  )   cd(1,  1  );
	else if(direction_&left_down )   cd(nx_,ny_);
	else if(direction_&left_up   )   cd(nx_,1  );
	else
	{
	    warning::print("No direction is set (should not happen)","mcontainer::cd_first()");
	    cd(1,  1  );
	}
	return *this;
    }

    bool mcontainer::cd_next()
    {
	int cur_ix = current_ix_;
	int cur_iy = current_iy_;

	if( (direction_ & right_down) || (direction_ & right_up) )
	{
	    if(++cur_ix > nx_)
	    {
		cur_ix = 1;
		if(direction_ & right_down)
		{
		    if(--cur_iy <= 0)
		    {
			if( (direction_&jump) == 0) return false;
			cur_iy = ny_;
		    }
		}
		else
		{
		    if(++cur_iy > ny_)
		    {
			if( (direction_&jump) == 0) return false;
			cur_iy = 1;
		    }
		}
	    }
	}

	else if( (direction_ & left_down) || (direction_ & left_up) )
	{
	    if(--cur_ix <= 0)
	    {
		cur_ix = nx_;
		if(direction_ & left_down)
		{
		    if(--cur_iy <= 0)
		    {
			if( (direction_&jump) == 0) return false;
			cur_iy = ny_;
		    }
		}
		else
		{
		    if(++cur_iy > ny_)
		    {
			if( (direction_&jump) == 0) return false;
			cur_iy = 1;
		    }
		}
	    }
	}

	else if( (direction_ & down_left) || (direction_ & down_right) )
	{
	    if(--cur_iy <= 0)
	    {
		cur_iy = ny_;
		if(direction_ & down_left)
		{
		    if(--cur_ix <= 0)
		    {
			if( (direction_&jump) == 0) return false;
			cur_ix = nx_;
		    }
		}
		else
		{
		    if(++cur_ix > nx_)
		    {
			if( (direction_&jump) == 0) return false;
			cur_ix = 1;
		    }
		}
	    }
	}

	else if( (direction_ & up_left) || (direction_ & up_right) )
	{
	    if(++cur_iy > ny_)
	    {
		cur_iy = 1;
		if(direction_ & up_left)
		{
		    if(--cur_ix <= 0)
		    {
			if( (direction_&jump) == 0) return false;
			cur_ix = nx_;
		    }
		}
		else
		{
		    if(++cur_ix > nx_)
		    {
			if( (direction_&jump) == 0) return false;
			cur_ix = 1;
		    }
		}
	    }
	}

	else
	{
	    warning::print("Bad direction","mcontainer::cd_next()");
	    return false;
	}

	cd(cur_ix, cur_iy);
	return true;
    }



    mcontainer &mcontainer::cd(int i, int j)
    {
	current_ix_ = i;
	current_iy_ = j;
	cd_to_sub_(ind(i,j));
	return *this;
    }

    mcontainer  &mcontainer::cd(int i)
    {
	if(i<1 || nx_*ny_<i)
	{
	    warning::print("Subcontainer number is out of range",
			   "mcontainer::cd(" & var(i) & ")");
	    return *this;
	}
	--i;
	if     (direction_&right_down  ) cd(1+i%nx_, ny_-i/nx_);
	else if(direction_&down_right  ) cd(1+i/ny_, ny_-i%ny_);
	else if(direction_&right_up    ) cd(1+i%nx_, 1+i/nx_  );
	else if(direction_&up_right    ) cd(1+i/ny_, 1+i%ny_  );
	else if(direction_&left_down   ) cd(nx_-i%ny_, ny_-i/ny_);
	else if(direction_&down_left   ) cd(nx_-i/ny_, ny_-i%ny_);
	else if(direction_&left_up     ) cd(nx_-i%ny_, 1+i/ny_  );
	else if(direction_&up_left     ) cd(nx_-i/ny_, 1+i%ny_  );
	else warning::print("Bad direction of mcontainer, this should not happen");
	return *this;
    }


    

}
