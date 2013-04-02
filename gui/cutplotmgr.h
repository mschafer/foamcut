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
#ifndef cutplotmgr_h
#define cutplotmgr_h

#include <boost/utility.hpp>
#include "shape.hpp"
#include "ruled_surface.hpp"
#include "qcustomplot.h"

/**
 * Displays a cut on a 2d plot.
 * Root and tip shapes (line fit)
 * Part surface.
 * Interpolated surface at frames.
 */
class CutPlotMgr : public boost::noncopyable
{
public:
	enum {
		ROOT_BASE_CURVE   = 0,
		TIP_BASE_CURVE    = 1,
		ROOT_PART_CURVE   = 2,
		TIP_PART_CURVE    = 3,
		LEFT_FRAME_CURVE  = 4,
		RIGHT_FRAME_CURVE = 5
	};

	CutPlotMgr(QCustomPlot *plot);

	void update(foamcut::Shape::handle root, foamcut::Shape::handle tip,
			foamcut::RuledSurface::handle part, foamcut::RuledSurface::handle frame) {
		root_ = root;
		tip_ = tip;
		part_ = part;
		frame_ = frame;
		replot();
	}

	static std::pair<QCPCurveDataMap*, QCPCurveDataMap*> surfacePoints(const foamcut::RuledSurface::handle);

private:
	CutPlotMgr();
	void replot();

	QCustomPlot *plot_;
	foamcut::Shape::handle root_;
	foamcut::Shape::handle tip_;
	foamcut::RuledSurface::handle part_;
	foamcut::RuledSurface::handle frame_;
};

#endif
