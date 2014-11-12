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
