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

	connect(ui->rotate_edit, SIGNAL(editingFinished()), this, SLOT(do_replot()));
	connect(ui->scale_edit, SIGNAL(editingFinished()), this, SLOT(do_replot()));
	connect(ui->reverse_check, SIGNAL(stateChanged()), this, SLOT(do_replot()));
	connect(ui->flipHorizontal_check, SIGNAL(stateChanged()), this, SLOT(do_replot()));
	connect(ui->flipVertical_check, SIGNAL(stateChanged()), this, SLOT(do_replot()));

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

void DatImportPage::do_replot()
{
	if (datFile_.get() == NULL) return;

	double scale = ui->scale_edit->text().toDouble();
	double rotate = ui->rotate_edit->text().toDouble();
	bool isXFoil = field("type.xfoil").toBool();
	if (isXFoil) {
		foamcut::Airfoil::handle airfoil(new foamcut::Airfoil(datFile_, scale, rotate));
		shape_ = airfoil->shape();

		// for xfoil import, the reverse check means add LE loop
		if (ui->reverse_check->isChecked()) {
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

	QCPCurve *curve = dynamic_cast<QCPCurve *>(ui->importPlot->plottable(0));
	curve->setData(x, y);

	x.clear(); y.clear();
	shape_->breaks(x, y);
	curve = dynamic_cast<QCPCurve *>(ui->importPlot->plottable(1));
	curve->setData(x, y);

	ui->importPlot->replot();

}

