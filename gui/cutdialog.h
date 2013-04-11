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
    
private:
    Ui::CutDialog *ui;
};

#endif // CUTDIALOG_H
