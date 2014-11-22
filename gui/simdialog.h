#ifndef SIMDIALOG_H
#define SIMDIALOG_H

#include <QDialog>
#include "qcustomplot.h"

namespace Ui {
class SimDialog;
}

class SimDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SimDialog(QWidget *parent = 0);
    ~SimDialog();

private slots:
	void update();
	void beforeReplot();

	void on_clear_button_clicked();

private:
	enum {
		LEFT_CURVE = 0,
		RIGHT_CURVE = 1
	};

    Ui::SimDialog *ui;
};

#endif
