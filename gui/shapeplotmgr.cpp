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
#include "shapeplotmgr.h"

ShapePlotMgr::ShapePlotMgr(QCustomPlot *plot) : plot_(plot)
{
	plot_->clearPlottables();
	QCPCurve *curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	plot_->addPlottable(curve);

	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	curve->setLineStyle(QCPCurve::lsNone);
	curve->setScatterStyle(QCPScatterStyle::ssDiamond);
	plot_->addPlottable(curve);

	plot_->xAxis->setLabel("X");
	plot_->yAxis->setLabel("Y");
	plot_->xAxis->setRange(-1., 1.);
	plot_->yAxis->setRange(-1., 1.);

	plot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	plot_->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
	plot_->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);

	connect(plot_, SIGNAL(beforeReplot()), this, SLOT(beforeReplot()));
}

QCPCurveDataMap *ShapePlotMgr::lineFit(const foamcut::Shape::handle shape)
{
	QCPCurveDataMap *dataMap = new QCPCurveDataMap();
	double s = 0.;
	auto p0 = shape->evaluate(s);
	dataMap->insert(s, QCPCurveData(s, p0.x, p0.y));
	double sMax = shape->s().back();
	while (s < .99999 * sMax) {
		/// \todo use step size to correctly set tolerance
		double s1 = shape->fitLineSegment(s, .001);
		auto p1 = shape->evaluate(s1);
		dataMap->insert(s1, QCPCurveData(s1, p1.x, p1.y));
		s = s1;
	}
	return dataMap;
}

QCPCurveDataMap *ShapePlotMgr::breakPoints(const foamcut::Shape::handle shape)
{
	QCPCurveDataMap *dataMap = new QCPCurveDataMap();
	size_t nseg = shape->segmentCount();
	for (size_t i=0; i<nseg; ++i) {
		std::pair<double, double> p = shape->breakPoint(i);
		double s = shape->breakS(i);
		QCPCurveData d(s, p.first, p.second);
		dataMap->insert(s, d);
	}
	return dataMap;
}

void ShapePlotMgr::replot()
{
	// convert to QCPCurveData and plot
	QCPCurve *curve = dynamic_cast<QCPCurve *>(plot_->plottable(LINE_CURVE));
	QCPCurveDataMap *dataMap = lineFit(shape_);
	curve->setData(dataMap, false);
	curve->rescaleAxes();

	curve = dynamic_cast<QCPCurve *>(plot_->plottable(BREAK_CURVE));
	dataMap = breakPoints(shape_);
	curve->setData(dataMap, false);

	plot_->replot();
}

void ShapePlotMgr::beforeReplot()
{
	QSize s = plot_->axisRect()->size();
	double pixelAR = (double)s.width() / (double)s.height();
	QCPRange xr = plot_->xAxis->range();
	QCPRange yr = plot_->yAxis->range();
	double axisAR = xr.size() / yr.size();

	// if the window is too wide for the current axes, make the plots x range bigger
	if (pixelAR > axisAR) {
		double newSize = xr.size() * pixelAR / axisAR;
		double lower = xr.center() - (newSize / 2.);
		double upper = xr.center() + (newSize / 2.);
		plot_->xAxis->setRange(QCPRange(lower, upper));
	}
	else {
		double newSize = yr.size() / (pixelAR / axisAR);
		double lower = yr.center() - (newSize / 2.);
		double upper = yr.center() + (newSize / 2.);
		plot_->yAxis->setRange(QCPRange(lower, upper));
	}
}

#if 0
BoundingBox bounds(const QCPCurveDataMap *dm)
{
	BoundingBox ret;
	QCPCurveData d = *(dm->begin());
	ret.xMin_ = ret.xMax_ = d.key;
	ret.yMin_ = ret.yMax_ = d.value;
	BOOST_FOREACH(QCPCurveData d, std::make_pair(dm->begin(), dm->end())) {
		ret.xMin_ = std::min(ret.xMin_, d.key);
		ret.xMax_ = std::max(ret.xMax_, d.key);
		ret.yMin_ = std::min(ret.yMin_, d.value);
		ret.yMax_ = std::max(ret.yMax_, d.value);
	}
	return ret;
}
#endif
