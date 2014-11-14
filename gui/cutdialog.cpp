#include "cutdialog.h"
#include "ui_cutdialog.h"
#include "foamcutapp.hpp"
#include <Host.hpp>
#include <QTimer>

CutDialog::CutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CutDialog)
{
    ui->setupUi(this);
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(250);
}

CutDialog::~CutDialog()
{
    delete ui;
}

void CutDialog::on_pause_button_clicked()
{
	FoamcutApp *a = FoamcutApp::instance();
	if (ui->pause_button->isChecked()) {
		a->host().pause();
	} else {
		a->host().resume();
	}
}

void CutDialog::on_speed_spin_valueChanged(double val)
{
	FoamcutApp *a = FoamcutApp::instance();
	a->host().speedScaleFactor(val);
}

void CutDialog::on_buttonBox_rejected()
{
	FoamcutApp *a = FoamcutApp::instance();
	a->host().abort();
}

void CutDialog::update()
{
	FoamcutApp *a = FoamcutApp::instance();
	if (a->host().scriptRunning()) {
		ui->cut_progress->setValue((int)(a->host().scriptProgress()*100.));
	} else {
		accept();
	}
}