#include "setupdialog.h"
#include "ui_setupdialog.h"
#include <qdebug.h>
#include "foamcutapp.hpp"

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::SetupDialog)
{
    ui->setupUi(this);
	auto app = FoamcutApp::instance();
	ui->xStepSize_edit->setText(QString::number(app->xStepSize()));
	ui->yStepSize_edit->setText(QString::number(app->yStepSize()));
	ui->frameSeparation_edit->setText(QString::number(app->frameSeparation()));
	ui->maxStepRate_edit->setText(QString::number(app->maxStepRate()));
	ui->activeLowStep_check->setChecked(app->activeLowSteps());
	ui->xLeftReverse_check->setChecked(app->xLeftReverse());
	ui->xRightReverse_check->setChecked(app->xRightReverse());
	ui->yLeftReverse_check->setChecked(app->yLeftReverse());
	ui->yRightReverse_check->setChecked(app->yRightReverse());

	ui->xStepSize_edit->setValidator(new QDoubleValidator(0, 1.e9, 20));
	ui->yStepSize_edit->setValidator(new QDoubleValidator(0, 1.e9, 20));
	ui->frameSeparation_edit->setValidator(new QDoubleValidator());
	ui->maxStepRate_edit->setValidator(new QIntValidator(0, 5000));

	ui->port_combo->clear();
	ui->port_combo->clearEditText();
	ui->port_combo->blockSignals(true);
	bool foundPort = false;
	serialPorts_ = QSerialPortInfo::availablePorts();
	auto it = serialPorts_.begin();
	while (it < serialPorts_.end()) {
		ui->port_combo->addItem(it->portName());
		if (it->portName() == app->port()) {
			ui->port_combo->setCurrentIndex(ui->port_combo->count() - 1);
			foundPort = true;
		}
		++it;
	}
	ui->port_combo->addItem("none");
	if (!foundPort) {
		ui->port_combo->setCurrentIndex(ui->port_combo->count() - 1);
	}
	ui->port_combo->blockSignals(false);
}

SetupDialog::~SetupDialog()
{
}

void SetupDialog::accept()
{
	auto app = FoamcutApp::instance();
	double xs = ui->xStepSize_edit->text().toDouble();
	app->xStepSize(xs);

	double ys = ui->yStepSize_edit->text().toDouble();
	app->yStepSize(ys);

	double frameSep = ui->frameSeparation_edit->text().toDouble();
	app->frameSeparation(frameSep);

	int maxStepRate = ui->maxStepRate_edit->text().toInt();
	app->maxStepRate(maxStepRate);

	app->xLeftReverse(ui->xLeftReverse_check->isChecked());
	app->xRightReverse(ui->xRightReverse_check->isChecked());
	app->yLeftReverse(ui->yLeftReverse_check->isChecked());
	app->yRightReverse(ui->yRightReverse_check->isChecked());

	QDialog::accept();
}

void SetupDialog::on_port_combo_currentIndexChanged(int index)
{
	auto app = FoamcutApp::instance();
	if (index >= 0 && index < serialPorts_.size()) {
		const QSerialPortInfo &spi = serialPorts_.at(index);
		app->portChanged(spi);
		qDebug() << "port changed to " << spi.portName() << " system: " << spi.systemLocation();
	}
	else if (index == serialPorts_.size()) {
		QSerialPortInfo spi("none");
		app->portChanged(spi);
		if (spi.isValid()) {
			qDebug() << "why is this valid?";
		}
		qDebug() << "port changed to none";
	}
}
