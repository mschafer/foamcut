
#define WIN32_LEAN_AND_MEAN // https://groups.google.com/forum/#!topic/boost-list/TKNG4U5UDU0

#include "simdialog.h"
#include "ui_simulator_dialog.h"
#include "foamcutapp.hpp"
#include <QTimer>
#include <Simulator.hpp>
#include <settings.hpp>

SimDialog::SimDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::SimDialog())
{
    ui->setupUi(this);

	ui->plot->clearPlottables();
	ui->plot->xAxis->setLabel("X");
	ui->plot->yAxis->setLabel("Y");
	ui->plot->xAxis->setRange(-1., 30);
	ui->plot->yAxis->setRange(-1., 15.);

	ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui->plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
	ui->plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);

	QCPCurve *curve;
	QPen pen;
	QCPScatterStyle scatter(QCPScatterStyle::ScatterShape::ssDot, 6.);

	// left
	curve = new QCPCurve(ui->plot->xAxis, ui->plot->yAxis);
	curve->setName("left");
	//curve->setLineStyle(QCPCurve::LineStyle::lsNone);
	//curve->setScatterStyle(scatter);
	pen.setColor(QColor("red"));
	curve->setPen(pen);
	curve->addData(0., 0.);
	ui->plot->addPlottable(curve);

	// right
	curve = new QCPCurve(ui->plot->xAxis, ui->plot->yAxis);
	curve->setName("right");
	//curve->setLineStyle(QCPCurve::LineStyle::lsNone);
	//curve->setScatterStyle(scatter);
	pen.setColor(QColor("blue"));
	curve->setPen(pen);
	curve->addData(0., 0.);
	ui->plot->addPlottable(curve);

	ui->plot->legend->setVisible(true);
	connect(ui->plot, SIGNAL(beforeReplot()), this, SLOT(beforeReplot()));
	ui->plot->replot();

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(1000);
}

SimDialog::~SimDialog()
{
    delete ui;
}

void SimDialog::update()
{
	stepper::device::Simulator &sim = stepper::device::Simulator::instance();
	double xStepSize = foamcut::Settings::xStepSize();
	double yStepSize = foamcut::Settings::yStepSize();
	auto posLog = sim.positionLog();
	auto lCurve = static_cast<QCPCurve*>(ui->plot->plottable(LEFT_CURVE));
	auto rCurve = static_cast<QCPCurve*>(ui->plot->plottable(RIGHT_CURVE));
	size_t begin = rCurve->data()->size();
	for (size_t i = begin; i < posLog.size(); ++i) {
		stepper::device::Position &p = posLog[i];
		double x = (double)p.pos_[0] * xStepSize;
		double y = (double)p.pos_[1] * yStepSize;
		lCurve->addData(x, y);
		x = (double)p.pos_[2] * xStepSize;
		y = (double)p.pos_[3] * yStepSize;
		rCurve->addData(x, y);
	}
	ui->plot->replot();
}

void SimDialog::beforeReplot()
{
	QCustomPlot *plot = ui->plot;
	QSize s = plot->axisRect()->size();
	double newArea = (double)s.height() * (double)s.width();
	double pixelAR = (double)s.width() / (double)s.height();
	QCPRange xr = plot->xAxis->range();
	QCPRange yr = plot->yAxis->range();
	double axisAR = xr.size() / yr.size();

	double xRangeScale = sqrt(pixelAR / axisAR);

	// scale xRange
	double newSize = xr.size() * xRangeScale;
	double lower = xr.center() - (newSize / 2.);
	double upper = xr.center() + (newSize / 2.);
	plot->xAxis->setRange(QCPRange(lower, upper));

	// scale yRange
	newSize = yr.size() / xRangeScale;
	lower = yr.center() - (newSize / 2.);
	upper = yr.center() + (newSize / 2.);
	plot->yAxis->setRange(QCPRange(lower, upper));
}
