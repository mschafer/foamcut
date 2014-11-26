#include "setupdialog.h"
#include "ui_setupdialog.h"
#include <qdebug.h>
#include "foamcutapp.hpp"
#include <QSerialPortInfo>

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

	bool foundPort = false;
	QList<QSerialPortInfo> spList = QSerialPortInfo::availablePorts();
	auto it = spList.begin();
	while (it < spList.end()) {
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
	connect(ui->port_combo, &QComboBox::currentTextChanged, app, &FoamcutApp::portChanged);
}

SetupDialog::~SetupDialog()
{
    delete ui;
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