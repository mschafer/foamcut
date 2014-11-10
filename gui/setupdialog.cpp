#include "setupdialog.h"
#include "ui_setupdialog.h"
#include <qdebug.h>
#include "settings.hpp"
#include <QSerialPortInfo>

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::SetupDialog)
{
    ui->setupUi(this);

	ui->xStepSize_edit->setText(QString::number(foamcut::Settings::xStepSize()));
	ui->yStepSize_edit->setText(QString::number(foamcut::Settings::yStepSize()));
	ui->maxStepRate_edit->setText(QString::number(foamcut::Settings::maxStepRate()));

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
	double xs = ui->xStepSize_edit->text().toDouble();
	foamcut::Settings::xStepSize(xs);

	double ys = ui->yStepSize_edit->text().toDouble();
	foamcut::Settings::yStepSize(ys);

	int maxStepRate = ui->maxStepRate_edit->text().toInt();
	foamcut::Settings::maxStepRate(maxStepRate);

	qDebug() << "setup accepted";
	QDialog::accept();
}