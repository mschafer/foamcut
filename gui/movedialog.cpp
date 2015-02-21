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
#include <QDebug>
#include "movedialog.h"
#include "ui_movedialog.h"
#include "foamcutapp.hpp"
#include <StepDir.hpp>
#include <QCursor>

class ScopedWaitCursor : private boost::noncopyable
{
public:
	ScopedWaitCursor(QApplication *app) : app_(app) { app->setOverrideCursor(QCursor(Qt::WaitCursor)); }
	~ScopedWaitCursor() { app_->restoreOverrideCursor(); }

private:
	QApplication *app_;
};


MoveDialog::MoveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveDialog)
{
	FoamcutApp *app = FoamcutApp::instance();

    ui->setupUi(this);
	ui->distance_spin->setValue(app->moveDistance());
	ui->fast_speed_radio->setChecked(app->moveFast());
}

MoveDialog::~MoveDialog()
{
    delete ui;
}

void MoveDialog::on_up_button_clicked()
{
	moveY(1);
}

void MoveDialog::on_left_button_clicked()
{
	moveX(-1);
}

void MoveDialog::on_right_button_clicked()
{
	moveX(1);
}

void MoveDialog::on_down_button_clicked()
{
	moveY(-1);
}

void MoveDialog::on_home_button_clicked()
{
	FoamcutApp *app = FoamcutApp::instance();

	ScopedWaitCursor waitCursor(app);
	
	double duration;
	if (ui->fast_speed_radio->isChecked()) {
		duration = (double) .001;
	}
	else {
		duration = std::min(app->xStepSize(), app->yStepSize()) / app->cutSpeed();
	}
	app->host().home();
}

void MoveDialog::done(int result)
{
	auto app = FoamcutApp::instance();
	double distance = ui->distance_spin->value();
	app->moveDistance(distance);
	app->moveFast(ui->fast_speed_radio->isChecked());
	QDialog::done(result);
}

void MoveDialog::moveX(int sign)
{
	auto app = FoamcutApp::instance();
	int16_t dx, dy, dz, du;
	dx = dy = dz = du = 0;
	double distance = ui->distance_spin->value();
	double stepSize = app->xStepSize();
	int stepCount = distance / stepSize;
	if (stepCount > INT16_MAX) {
		throw std::overflow_error("Move: step count is too large");
	}

	bool moving = false;
	if (ui->leftEnable_check->isChecked()) {
		dx = stepCount * sign;
		moving = true;
	}
	if (ui->rightEnable_check->isChecked()) {
		dz = stepCount * sign;
		moving = true;
	}

	if (moving) {
		double duration;
		if (ui->fast_speed_radio->isChecked()) {
			duration = (double)stepCount / (double)app->maxStepRate();
		}
		else {
			duration = distance / app->cutSpeed();
		}

		app->host().move(dx, dy, dz, du, duration);
	}
}

void MoveDialog::moveY(int sign)
{
	auto app = FoamcutApp::instance();
	int16_t dx, dy, dz, du;
	dx = dy = dz = du = 0;
	double distance = ui->distance_spin->value();
	double stepSize = app->yStepSize();
	int stepCount = distance / stepSize;
	if (stepCount > INT16_MAX) {
		throw std::overflow_error("Move: step count is too large");
	}

	bool moving = false;
	if (ui->leftEnable_check->isChecked()) {
		dy = stepCount * sign;
		moving = true;
	}
	if (ui->rightEnable_check->isChecked()) {
		du = stepCount * sign;
		moving = true;
	}

	if (moving) {
		double duration;
		if (ui->fast_speed_radio->isChecked()) {
			duration = (double)stepCount / (double)app->maxStepRate();
		}
		else {
			duration = distance / app->cutSpeed();
		}

		app->host().move(dx, dy, dz, du, duration);
	}
}
