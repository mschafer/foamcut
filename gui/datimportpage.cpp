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
#include <QFileDialog>
#include <fstream>
#include <QDebug>
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

	plotMgr_.reset(new ShapePlotMgr(ui->importPlot));

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

	plotMgr_->update(shape_);
}

