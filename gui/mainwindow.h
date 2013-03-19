#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_rootImport_button_clicked();
    void on_rootZ_edit_editingFinished();
    void on_rootKerf_edit_editingFinished();

    void on_tipImport_button_clicked();
    void on_tipZ_edit_editingFinished();
    void on_tipKerf_edit_editingFinished();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
