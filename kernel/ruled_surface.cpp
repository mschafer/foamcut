/*
 * (C) Copyright 2013 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */

#include <stdexcept>
#include <boost/assert.hpp>
#include "ruled_surface.hpp"
#include "stepper_info.hpp"
#include "Script.hpp"

namespace foamcut {

RuledSurface::RuledSurface(const Shape &lShape, const Shape &rShape,
		double leftZ, double span, double eps) :
    		lZ_(leftZ), span_(span)
{
	if (lShape.segmentCount() != rShape.segmentCount()) {
		throw std::runtime_error("RuledSurface::RuledSurface Left and right shapes must have the same number of segments");
	}

	// convert the pair of shapes to a pair of poly lines with aligned sample points
	size_t nseg = lShape.segmentCount();
	for (size_t iseg=0; iseg<nseg; ++iseg) {
		std::pair<double, double> lsLimits;
		lsLimits.first = lShape.breakS(iseg);
		lsLimits.second = lShape.breakS(iseg+1);
		double dls = lsLimits.second - lsLimits.first;
		std::pair<double, double> rsLimits;
		rsLimits.first = rShape.breakS(iseg);
		rsLimits.second = rShape.breakS(iseg+1);
		double drs = rsLimits.second - rsLimits.first;

		bool segEnd = false;
		double ls = lsLimits.first;
		double rs = rsLimits.first;
		while (!segEnd) {
			Shape::Point lp = lShape.evaluate(ls, iseg);
			Shape::Point rp = rShape.evaluate(rs, iseg);
			lx_.push_back(lp.x); ly_.push_back(lp.y);
			rx_.push_back(rp.x); ry_.push_back(rp.y);

			double lsNext = lShape.fitLineSegment(ls, eps);
			double rsNext = rShape.fitLineSegment(rs, eps);

			// advance by the largest length fraction that satisfies the tolerance on both sides.
			double lsFrac = (lsNext - lsLimits.first) / dls;
			double rsFrac = (rsNext - rsLimits.first) / drs;
			double sFrac = std::min(lsFrac, rsFrac);
			ls = sFrac*dls + lsLimits.first;
			rs = sFrac*drs + rsLimits.first;

			// match endpoint exactly when within eps. on either side
			if (fabs(ls - lsLimits.second) <= eps || (fabs(rs - rsLimits.second) <= eps)) {
				segEnd = true;
			}
		}

		// last point
		auto lastPoint = lShape.breakPoint(iseg+1);
		lx_.push_back(lastPoint.first); ly_.push_back(lastPoint.second);
		lastPoint = rShape.breakPoint(iseg+1);
		rx_.push_back(lastPoint.first); ry_.push_back(lastPoint.second);
	}

}

RuledSurface::~RuledSurface()
{

}

RuledSurface::handle
RuledSurface::translateZ(double zOffset) const
{
	handle ret(new RuledSurface(*this));
	ret->lZ_ += zOffset;
	return ret;
}

RuledSurface::handle
RuledSurface::interpolateZ(double newlZ, double newrZ) const
{
	size_t n = lx_.size();

	std::vector<double> nlx(n);
	std::vector<double> nly(n);
	std::vector<double> nrx(n);
	std::vector<double> nry(n);

	double lt = (newlZ - lZ_) / (span_);
	double rt = (newrZ - lZ_) / (span_);

	for (size_t i=0; i<n; i++) {
		nlx[i] = (1. - lt) * lx_[i] + lt * rx_[i];
		nly[i] = (1. - lt) * ly_[i] + lt * ry_[i];
		nrx[i] = (1. - rt) * lx_[i] + rt * rx_[i];
		nry[i] = (1. - rt) * ly_[i] + rt * ry_[i];
	}

	handle ret(new RuledSurface());
	ret->lx_.swap(nlx);
	ret->ly_.swap(nly);
	ret->rx_.swap(nrx);
	ret->ry_.swap(nry);
	ret->lZ_ = newlZ;
	ret->span_ = newrZ - newlZ;
	ret->time_ = time_;

	return ret;
}

void RuledSurface::setTime(double speed)
{
	time_.clear();
	time_.resize(lx_.size());
	time_[0] = 0.;
	for (size_t i=1; i<lx_.size(); ++i) {
		double ld = hypot(lx_[i]-lx_[i-1], ly_[i]-ly_[i-1]);
		double rd = hypot(rx_[i]-rx_[i-1], ry_[i]-ry_[i-1]);
		if (ld > rd) {
			time_[i] = time_[i-1] + (ld / speed);
		} else {
			time_[i] = time_[i-1] + (rd / speed);
		}
	}
}

double RuledSurface::duration() const
{
	if (time_.empty()) {
		throw std::runtime_error("RuledSurface::duaration times haven't been set yet");
	}

	return time_.back();
}

RuledSurface::Point RuledSurface::interpolateTime(double t) const
{
	if (time_.empty()) {
		throw std::runtime_error("RuledSurface::duaration times haven't been set yet");
	}

	if (t < 0 || t > time_.back()) {
		throw std::range_error("RuledSurface::interpolate time out of range");
	}

	size_t i = 1;
	while (t > time_[i]) ++i;

	Point ret;
	double dtf = (t - time_[i-1]) / (time_[i] - time_[i-1]);
	ret.lx_ = lx_[i-1] + (lx_[i] - lx_[i-1]) * dtf;
	ret.ly_ = ly_[i-1] + (ly_[i] - ly_[i-1]) * dtf;
	ret.rx_ = rx_[i-1] + (rx_[i] - rx_[i-1]) * dtf;
	ret.ry_ = ry_[i-1] + (ry_[i] - ry_[i-1]) * dtf;
	return ret;
}

stepper::Script::handle RuledSurface::generateScript(const StepperInfo &sInfo)
{
	stepper::Script::handle ret(new stepper::Script());

	double xsc = 1. / sInfo.xStepSize_;
	double ysc = 1. / sInfo.yStepSize_;

	// calculate relative steps and lines from coordinates
	double skipTime = 0.;
	int last_lx, last_ly, last_rx, last_ry;
	last_lx = static_cast<int>(lx_[0] / sInfo.xStepSize_);
	last_ly = static_cast<int>(ly_[0] / sInfo.yStepSize_);
	last_rx = static_cast<int>(rx_[0] / sInfo.xStepSize_);
	last_ry = static_cast<int>(ry_[0] / sInfo.yStepSize_);

	for (size_t i=1; i<lx_.size(); ++i) {
		int16_t dlxi = static_cast<int16_t>((lx_[i] * xsc) - (double)last_lx);
		int16_t dlyi = static_cast<int16_t>((ly_[i] * ysc) - (double)last_ly);
		int16_t drxi = static_cast<int16_t>((rx_[i] * xsc) - (double)last_rx);
		int16_t dryi = static_cast<int16_t>((ry_[i] * ysc) - (double)last_ry);
		int maxd = std::max(abs(dlxi), abs(dlyi));
		maxd = std::max(maxd, abs(drxi));
		maxd = std::max(maxd, abs(dryi));
		if (maxd == 0) {
			// points are less than one step apart so skip
			skipTime = time_[i] - time_[i-1];
			continue;
		} else if (maxd == 1) {
			// individual step
			stepper::device::StepDir s;
			s.xStepDir(dlxi);
			s.yStepDir(dlyi);
			s.zStepDir(drxi);
			s.uStepDir(dryi);
			ret->addStep(s, time_[i]-time_[i-1]+skipTime);
			skipTime = 0.;
		} else {
			// line
			ret->addLine(dlxi, dlyi, drxi, dryi, time_[i]-time_[i-1]+skipTime);
			skipTime = 0.;
		}
		last_lx += dlxi;
		last_ly += dlyi;
		last_rx += drxi;
		last_ry += dryi;
	}
	return ret;
}

}
