#ifndef CUTDIALOG_H
#define CUTDIALOG_H

#include <QDialog>

namespace Ui {
class CutDialog;
}

class CutDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CutDialog(QWidget *parent = 0);
    ~CutDialog();

private slots:
	void on_pause_button_clicked();
	void on_speed_spin_valueChanged(double val);
	void on_buttonBox_rejected();
	void update();

private:
    Ui::CutDialog *ui;
};

#endif // CUTDIALOG_H
