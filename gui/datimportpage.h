#ifndef DATIMPORTPAGE_H
#define DATIMPORTPAGE_H

#include <QWizardPage>
#include <memory>
#include <boost/shared_ptr.hpp>

namespace Ui {
    class DatImportPage;
}

namespace foamcut {
	class Shape;
	class DatFile;
}

class DatImportPage : public QWizardPage
{
    Q_OBJECT
    
public:
    explicit DatImportPage(QWidget *parent = 0);
    ~DatImportPage();

    void initializePage();
    int nextId() const;
    
private slots:
    void on_fileBrowseButton_clicked();
    void on_fileName_edit_editingFinished(void);
	void do_replot(void);

private:
    Ui::DatImportPage *ui;
	boost::shared_ptr<foamcut::Shape> shape_;
	boost::shared_ptr<foamcut::DatFile> datFile_;
};

#endif // DATIMPORTPAGE_H
