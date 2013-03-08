#include "importwizard.h"
#include "datimportpage.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QRadioButton>

ImportWizard::ImportWizard(QWidget *parent) :
    QWizard(parent)
{
    setPage(TYPE_PAGE, new TypePage());
    setPage(IMPORT_PAGE, new DatImportPage());

    setStartId(TYPE_PAGE);

#ifndef Q_WS_MAC
    setWizardStyle(ModernStyle);
#endif

    setWindowTitle(tr("Import Shape Wizard"));
}

TypePage::TypePage(QWidget *parent)
    :QWizardPage(parent)
{
    setTitle(tr("File Type"));

    xfoilRB_ = new QRadioButton(tr("&XFoil airfoil"));
    datRB_ = new QRadioButton(tr("&Generic .dat file"));

    typeLabel_ = new QLabel(tr("file type"));
    typeLabel_->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(xfoilRB_);
    layout->addWidget(datRB_);
    layout->addWidget(typeLabel_);
    setLayout(layout);

    connect(xfoilRB_, SIGNAL(toggled(bool)), this, SLOT(typeChanged(bool)));
    connect(datRB_, SIGNAL(toggled(bool)), this, SLOT(typeChanged(bool)));

    xfoilRB_->setChecked(true);
    registerField("type.xfoil", xfoilRB_);

}

int TypePage::nextId() const
{
    return ImportWizard::IMPORT_PAGE;
}

void TypePage::typeChanged(bool checked)
{
    if (!checked) return;

    if (xfoilRB_->isChecked())
        typeLabel_->setText(tr("Importing an airfoil takes the following extra steps\n"
                               "  1. Reorder points so top (+y) surface is first.\n"
                               "  2. Reflect and translate TE to x=0 and LE to x=chord."
                               "  3. Add a synchronization point at the LE.\n"
                               "  4. Scale to desired chord.\n"
                               "  5. Rotate to desired angle of attack.\n"
                               "  6. Add optional cooling loop at leading edge."
                               ));
    else
        typeLabel_->setText(tr("Import a .dat file as is.  The following options are "
                               "available: \n"
                               "  1. Reverse the order of the points."
                               "  2. Scale the shape."
                               "  3. Rotate the shape."
                               ));
}
