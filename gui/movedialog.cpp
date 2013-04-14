#include "movedialog.h"
#include "ui_movedialog.h"
#include <QDebug>

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

void MoveDialog::on_up_button_clicked()
{
	qDebug() << "up";
}

void MoveDialog::on_left_button_clicked()
{
	qDebug() << "left";
}

void MoveDialog::on_right_button_clicked()
{
	qDebug() << "right";
}

void MoveDialog::on_down_button_clicked()
{
	qDebug() << "down";
}
