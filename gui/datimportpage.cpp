#include "datimportpage.h"
#include "ui_datimportpage.h"
#include <QFileDialog>
#include <fstream>
#include <QtDebug>

#include "airfoil.hpp"

DatImportPage::DatImportPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::DatImportPage)
{
    ui->setupUi(this);

    connect(ui->fileEdit, SIGNAL(textChanged(QString)), this,
            SLOT(on_fileName_textChanged(QString&)));

    registerField("dat.file*", ui->fileEdit);
}

DatImportPage::~DatImportPage()
{
    delete ui;
}

void DatImportPage::initializePage()
{
    bool isXFoil = field("type.xfoil").toBool();
    if (isXFoil) {
        ui->rotateLabel->setText(tr("Alpha"));
        ui->scaleLabel->setText(tr("Chord"));
        ui->reverseCheck->setText(tr("Add LE cooling loop"));
    } else {
        ui->rotateLabel->setText(tr("Rotate"));
        ui->scaleLabel->setText(tr("Scale"));
        ui->reverseCheck->setText(tr("Add LE cooling loop"));
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
}

void DatImportPage::on_fileName_textChanged(QString &text)
{
    if (text.length() != 0) {
        try {
            std::ifstream in(text.toLatin1(), std::ifstream::in);
            foamcut::DatFile::handle df = foamcut::DatFile::read(in);

            bool isXFoil = field("type.xfoil").toBool();
            if (isXFoil) {
                foamcut::Airfoil::handle airfoil(new foamcut::Airfoil(df));
            }
        } catch (std::exception &ex) {
            qCritical() << "Failed to open file" << text << "\n" << ex.what();
            ui->fileEdit->setText("");
        }
    }

}
