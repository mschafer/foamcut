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
#ifndef FIXEDARPLOT_H
#define FIXEDARPLOT_H

#include "qcustomplot.h"

class FixedARPlot : public QCustomPlot
{
    Q_OBJECT
    
public:
	explicit FixedARPlot(QWidget *parent = 0);
	virtual ~FixedARPlot();

	Q_SLOT void rescaleAxesFixedAR(bool onlyVisiblePlottables = false);

protected:
	virtual void wheelEvent(QWheelEvent *event);
	virtual void resizeEvent(QResizeEvent *event);

};

#endif
