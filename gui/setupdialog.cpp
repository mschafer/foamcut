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
	ui->maxStepRate_edit->setText(QString::number(app->maxStepRate()));

	ui->xStepSize_edit->setValidator(new QDoubleValidator(0, 1.e9, 20));
	ui->yStepSize_edit->setValidator(new QDoubleValidator(0, 1.e9, 20));
	ui->maxStepRate_edit->setValidator(new QIntValidator(0, 5000));

	ui->port_combo->addItem("Simulator");
	QList<QSerialPortInfo> spList = QSerialPortInfo::availablePorts();
	auto it = spList.begin();
	while (it < spList.end()) {
		ui->port_combo->addItem(it->portName());
		++it;
	}
	ui->port_combo->setCurrentIndex(0);
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

	int maxStepRate = ui->maxStepRate_edit->text().toInt();
	app->maxStepRate(maxStepRate);

	qDebug() << "setup accepted";
	QDialog::accept();
}