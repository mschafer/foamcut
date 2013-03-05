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
		std::pair<double, double> lsLimits = lShape.segmentS(iseg);
		double dls = lsLimits.second - lsLimits.first;
		std::pair<double, double> rsLimits = rShape.segmentS(iseg);
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
			double lsFrac = (lsNext - ls) / dls;
			double rsFrac = (rsNext - rs) / drs;
			double sFrac = std::min(lsFrac, rsFrac);
			ls = ls + sFrac*dls;
			rs = rs + sFrac*dls;

			// match endpoint exactly when within eps.
			if (fabs(ls - lsLimits.second) <= eps || (fabs(rs - rsLimits.second) <= eps)) {
				BOOST_ASSERT_MSG((fabs(ls - lsLimits.second) <= eps && (fabs(rs - rsLimits.second) <= eps)),
						"RuledSurface::RuledSurface discretization didn't reach segment end simultaneously for left and right");
				segEnd = true;
			}
		}

		// last point
		lx_.push_back(lShape.x().back()); ly_.push_back(lShape.y().back());
		rx_.push_back(rShape.x().back()); ry_.push_back(rShape.y().back());
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

}
