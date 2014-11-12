#include "fixedarplot.h"

FixedARPlot::FixedARPlot(QWidget *parent) : QCustomPlot(parent)
{
}

FixedARPlot::~FixedARPlot()
{
}

void FixedARPlot::rescaleAxesFixedAR(bool onlyVisiblePlottables)
{
	// use base class to rescale so everything fits
	QCustomPlot::rescaleAxes(onlyVisiblePlottables);

	// then enlarge the range of one axis to correct aspect ratio
	QSize s = axisRect()->size();
	double xpp = xAxis->range().size() / (double)s.width();
	double ypp = yAxis->range().size() / (double)s.height();

	if (xpp > ypp) {
		yAxis->scaleRange(xpp/ypp, yAxis->range().center());
	} else {
		xAxis->scaleRange(ypp/xpp, xAxis->range().center());
	}
}

void FixedARPlot::wheelEvent(QWheelEvent *event) {
	// zooming should preserve the aspect ration since the scale factor
	// in each direction is identical so just use the base class implementation.
	QCustomPlot::wheelEvent(event);
}

void FixedARPlot::resizeEvent(QResizeEvent *event)
{
	QSize ns = event->size();
	QSize os = event->oldSize();

	if (os.width() > 0 && os.height() > 0) {
		// approximately preserve the length per pixel in both directions
		double lpp = xAxis->range().size() / (double)os.width();
		double nxWidth = lpp * (double)ns.width();
		double xRatio = nxWidth / xAxis->range().size();
		xAxis->scaleRange(xRatio, xAxis->range().center());

		double nyWidth = lpp * (double)ns.height();
		double yRatio = nyWidth / yAxis->range().size();
		yAxis->scaleRange(yRatio, yAxis->range().center());
	}

	QCustomPlot::resizeEvent(event);
}
