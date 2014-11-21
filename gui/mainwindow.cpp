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
#include "cutdialog.h"
#include "movedialog.h"
#include "setupdialog.h"
#include "ui_mainwindow.h"
#include "datimportwizard.h"
#include "ruled_surface.hpp"
#include "cutplotmgr.h"
#include "simdialog.h"
#include "foamcutapp.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	auto app = FoamcutApp::instance();
    ui->setupUi(this);

	ui->speed_edit->setValidator(new QDoubleValidator());
    ui->xLeadIn_edit->setValidator(new QDoubleValidator());
    ui->yLeadIn_edit->setValidator(new QDoubleValidator());

    ui->rootZ_edit->setValidator(new QDoubleValidator());
    ui->rootKerf_edit->setValidator(new QDoubleValidator());
    ui->rootName_label->setText("");

    ui->tipZ_edit->setValidator(new QDoubleValidator());
    ui->tipKerf_edit->setValidator(new QDoubleValidator());
    ui->tipName_label->setText("");

	ui->speed_edit->setText(QString::number(app->cutSpeed()));
	
	cutPlotMgr_.reset(new CutPlotMgr(ui->cut_fixedARPlot));

	//SimDialog *simd = new SimDialog(this);
	//simd->show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_speed_edit_editingFinished()
{
	///\todo need to update times on RuledSurfaces
	auto app = FoamcutApp::instance();
	app->cutSpeed(ui->speed_edit->text().toDouble());
}

void MainWindow::on_rootZ_edit_editingFinished()
{
	geometryChanged();
}

void MainWindow::on_rootKerf_edit_editingFinished()
{
	geometryChanged();
}


void MainWindow::on_tipZ_edit_editingFinished()
{
	geometryChanged();
}

void MainWindow::on_tipKerf_edit_editingFinished()
{
	geometryChanged();
}

void MainWindow::on_xLeadIn_edit_editingFinished()
{
	geometryChanged();
}

void MainWindow::on_yLeadIn_edit_editingFinished()
{
	geometryChanged();
}

void MainWindow::geometryChanged(bool rescale)
{
	auto app = FoamcutApp::instance();
	try {
		double xLead = ui->xLeadIn_edit->text().toDouble();
		double yLead = ui->yLeadIn_edit->text().toDouble();

		if (rootShape_ != nullptr) {
			double rootKerf = ui->rootKerf_edit->text().toDouble();
			rootKerfShape_ = rootShape_->offset(rootKerf / 2.);
			rootKerfShape_ = rootKerfShape_->addLeadInOut(xLead, yLead);
		} else {
			rootKerfShape_ = nullptr;
		}

		if (tipShape_ != nullptr) {
			double tipKerf = ui->tipKerf_edit->text().toDouble();
			tipKerfShape_ = tipShape_->offset(tipKerf / 2.);
			tipKerfShape_ = tipKerfShape_->addLeadInOut(xLead, yLead);
		} else {
			tipKerfShape_ = nullptr;
		}

		if (rootKerfShape_ != nullptr && tipKerfShape_ != nullptr) {
			double rootZ = ui->rootZ_edit->text().toDouble();
			double tipZ  = ui->tipZ_edit->text().toDouble();
			///\todo get correct eps value from step size
			partPath_.reset(new foamcut::RuledSurface(*rootKerfShape_, *tipKerfShape_, rootZ, tipZ-rootZ, .001));
			double zRightFrame = app->frameSeparation();
			partPath_->setTime(ui->speed_edit->text().toDouble());
			cutterPath_ = partPath_->interpolateZ(0., zRightFrame);
			ui->cut_button->setEnabled(true);
		} else {
			partPath_ = nullptr;
			cutterPath_ = nullptr;
			ui->cut_button->setEnabled(false);
		}
		cutPlotMgr_->update(rootShape_, tipShape_, partPath_, cutterPath_, rescale);
	} catch (std::exception &ex) {
		qDebug() << ex.what();
	}
}

void MainWindow::on_move_button_clicked()
{
	std::unique_ptr<MoveDialog> md(new MoveDialog());
	md->exec();
}

void MainWindow::on_cut_button_clicked()
{
	FoamcutApp *app = FoamcutApp::instance();
	if (app->host().scriptRunning()) {
		QMessageBox::critical(nullptr, "Foamcutter Device Busy", 
			"The foamcutter device is already cutting something");
		return;
	}

	auto script = cutterPath_->generateScript(app->stepperInfo());
	app->host().executeScript(*script);

	std::unique_ptr<CutDialog> cd(new CutDialog());
	cd->exec();
}

void MainWindow::on_actionSetup_triggered()
{
	std::unique_ptr<SetupDialog> sd(new SetupDialog());
	sd->exec();
}

void MainWindow::on_wire_slider_valueChanged(int slider)
{
	// nothing to do unless both shapes are defined and we have a path
	if (cutterPath_) {
		double t = cutterPath_->duration() * (double)slider / (double)ui->wire_slider->maximum();
		cutPlotMgr_->updateWirePosition(t);
	}
}

void MainWindow::on_actionDAT_Import_triggered()
{
	std::unique_ptr<DatImportWizard> diw(new DatImportWizard());
	diw->exec();
	if (diw->result() == QDialog::Accepted) {
		auto shapes = diw->shapes();
		rootShape_ = shapes.first;
		tipShape_ = shapes.second;
		ui->rootName_label->setText(QString::fromStdString(rootShape_->name()));
		ui->tipName_label->setText(QString::fromStdString(tipShape_->name()));
		geometryChanged(true);
	}
}

void MainWindow::on_swap_button_clicked()
{
	// swap the shapes
	foamcut::Shape::handle tmp = rootShape_;
	rootShape_ = tipShape_;
	tipShape_ = tmp;

	// swap the shape names
	ui->rootName_label->setText(QString::fromStdString(rootShape_->name()));
	ui->tipName_label->setText(QString::fromStdString(tipShape_->name()));

	// swap the kerfs
	QString tmpK = ui->rootKerf_edit->text();
	ui->rootKerf_edit->setText(ui->tipKerf_edit->text());
	ui->tipKerf_edit->setText(tmpK);

	geometryChanged();
}