/*
 * (C) Copyright 2015 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
#define WIN32_LEAN_AND_MEAN // https://groups.google.com/forum/#!topic/boost-list/TKNG4U5UDU0

#include "simdialog.h"
#include "ui_simulator_dialog.h"
#include "foamcutapp.hpp"
#include <QTimer>
#include <Simulator.hpp>

SimDialog::SimDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::SimDialog())
{
    ui->setupUi(this);
	auto app = FoamcutApp::instance();

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
	ui->plot->replot();

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(500);

	auto &sim = stepper::device::Simulator::instance();
	auto simLimits = sim.limits();

	// set right side limits to be the same as left side
	simLimits[stepper::device::StepDir::Z_AXIS].low_ = simLimits[stepper::device::StepDir::X_AXIS].low_;
	simLimits[stepper::device::StepDir::Z_AXIS].high_ = simLimits[stepper::device::StepDir::X_AXIS].high_;
	simLimits[stepper::device::StepDir::U_AXIS].low_ = simLimits[stepper::device::StepDir::Y_AXIS].low_;
	simLimits[stepper::device::StepDir::U_AXIS].high_ = simLimits[stepper::device::StepDir::Y_AXIS].high_;
	sim.limits(simLimits);

	double xSize = app->xStepSize();
	double ySize = app->yStepSize();
	QLineEdit *le;

	le = ui->xLowerLimit_edit;
	le->setValidator(new QDoubleValidator());
	le->setText(QString::number(xSize * (double)simLimits[stepper::device::StepDir::X_AXIS].low_));
	connect(le, &QLineEdit::editingFinished, this, &SimDialog::my_limit_editingFinished);

	le = ui->xUpperLimit_edit;
	le->setValidator(new QDoubleValidator());
	le->setText(QString::number(xSize * (double)simLimits[stepper::device::StepDir::X_AXIS].high_));
	connect(le, &QLineEdit::editingFinished, this, &SimDialog::my_limit_editingFinished);

	le = ui->yLowerLimit_edit;
	le->setValidator(new QDoubleValidator());
	le->setText(QString::number(ySize * (double)simLimits[stepper::device::StepDir::Y_AXIS].low_));
	connect(le, &QLineEdit::editingFinished, this, &SimDialog::my_limit_editingFinished);

	le = ui->yUpperLimit_edit;
	le->setValidator(new QDoubleValidator());
	le->setText(QString::number(ySize * (double)simLimits[stepper::device::StepDir::Y_AXIS].high_));
	connect(le, &QLineEdit::editingFinished, this, &SimDialog::my_limit_editingFinished);
}

SimDialog::~SimDialog()
{
}

void SimDialog::update()
{
	auto app = FoamcutApp::instance();
	stepper::device::Simulator &sim = stepper::device::Simulator::instance();
	boost::lock_guard<boost::mutex> simPosGuard(sim.positionMutex());
	double xStepSize = app->xStepSize();
	double yStepSize = app->yStepSize();
	const std::deque<stepper::device::Position> &posLog = sim.positionLog();
	auto lCurve = static_cast<QCPCurve*>(ui->plot->plottable(LEFT_CURVE));
	auto rCurve = static_cast<QCPCurve*>(ui->plot->plottable(RIGHT_CURVE));
	size_t begin = rCurve->data()->size();
	for (size_t i = begin; i < posLog.size(); ++i) {
		const stepper::device::Position &p = posLog[i];
		double x = (double)p.pos_[0] * xStepSize;
		double y = (double)p.pos_[1] * yStepSize;
		lCurve->addData(x, y);
		x = (double)p.pos_[2] * xStepSize;
		y = (double)p.pos_[3] * yStepSize;
		rCurve->addData(x, y);
	}
	ui->plot->replot();
}

void SimDialog::on_clear_button_clicked()
{
	auto &sim = stepper::device::Simulator::instance();
	sim.clearLog();

    auto lCurve = static_cast<QCPCurve*>(ui->plot->plottable(LEFT_CURVE));
	auto rCurve = static_cast<QCPCurve*>(ui->plot->plottable(RIGHT_CURVE));
	lCurve->clearData();
	rCurve->clearData();
    update();
}

void SimDialog::my_limit_editingFinished()
{
	auto app = FoamcutApp::instance();
	auto &sim = stepper::device::Simulator::instance();
	auto limits = sim.limits();
	double xSize = app->xStepSize();
	double ySize = app->yStepSize();


	limits[stepper::device::StepDir::X_AXIS].low_ =  static_cast<int>(ui->xLowerLimit_edit->text().toDouble() / xSize);
	limits[stepper::device::StepDir::X_AXIS].high_ = static_cast<int>(ui->xUpperLimit_edit->text().toDouble() / xSize);
	limits[stepper::device::StepDir::Y_AXIS].low_ =  static_cast<int>(ui->yLowerLimit_edit->text().toDouble() / ySize);
	limits[stepper::device::StepDir::Y_AXIS].high_ = static_cast<int>(ui->yUpperLimit_edit->text().toDouble() / ySize);

	// set right side limits to be the same as left side
	limits[stepper::device::StepDir::Z_AXIS].low_ = limits[stepper::device::StepDir::X_AXIS].low_;
	limits[stepper::device::StepDir::Z_AXIS].high_ = limits[stepper::device::StepDir::X_AXIS].high_;
	limits[stepper::device::StepDir::U_AXIS].low_ = limits[stepper::device::StepDir::Y_AXIS].low_;
	limits[stepper::device::StepDir::U_AXIS].high_ = limits[stepper::device::StepDir::Y_AXIS].high_;

	sim.limits(limits);
}

void SimDialog::done(int result)
{
	auto app = FoamcutApp::instance();
	app->stopSimulator();
	QDialog::done(result);
}
