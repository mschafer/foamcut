#include "cutdialog.h"
#include "ui_cutdialog.h"

CutDialog::CutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CutDialog)
{
    ui->setupUi(this);
}

CutDialog::~CutDialog()
{
    delete ui;
}
