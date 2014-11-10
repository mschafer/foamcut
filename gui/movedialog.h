#ifndef MOVEDIALOG_H
#define MOVEDIALOG_H

#include <QDialog>

namespace Ui {
class MoveDialog;
}

class MoveDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MoveDialog(QWidget *parent = 0);
    ~MoveDialog();
    
private slots:
	void on_up_button_clicked();
	void on_left_button_clicked();
	void on_right_button_clicked();
	void on_down_button_clicked();
	void on_home_button_clicked();

private:
    Ui::MoveDialog *ui;

	void moveX(int sign);
	void moveY(int sign);
};

#endif // MOVEDIALOG_H
