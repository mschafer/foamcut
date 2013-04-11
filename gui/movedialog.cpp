#include "movedialog.h"
#include "ui_movedialog.h"

MoveDialog::MoveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveDialog)
{
    ui->setupUi(this);
}

MoveDialog::~MoveDialog()
{
    delete ui;
}
