
#include <QDebug>
#include "movedialog.h"
#include "ui_movedialog.h"
#include "foamcutapp.hpp"
#include "settings.hpp"
#include <StepDir.hpp>
#include <Host.hpp>

MoveDialog::MoveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveDialog)
{
    ui->setupUi(this);
	ui->speed_edit->setValidator(new QDoubleValidator());
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
	FoamcutApp *a = FoamcutApp::instance();
	a->host().home();
}

void MoveDialog::moveX(int sign)
{
	int16_t dx, dy, dz, du;
	dx = dy = dz = du = 0;
	double distance = ui->distance_spin->value();
	double stepSize = foamcut::Settings::xStepSize();
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
		if (ui->fast_radio->isChecked()) {
			duration = (double)stepCount / 1000.;
		}
		else {
			duration = distance / ui->speed_edit->text().toDouble();
		}

		FoamcutApp *a = FoamcutApp::instance();
		a->host().move(dx, dy, dz, du, duration);
	}
}

void MoveDialog::moveY(int sign)
{
	int16_t dx, dy, dz, du;
	dx = dy = dz = du = 0;
	double distance = ui->distance_spin->value();
	double stepSize = foamcut::Settings::yStepSize();
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
		if (ui->fast_radio->isChecked()) {
			duration = (double)stepCount / 1000.;
		}
		else {
			duration = distance / ui->speed_edit->text().toDouble();
		}

		FoamcutApp *a = FoamcutApp::instance();
		a->host().move(dx, dy, dz, du, duration);
	}
}