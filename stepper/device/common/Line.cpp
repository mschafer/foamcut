#include "Line.hpp"
#include "Commands.hpp"

namespace stepper { namespace device {

Line::Line(int dx, int dy, int dz, int du, uint32_t delay)
{
	absd_[X_AXIS] = abs(dx);
	absd_[Y_AXIS] = abs(dy);
	absd_[Z_AXIS] = abs(dz);
	absd_[U_AXIS] = abs(du);

    dir_ = 0;
    dir_ |= (dx < 0) ? 0 : X_DIR;
    dir_ |= (dy < 0) ? 0 : Y_DIR;
    dir_ |= (dz < 0) ? 0 : Z_DIR;
    dir_ |= (du < 0) ? 0 : U_DIR;

    maxd_ = 0;
    for (int i=0; i<4; ++i) {
    	maxd_ = absd_[i] > maxd_ ? absd_[i] : maxd_;
    	e_[i] = 0;
    }
    // dither delay between steps to get exactly the right delay
    if (maxd_ != 0) {
    	stepDelay_ = delay / maxd_;
    } else {
    	stepDelay_ = delay;
    }
    absd_[4] = delay - (stepDelay_ - maxd_);


}




}}
