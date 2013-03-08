#ifndef DATIMPORTPAGE_H
#define DATIMPORTPAGE_H

#include <QWizardPage>
#include <memory>

namespace Ui {
    class DatImportPage;
}

namespace foamcut {
    class Airfoil;
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
    void on_fileName_textChanged(QString &text);

private:
    Ui::DatImportPage *ui;
    std::auto_ptr<foamcut::Airfoil> airfoil_;
};

#endif // DATIMPORTPAGE_H
