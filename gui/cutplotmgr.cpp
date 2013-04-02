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
#include "cutplotmgr.h"

CutPlotMgr::CutPlotMgr(QCustomPlot *plot) : plot_(plot)
{
	plot_->clearPlottables();

	plot_->xAxis->setLabel("X");
	plot_->yAxis->setLabel("Y");
	plot_->xAxis->setRange(-1., 1.);
	plot_->yAxis->setRange(-1., 1.);

	plot_->setRangeDrag(Qt::Horizontal | Qt::Vertical);
	plot_->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}
