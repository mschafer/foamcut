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

#include <QtGui/QPen>
#include "cutplotmgr.h"
#include "shapeplotmgr.h"

CutPlotMgr::CutPlotMgr(QCustomPlot *plot) : plot_(plot)
{
	plot_->clearPlottables();

	plot_->xAxis->setLabel("X");
	plot_->yAxis->setLabel("Y");
	plot_->xAxis->setRange(-1., 1.);
	plot_->yAxis->setRange(-1., 1.);

	plot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |QCP::iSelectLegend);
	plot_->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
	plot_->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
	plot_->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

	// connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
	connect(plot_, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
	connect(plot_, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
	connect(plot_, SIGNAL(beforeReplot()), this, SLOT(beforeReplot()));

	QCPCurve *curve;
	QPen pen;

	// ROOT_BASE_CURVE
	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	pen.setColor(QColor("darkBlue"));
	curve->setPen(pen);
	curve->setName("root base");
	plot_->addPlottable(curve);

	// TIP_BASE_CURVE
	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	pen.setColor(QColor("darkMagenta"));
	curve->setPen(pen);
	curve->setName("tip base");
	plot_->addPlottable(curve);

	// ROOT_PART_CURVE
	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	pen.setColor(QColor("blue"));
	curve->setPen(pen);
	curve->setName("root part");
	plot_->addPlottable(curve);

	// TIP_PART_CURVE
	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	pen.setColor(QColor("magenta"));
	curve->setPen(pen);
	curve->setName("tip part");
	plot_->addPlottable(curve);

	// LEFT_FRAME_CURVE
	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	pen.setColor(QColor("red"));
	curve->setPen(pen);
	curve->setName("left frame");
	plot_->addPlottable(curve);

	// RIGHT_FRAME_CURVE
	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	pen.setColor(QColor("green"));
	curve->setPen(pen);
	curve->setName("right frame");
	plot_->addPlottable(curve);

	plot_->legend->setVisible(true);
}

std::pair<QCPCurveDataMap*, QCPCurveDataMap*>
CutPlotMgr::surfacePoints(const foamcut::RuledSurface::handle surf)
{
	QCPCurveDataMap *leftdm  = new QCPCurveDataMap();
	QCPCurveDataMap *rightdm = new QCPCurveDataMap();

	const std::vector<double> &lx = surf->leftX();
	const std::vector<double> &ly = surf->leftY();
	const std::vector<double> &rx = surf->rightX();
	const std::vector<double> &ry = surf->rightY();
	size_t n = lx.size();
	for (size_t i=0; i<n; ++i) {
		double id = (double)i;
		leftdm->insert(id, QCPCurveData(id, lx[i], ly[i]));
		rightdm->insert(id, QCPCurveData(id, rx[i], ry[i]));
	}

	return std::make_pair(leftdm, rightdm);
}

void CutPlotMgr::replot()
{
	QCPCurveDataMap *cdm;
	QCPCurve *curve;

	std::string rootName = "root";
	std::string tipName = "tip";

	plot_->xAxis->setRange(0., .1);
	plot_->yAxis->setRange(0., .1);

	if (root_.get() != nullptr) {
		cdm = ShapePlotMgr::lineFit(root_);
		curve = (QCPCurve*)(plot_->plottable(ROOT_BASE_CURVE));
		curve->setData(cdm, false);
		if (!root_->name().empty()) rootName = root_->name();
		curve->setName(QString::fromStdString(rootName + " base"));
		curve->rescaleAxes(true);
	}

	if (tip_.get() != nullptr) {
		cdm = ShapePlotMgr::lineFit(tip_);
		curve = (QCPCurve*)(plot_->plottable(TIP_BASE_CURVE));
		curve->setData(cdm, false);
		if (!tip_->name().empty()) tipName = tip_->name();
		curve->setName(QString::fromStdString(tipName + " base"));
		curve->rescaleAxes(true);
	}

	if (part_.get() != nullptr) {
		auto dms = surfacePoints(part_);
		curve = (QCPCurve*)(plot_->plottable(ROOT_PART_CURVE));
		curve->setData(dms.first, false);
		curve->setName(QString::fromStdString(rootName + " part"));
		curve = (QCPCurve*)(plot_->plottable(TIP_PART_CURVE));
		curve->setData(dms.second, false);
		curve->setName(QString::fromStdString(tipName + " part"));
		curve->rescaleAxes(true);
	}

	if (frame_.get() != nullptr) {
		auto dms = surfacePoints(frame_);
		curve = (QCPCurve*)(plot_->plottable(LEFT_FRAME_CURVE));
		curve->setData(dms.first, false);
		curve = (QCPCurve*)(plot_->plottable(RIGHT_FRAME_CURVE));
		curve->setData(dms.second, false);
		curve->rescaleAxes(true);
	}

	// make room for legend to right side of plot
	QCPRange range = plot_->xAxis->range();
	range.upper = 1.4 * range.upper;
	plot_->xAxis->setRange(range);
	plot_->replot();
}

/** \todo change to selectionChangedByUser? */
void CutPlotMgr::mousePress() {
	// if an axis is selected, only allow the direction of that axis to be dragged
	// if no axis is selected, both directions may be dragged

	if (plot_->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
		plot_->axisRect()->setRangeDrag(plot_->xAxis->orientation());
	else if (plot_->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
		plot_->axisRect()->setRangeDrag(plot_->yAxis->orientation());
	else
		plot_->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
}

void CutPlotMgr::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
	// hide or show a curve by double clicking on its legend item.
	Q_UNUSED(legend)
	if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
	{
		QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
		bool visible = plItem->plottable()->visible();
		plItem->plottable()->setVisible(!visible);
	}
}

void CutPlotMgr::beforeReplot()
{
	QSize s = plot_->axisRect()->size();
	double newArea = (double)s.height() * (double)s.width();
	double pixelAR = (double)s.width() / (double)s.height();
	QCPRange xr = plot_->xAxis->range();
	QCPRange yr = plot_->yAxis->range();
	double axisAR = xr.size() / yr.size();

	double xRangeScale = sqrt(pixelAR / axisAR);

	// scale xRange
	double newSize = xr.size() * xRangeScale;
	double lower = xr.center() - (newSize / 2.);
	double upper = xr.center() + (newSize / 2.);
	plot_->xAxis->setRange(QCPRange(lower, upper));

	// scale yRange
	newSize = yr.size() / xRangeScale;
	lower = yr.center() - (newSize / 2.);
	upper = yr.center() + (newSize / 2.);
	plot_->yAxis->setRange(QCPRange(lower, upper));
}

