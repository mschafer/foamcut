#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "importwizard.h"
#include <memory>

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

void MainWindow::on_pushButton_clicked()
{
    QVector<double> x(100);
    QVector<double> y(100);

    for (int i=0; i<100; ++i) {
        double theta = i * 6.28 / 99.;
        x[i] = sin(theta);
        y[i] = cos(theta);
    }

    if (ui->customPlot->plottableCount() == 0) {
        QCPCurve *curve = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);
        ui->customPlot->addPlottable(curve);
        curve->setData(x,y);
        ui->customPlot->xAxis->setLabel("X");
        ui->customPlot->yAxis->setLabel("Y");
        ui->customPlot->xAxis->setRange(-1., 1.);
        ui->customPlot->yAxis->setRange(-1., 1.);
        ui->customPlot->replot();
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    std::auto_ptr<ImportWizard> iw(new ImportWizard());
    iw->exec();
}

void MainWindow::on_rootImport_button_clicked()
{
	qDebug() << "root import clicked\n";
}

void MainWindow::on_rootZ_edit_editingFinished()
{

}

void MainWindow::on_rootKerf_edit_editingFinished()
{

}

void MainWindow::on_tipImport_button_clicked()
{
	qDebug() << "tip import clicked\n";
}

void MainWindow::on_tipZ_edit_editingFinished()
{

}

void MainWindow::on_tipKerf_edit_editingFinished()
{

}

