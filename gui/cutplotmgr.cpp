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

	plot_->setRangeDrag(Qt::Horizontal | Qt::Vertical);
	plot_->setRangeZoom(Qt::Horizontal | Qt::Vertical);

	QCPCurve *curve;
	QPen pen;

	// ROOT_BASE_CURVE
	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	curve->setPen(pen);
	plot_->addPlottable(curve);

	// TIP_BASE_CURVE
	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	pen.setColor(QColor("darkMagenta"));
	curve->setPen(pen);
	plot_->addPlottable(curve);

	// ROOT_PART_CURVE
	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	pen.setColor(QColor("darkBlue"));
	curve->setPen(pen);
	plot_->addPlottable(curve);

	// TIP_PART_CURVE
	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	pen.setColor(QColor("magenta"));
	curve->setPen(pen);
	plot_->addPlottable(curve);

	// LEFT_FRAME_CURVE
	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	pen.setColor(QColor("blue"));
	curve->setPen(pen);
	plot_->addPlottable(curve);

	// RIGHT_FRAME_CURVE
	curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
	pen.setColor(QColor("green"));
	curve->setPen(pen);
	plot_->addPlottable(curve);
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

	if (root_.get() != nullptr) {
		cdm = ShapePlotMgr::lineFit(root_);
		curve = (QCPCurve*)(plot_->plottable(ROOT_BASE_CURVE));
		curve->setData(cdm, false);
		if (!root_->name().empty()) rootName = root_->name();
		curve->setName(QString::fromStdString(rootName + " base"));
	}

	if (tip_.get() != nullptr) {
		cdm = ShapePlotMgr::lineFit(tip_);
		curve = (QCPCurve*)(plot_->plottable(TIP_BASE_CURVE));
		curve->setData(cdm, false);
		if (!tip_->name().empty()) tipName = tip_->name();
		curve->setName(QString::fromStdString(tipName + " base"));
	}

	if (part_.get() != nullptr) {
		auto dms = surfacePoints(part_);
		curve = (QCPCurve*)(plot_->plottable(ROOT_PART_CURVE));
		curve->setData(dms.first, false);
		curve->setName(QString::fromStdString(rootName + " part"));
		curve = (QCPCurve*)(plot_->plottable(TIP_PART_CURVE));
		curve->setData(dms.second, false);
		curve->setName(QString::fromStdString(tipName + " part"));
	}

	if (frame_.get() != nullptr) {
		auto dms = surfacePoints(frame_);
		curve = (QCPCurve*)(plot_->plottable(LEFT_FRAME_CURVE));
		curve->setData(dms.first, false);
		curve->setName("left frame");
		curve = (QCPCurve*)(plot_->plottable(RIGHT_FRAME_CURVE));
		curve->setData(dms.second, false);
		curve->setName("right frame");
	}

	plot_->replot();
}
