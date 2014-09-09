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

#include "ruled_surface.hpp"
#include <stdexcept>
#include <boost/assert.hpp>
#include <Script.hpp>

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

	return ret;
}

void RuledSurface::setTime(double speed)
{
	time_.clear();
	time_.reserve(lx_.size());
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

stepper::Script::handle RuledSurface::generateScript(const StepperInfo &stepper)
{
	stepper::Script::handle ret(new stepper::Script());

	// convert distances to steps
	std::vector<int> lxi(lx_.size());
	std::vector<int> lyi(ly_.size());
	std::vector<int> rxi(rx_.size());
	std::vector<int> ryi(ry_.size());
	for (size_t i=0; i<lx_.size(); ++i) {
		lxi[i] = static_cast<int>(lx_[i] / stepper.xStepSize_);
		lyi[i] = static_cast<int>(ly_[i] / stepper.yStepSize_);
		rxi[i] = static_cast<int>(rx_[i] / stepper.xStepSize_);
		ryi[i] = static_cast<int>(ry_[i] / stepper.yStepSize_);
	}

	// calculate relative steps and lines from coordinates
	double skipTime = 0.;
	for (size_t i=1; i<lxi.size(); ++i) {
		int16_t dlxi = static_cast<int16_t>(lxi[i] - lxi[i-1]);
		int16_t dlyi = static_cast<int16_t>(lyi[i] - lyi[i-1]);
		int16_t drxi = static_cast<int16_t>(rxi[i] - rxi[i-1]);
		int16_t dryi = static_cast<int16_t>(ryi[i] - ryi[i-1]);
		int mind = std::min(abs(dlxi), abs(dlyi));
		mind = std::min(mind, abs(drxi));
		mind = std::min(mind, abs(dryi));
		if (mind == 0) {
			// points are less than one step apart so skip
			skipTime = time_[i] - time_[i-1];
			continue;
		} else if (mind == 1) {
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
	}
	return ret;
}

}
