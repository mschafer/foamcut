/*
 * (C) Copyright 2013 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
#include "datimportpage.h"
#include "ui_datimportpage.h"
#include "shapeplotmgr.h"
#include "foamcutapp.hpp"
#include <QFileDialog>
#include <fstream>
#include <QDebug>
#include <QSettings>
#include <boost/algorithm/minmax_element.hpp>
#include "airfoil.hpp"

DatImportPage::DatImportPage(DatImportPage::Side side, QWidget *parent) :
    side_(side), QWizardPage(parent),
    ui(new Ui::DatImportPage())
{
	ui->setupUi(this);
	registerField(fileFieldName() + "*", ui->fileName_edit);
	if (side == ROOT) {
		setTitle("Root");
	}
	else {
		setTitle("Tip");
	}

	connect(ui->alpha_edit, SIGNAL(editingFinished()), this, SLOT(do_replot()));
	connect(ui->chord_edit, SIGNAL(editingFinished()), this, SLOT(do_replot()));

	ui->alpha_edit->setValidator(new QDoubleValidator());
	ui->chord_edit->setValidator(new QDoubleValidator());

	plotMgr_.reset(new ShapePlotMgr(ui->importPlot));
}

DatImportPage::~DatImportPage()
{
}

void DatImportPage::initializePage()
{
	FoamcutApp *app = FoamcutApp::instance();
	datFile_.reset();
	bool isAirfoil = field("type.airfoil").toBool();
	if (!isAirfoil) {
		ui->alpha_edit->setDisabled(true);
		ui->chord_edit->setDisabled(true);
	}
}

void DatImportPage::on_fileBrowseButton_clicked()
{
	FoamcutApp *app = FoamcutApp::instance();
	QString lastPath = app->airfoilImportDir();
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open .dat file"), lastPath, tr(".dat files (*.dat);;All files (*.*)"));

    if (fileName.length() != 0) {
        ui->fileName_edit->setText(fileName);
        QString lastPath = QFileInfo(fileName).absolutePath() + QDir::separator() + "*";
        app->airfoilImportDir(lastPath);
    }

	on_fileName_edit_editingFinished();
}

void DatImportPage::on_fileName_edit_editingFinished()
{
	QString fileName = ui->fileName_edit->text();
	setField(fileFieldName(), fileName);

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

	double alpha = ui->alpha_edit->text().toDouble();
	double chord = ui->chord_edit->text().toDouble();
	bool isAirfoil = field("type.airfoil").toBool();
	bool leLoop = field("type.leLoop").toBool();
	if (isAirfoil) {
		foamcut::Airfoil::handle airfoil(new foamcut::Airfoil(datFile_, chord, alpha, leLoop));
		shape_ = airfoil->shape();
	} else {
		shape_.reset(new foamcut::Shape(*datFile_));
	}

	plotMgr_->update(shape_);
}

const QString &DatImportPage::fileFieldName()
{
	static const QString rootName("dat.rootFile");
	static const QString tipName("dat.tipFile");

	if (side_ == ROOT) { return rootName; }
	else { return tipName; }
}
