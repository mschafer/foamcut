#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <boost/shared_ptr.hpp>

namespace foamcut {
class Shape;
class RuledSurface;
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_rootZ_edit_editingFinished();
    void on_rootKerf_edit_editingFinished();
    void on_tipZ_edit_editingFinished();
    void on_tipKerf_edit_editingFinished();

	void on_xLeadIn_edit_editingFinished();
	void on_yLeadIn_edit_editingFinished();

	void geometryChanged(bool rescale = false);

	void on_speed_edit_editingFinished();
	void on_move_button_clicked();
	void on_cut_button_clicked();
	void on_swap_button_clicked();

	void on_actionSetup_triggered();
	void on_actionDAT_Import_triggered();
	void on_actionSimulator_triggered();

	void on_wire_slider_valueChanged(int);

	void on_connectionChanged(bool connected);

private:
    std::unique_ptr<Ui::MainWindow> ui;
	boost::shared_ptr<foamcut::Shape> rootShape_;
	boost::shared_ptr<foamcut::Shape> tipShape_;
	boost::shared_ptr<foamcut::Shape> rootKerfShape_;
	boost::shared_ptr<foamcut::Shape> tipKerfShape_;
	boost::shared_ptr<foamcut::RuledSurface> partPath_;
	boost::shared_ptr<foamcut::RuledSurface> cutterPath_;
	std::unique_ptr<class CutPlotMgr> cutPlotMgr_;
};

#endif // MAINWINDOW_H
