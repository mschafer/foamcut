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
#ifndef shapeplotmgr_h
#define shapeplotmgr_h

#include "shape.hpp"
#include "qcustomplot.h"

/**
 * Displays a single shape on a plot.
 */
class ShapePlotMgr : public QObject
{
	Q_OBJECT

public:
	enum {
		LINE_CURVE = 0,
		BREAK_CURVE = 1
	};

	ShapePlotMgr(QCustomPlot *plot);

	void update(foamcut::Shape::handle shape) { shape_ = shape; replot(); }

	static QCPCurveDataMap *lineFit(const foamcut::Shape::handle shape);
	static QCPCurveDataMap *breakPoints(const foamcut::Shape::handle shape);

private slots:
	void beforeReplot();

private:
	ShapePlotMgr();
	void replot();

	QCustomPlot *plot_;
	foamcut::Shape::handle shape_;
};

#endif
