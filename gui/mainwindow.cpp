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
#include <memory>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "importwizard.h"
#include "ruled_surface.hpp"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->speed_edit->setValidator(new QDoubleValidator());
    ui->zRightFrame_edit->setValidator(new QDoubleValidator());
    ui->rotatePart_edit->setValidator(new QDoubleValidator());

    ui->rootZ_edit->setValidator(new QDoubleValidator());
    ui->rootKerf_edit->setValidator(new QDoubleValidator());
    ui->rootName_label->setText("");
    ui->rootSpeeds_label->setText("");

    ui->tipZ_edit->setValidator(new QDoubleValidator());
    ui->tipKerf_edit->setValidator(new QDoubleValidator());
    ui->tipName_label->setText("");
    ui->tipSpeeds_label->setText("");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_rootImport_button_clicked()
{
    std::auto_ptr<ImportWizard> iw(new ImportWizard());
    iw->exec();
	if (iw->result() == QDialog::Accepted) {
		rootShape_ = iw->shape();
		ui->rootName_label->setText(QString::fromStdString(rootShape_->name()));
	}
}

void MainWindow::on_tipImport_button_clicked()
{
    std::auto_ptr<ImportWizard> iw(new ImportWizard());
    iw->exec();
	if (iw->result() == QDialog::Accepted) {
		tipShape_ = iw->shape();
		ui->tipName_label->setText(QString::fromStdString(tipShape_->name()));
	}
}



void MainWindow::on_rootZ_edit_editingFinished()
{

}

void MainWindow::on_rootKerf_edit_editingFinished()
{

}


void MainWindow::on_tipZ_edit_editingFinished()
{

}

void MainWindow::on_tipKerf_edit_editingFinished()
{

}

void MainWindow::geometryChanged()
{
	///\todo handle error of mismatched segment count

	if (rootShape_ != nullptr) {
		double rootKerf = ui->rootKerf_edit->text().toDouble();
		rootKerfShape_ = rootShape_->offset(rootKerf / 2.);
	} else {
		rootKerfShape_ = nullptr;
	}

	if (tipShape_ != nullptr) {
		double tipKerf = ui->tipKerf_edit->text().toDouble();
		tipKerfShape_ = tipShape_->offset(tipKerf / 2.);
	} else {
		tipKerfShape_ = nullptr;
	}

	if (rootKerfShape_ != nullptr && tipKerfShape_ != nullptr) {
		double rootZ = ui->rootZ_edit->text().toDouble();
		double tipZ  = ui->tipZ_edit->text().toDouble();
		///\todo get correct eps value from step size
		partPath_.reset(new foamcut::RuledSurface(*rootKerfShape_, *tipKerfShape_, rootZ, tipZ-rootZ, .001));
		double zRightFrame = ui->zRightFrame_edit->text().toDouble();
		cutterPath_ = partPath_->interpolateZ(0., zRightFrame);

		// update the plot

	} else {
		partPath_ = nullptr;
		cutterPath_ = nullptr;

		///\todo put text here on the plot saying root and tip need to be imported
	}
}
