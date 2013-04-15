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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_rootImport_button_clicked();
    void on_rootZ_edit_editingFinished();
    void on_rootKerf_edit_editingFinished();

    void on_tipImport_button_clicked();
    void on_tipZ_edit_editingFinished();
    void on_tipKerf_edit_editingFinished();

	void geometryChanged();

	void on_move_button_clicked();
	void on_cut_button_clicked();

private:
    Ui::MainWindow *ui;
	boost::shared_ptr<foamcut::Shape> rootShape_;
	boost::shared_ptr<foamcut::Shape> tipShape_;
	boost::shared_ptr<foamcut::Shape> rootKerfShape_;
	boost::shared_ptr<foamcut::Shape> tipKerfShape_;
	boost::shared_ptr<foamcut::RuledSurface> partPath_;
	boost::shared_ptr<foamcut::RuledSurface> cutterPath_;
	std::unique_ptr<class CutPlotMgr> cutPlotMgr_;

};

#endif // MAINWINDOW_H
