#include "datimportpage.h"
#include "ui_datimportpage.h"
#include "shapeplots.h"
#include <QFileDialog>
#include <fstream>
#include <QtDebug>
#include <boost/algorithm/minmax_element.hpp>
#include "airfoil.hpp"

DatImportPage::DatImportPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::DatImportPage)
{
    ui->setupUi(this);

	connect(ui->rotate_edit, SIGNAL(editingFinished()), this, SLOT(do_replot()));
	connect(ui->scale_edit, SIGNAL(editingFinished()), this, SLOT(do_replot()));
	connect(ui->reverse_check, SIGNAL(stateChanged(int)), this, SLOT(do_check_changed(int)));
	connect(ui->flipHorizontal_check, SIGNAL(stateChanged(int)), this, SLOT(do_check_changed(int)));
	connect(ui->flipVertical_check, SIGNAL(stateChanged(int)), this, SLOT(do_check_changed(int)));

	ui->rotate_edit->setValidator(new QDoubleValidator());
	ui->scale_edit->setValidator(new QDoubleValidator());


    registerField("dat.file*", ui->fileName_edit);
}

DatImportPage::~DatImportPage()
{
    delete ui;
}

void DatImportPage::initializePage()
{
	datFile_.reset();
    bool isXFoil = field("type.xfoil").toBool();
    if (isXFoil) {
        ui->rotateLabel->setText(tr("Alpha"));
        ui->scaleLabel->setText(tr("Chord"));
        ui->reverse_check->setText(tr("Add LE loop"));
    } else {
        ui->rotateLabel->setText(tr("Rotate"));
        ui->scaleLabel->setText(tr("Scale"));
        ui->reverse_check->setText(tr("Reverse points"));
    }

	if (ui->importPlot->plottableCount() == 0) {
        QCPCurve *curve = new QCPCurve(ui->importPlot->xAxis, ui->importPlot->yAxis);
        ui->importPlot->addPlottable(curve);

        curve = new QCPCurve(ui->importPlot->xAxis, ui->importPlot->yAxis);
        curve->setLineStyle(QCPCurve::lsNone);
        curve->setScatterStyle(QCP::ssDiamond);
        ui->importPlot->addPlottable(curve);
        ui->importPlot->xAxis->setLabel("X");
        ui->importPlot->yAxis->setLabel("Y");
        ui->importPlot->xAxis->setRange(-1., 1.);
        ui->importPlot->yAxis->setRange(-1., 1.);
    }
}

int DatImportPage::nextId() const
{
    return -1;
}

void DatImportPage::on_fileBrowseButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open .dat file"), "", tr(".dat files (*.dat);;All files (*.*)"));

    if (fileName.length() != 0) {
        ui->fileName_edit->setText(fileName);
    }

	on_fileName_edit_editingFinished();
}

void DatImportPage::on_fileName_edit_editingFinished()
{
	QString fileName = ui->fileName_edit->text();
    if (fileName.length() != 0) {
        try {
            std::ifstream in(fileName.toLatin1(), std::ifstream::in);
            datFile_ = foamcut::DatFile::read(in);
			do_replot();
        } catch (std::exception &ex) {
            qCritical() << "Failed to open file" << fileName << "\n" << ex.what();
            ui->fileName_edit->setText("");
        }
    }
}

void DatImportPage::do_check_changed(int state)
{
	do_replot();
}

void DatImportPage::do_replot()
{
	if (datFile_.get() == NULL) return;

	double scale = ui->scale_edit->text().toDouble();
	double rotate = ui->rotate_edit->text().toDouble();
	bool isXFoil = field("type.xfoil").toBool();
	bool isReversed = ui->reverse_check->isChecked();
	if (isXFoil) {
		bool leLoop = isReversed;  // meaning and label changes for xfoil
		foamcut::Airfoil::handle airfoil(new foamcut::Airfoil(datFile_, scale, rotate, ui->reverse_check->isChecked()));
		shape_ = airfoil->shape();
	} else {
		shape_.reset(new foamcut::Shape(*datFile_));
		shape_ = shape_->scale(scale);
		shape_ = shape_->rotate(rotate);
		if (isReversed) {
			shape_ = shape_->reverse();
		}
	}

	double xFlip = ui->flipVertical_check->isChecked() ? -1. : 1.;
	double yFlip = ui->flipHorizontal_check->isChecked() ? -1. : 1.;
	shape_ = shape_->scale(xFlip, yFlip);

	// convert to QCPCurveData and plot
	QCPCurve *curve = dynamic_cast<QCPCurve *>(ui->importPlot->plottable(0));
	QCPCurveDataMap *dataMap = lineFit(*shape_);
	BoundingBox bbox = bounds(dataMap);
	ui->importPlot->xAxis->setRange(bbox.xMin_*1.01, bbox.xMax_*1.01);
	ui->importPlot->yAxis->setRange(bbox.yMin_*1.01, bbox.yMax_*1.01);
	curve->setData(dataMap, false);

	curve = dynamic_cast<QCPCurve *>(ui->importPlot->plottable(1));
	dataMap = breakPoints(*shape_);
	curve->setData(dataMap, false);

	ui->importPlot->replot();

}

