/*
 * (C) Copyright 2013 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
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

    ///\todo need cancel button on Mac because red x is disabled for some reason
    setOption(NoCancelButton, false);

    setWindowTitle(tr("Import Shape Wizard"));
}

boost::shared_ptr<foamcut::Shape> ImportWizard::shape()
{
	return ((DatImportPage *)page(IMPORT_PAGE))->shape();
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
                               "  2. Reflect and translate TE to x=0 and LE to x=chord.\n"
                               "  3. Add a synchronization point at the LE.\n"
                               "  4. Scale to desired chord.\n"
                               "  5. Rotate to desired angle of attack.\n"
                               "  6. Add optional cooling loop at leading edge."
                               ));
    else
        typeLabel_->setText(tr("Import a .dat file as is.  The following options are "
                               "available: \n"
                               "  1. Reverse the order of the points.\n"
                               "  2. Scale the shape.\n"
                               "  3. Rotate the shape.\n"
                               ));
}
