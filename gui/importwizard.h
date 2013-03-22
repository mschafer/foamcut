#ifndef IMPORTWIZARD_H
#define IMPORTWIZARD_H

#include <QWizard>
#include <boost/shared_ptr.hpp>

namespace foamcut {
	class Shape;
}

class QLabel;
class QRadioButton;
class DatImportPage;

class ImportWizard : public QWizard
{
    Q_OBJECT
public:
    enum {
        TYPE_PAGE,
        IMPORT_PAGE
    };

    explicit ImportWizard(QWidget *parent = 0);

	boost::shared_ptr<foamcut::Shape> shape();
signals:
    
public slots:
    
};

class TypePage : public QWizardPage
{
    Q_OBJECT

public:
    TypePage(QWidget *parent = 0);

    int nextId() const;

private slots:
    void typeChanged(bool checked);

private:
    QLabel *typeLabel_;
    QRadioButton *xfoilRB_;
    QRadioButton *datRB_;
};

#endif // IMPORTWIZARD_H
