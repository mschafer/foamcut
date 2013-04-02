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
 * Displays a single shape on a plot.
 */
class CutPlotMgr : public boost::noncopyable
{
public:
	enum {
		LINE_CURVE = 0,
		BREAK_CURVE = 1
	};

	CutPlotMgr(QCustomPlot *plot);

	//void update(foamcut::Shape::handle shape) { shape_ = shape; replot(); }

private:
	CutPlotMgr();
	void replot();

	QCustomPlot *plot_;
	foamcut::Shape::handle root_;
	foamcut::Shape::handle tip_;
	foamcut::RuledSurface::handle part_;
	foamcut::RuledSurface::handle cut_;
};

#endif
