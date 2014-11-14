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
#include <Host.hpp>
#include "mainwindow.h"
#include "cutdialog.h"
#include "movedialog.h"
#include "setupdialog.h"
#include "ui_mainwindow.h"
#include "importwizard.h"
#include "ruled_surface.hpp"
#include "cutplotmgr.h"
#include "simdialog.h"
#include "foamcutapp.hpp"
#include "settings.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->speed_edit->setValidator(new QDoubleValidator());
    ui->zRightFrame_edit->setValidator(new QDoubleValidator());
    ui->rotatePart_edit->setValidator(new QDoubleValidator());
    ui->xLeadIn_edit->setValidator(new QDoubleValidator());
    ui->yLeadIn_edit->setValidator(new QDoubleValidator());

    ui->rootZ_edit->setValidator(new QDoubleValidator());
    ui->rootKerf_edit->setValidator(new QDoubleValidator());
    ui->rootName_label->setText("");
    ui->rootSpeeds_label->setText("");

    ui->tipZ_edit->setValidator(new QDoubleValidator());
    ui->tipKerf_edit->setValidator(new QDoubleValidator());
    ui->tipName_label->setText("");
    ui->tipSpeeds_label->setText("");

    cutPlotMgr_.reset(new CutPlotMgr(ui->cut_fixedARPlot));

	SimDialog *simd = new SimDialog(this);
	simd->show();
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
		if (rootShape_->area() < 0) {
			ui->rootKerf_label->setText("Kerf [CW]");
		} else {
			ui->rootKerf_label->setText("Kerf [CCW]");
		}
		ui->rootName_label->setText(QString::fromStdString(rootShape_->name()));
		geometryChanged(true);
	}
}

void MainWindow::on_tipImport_button_clicked()
{
    std::auto_ptr<ImportWizard> iw(new ImportWizard());
    iw->exec();
	if (iw->result() == QDialog::Accepted) {
		tipShape_ = iw->shape();
		if (tipShape_->area() < 0) {
			ui->tipKerf_label->setText("Kerf [CW]");
		} else {
			ui->tipKerf_label->setText("Kerf [CCW]");
		}
		ui->tipName_label->setText(QString::fromStdString(tipShape_->name()));
		geometryChanged(true);
	}
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
			double zRightFrame = ui->zRightFrame_edit->text().toDouble();
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

	auto script = cutterPath_->generateScript(foamcut::Settings::stepperInfo());
	app->host().executeScript(*script);

	std::unique_ptr<CutDialog> cd(new CutDialog());
	cd->exec();
}

void MainWindow::on_actionSetup_triggered()
{
	std::unique_ptr<SetupDialog> sd(new SetupDialog());
	sd->exec();
}
