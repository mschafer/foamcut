#include "datimportpage.h"
#include "ui_datimportpage.h"
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

    connect(ui->fileEdit, SIGNAL(editingFinished()), this,
            SLOT(on_fileName_editingFinished()));
	connect(ui->rotateEdit, SIGNAL(editingFinished()), this, SLOT(do_replot()));
	connect(ui->scaleEdit, SIGNAL(editingFinished()), this, SLOT(do_replot()));

	ui->rotateEdit->setValidator(new QDoubleValidator());
	ui->scaleEdit->setValidator(new QDoubleValidator());


    registerField("dat.file*", ui->fileEdit);
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
        ui->reverseCheck->setText(tr("Add LE cooling loop"));
    } else {
        ui->rotateLabel->setText(tr("Rotate"));
        ui->scaleLabel->setText(tr("Scale"));
        ui->reverseCheck->setText(tr("Reverse"));
    }

	if (ui->importPlot->plottableCount() == 0) {
        QCPCurve *curve = new QCPCurve(ui->importPlot->xAxis, ui->importPlot->yAxis);
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
        ui->fileEdit->setText(fileName);
    }

	on_fileName_editingFinished();
}

void DatImportPage::on_fileName_editingFinished()
{
	QString fileName = ui->fileEdit->text();
    if (fileName.length() != 0) {
        try {
            std::ifstream in(fileName.toLatin1(), std::ifstream::in);
            datFile_ = foamcut::DatFile::read(in);
			do_replot();
        } catch (std::exception &ex) {
            qCritical() << "Failed to open file" << fileName << "\n" << ex.what();
            ui->fileEdit->setText("");
        }
    }
}

void DatImportPage::do_replot()
{
	if (datFile_.get() == NULL) return;

	double scale = ui->scaleEdit->text().toDouble();
	double rotate = ui->rotateEdit->text().toDouble();
	bool isXFoil = field("type.xfoil").toBool();
	if (isXFoil) {
		foamcut::Airfoil::handle airfoil(new foamcut::Airfoil(datFile_, scale, rotate));
		shape_ = airfoil->shape();

		// for xfoil import, the reverse check means add LE loop
		if (ui->reverseCheck->isChecked()) {
			///\todo implement me
		}
	} else {
		foamcut::DatFile::handle df = datFile_->scale(scale);
		df = df->rotate(rotate);
		shape_.reset(new foamcut::Shape(*df));
	}

	QVector<double> x;
	QVector<double> y;
	shape_->plottable(x, y);
	typedef QVector<double>::iterator iterator;
	std::pair<iterator, iterator> xlim = boost::minmax_element(x.begin(), x.end());
	std::pair<iterator, iterator> ylim = boost::minmax_element(y.begin(), y.end());
	ui->importPlot->xAxis->setRange(*(xlim.first)*1.01, *(xlim.second)*1.01);
	ui->importPlot->yAxis->setRange(*(ylim.first)*1.01, *(ylim.second)*1.01);

	QCPCurve *curve = dynamic_cast<QCPCurve *>(ui->importPlot->plottable());
	curve->setData(x, y);
	ui->importPlot->replot();
}

