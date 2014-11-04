#include "setupdialog.h"
#include "ui_setupdialog.h"
#include <qdebug.h>
#include <qsettings.h>

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);

	QSettings settings;
	double xNum = settings.value("stepper/xSizeNum", 1).toDouble();
	double xDen = settings.value("stepper/xSizeDen", 1000).toDouble();
	double yNum = settings.value("stepper/ySizeNum", 1).toDouble();
	double yDen = settings.value("stepper/ySizeDen", 1000).toDouble();
	int maxStepRate = settings.value("stepper/maxStepRate", 2000).toInt();

	ui->xStepSizeNum_edit->setText(QString::number(xNum));
	ui->xStepSizeDen_edit->setText(QString::number(xDen));
	ui->yStepSizeNum_edit->setText(QString::number(yNum));
	ui->yStepSizeDen_edit->setText(QString::number(yDen));
	ui->maxStepRate_edit->setText(QString::number(maxStepRate));

	ui->xStepSizeNum_edit->setValidator(new QDoubleValidator());
	ui->xStepSizeDen_edit->setValidator(new QDoubleValidator());
	ui->yStepSizeNum_edit->setValidator(new QDoubleValidator());
	ui->yStepSizeDen_edit->setValidator(new QDoubleValidator());
	ui->maxStepRate_edit->setValidator(new QIntValidator(0, 5000));

	//connect(ui->dialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

SetupDialog::~SetupDialog()
{
    delete ui;
}

void SetupDialog::accept()
{
	QSettings settings;



	qDebug() << "setup accepted";
	QDialog::accept();
}