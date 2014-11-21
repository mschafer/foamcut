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
#include "datimportwizard.h"
#include "datimportpage.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QDebug>

DatImportWizard::DatImportWizard(QWidget *parent) :
    QWizard(parent)
{
    setPage(TYPE_PAGE, new DatTypePage(this));
	setPage(ROOT_IMPORT_PAGE, new DatImportPage(DatImportPage::ROOT, this));
	setPage(TIP_IMPORT_PAGE, new DatImportPage(DatImportPage::TIP, this));

    setStartId(TYPE_PAGE);

    ///\todo need cancel button on Mac because red x is disabled for some reason
    setOption(NoCancelButton, false);

    setWindowTitle(tr("Import Shapes from DAT files"));
}

bool DatImportWizard::validateCurrentPage()
{
	if (currentId() == TIP_IMPORT_PAGE) {
		auto s = shapes();

		if (!s.first || !s.second) {
			qDebug() << "one of the shapes in null";
			return false;
		}

		if (s.first->segmentCount() != s.second->segmentCount()) {
			qDebug() << "different segment count";
			return false;
		}

		if (s.first->area() * s.second->area() < 0) {
			qDebug() << "areas are different sign";
			return false;
		}

		double lengthRatio = s.first->s().back() / s.second->s().back();
		if (lengthRatio > 1.) lengthRatio = 1. / lengthRatio;
		if (lengthRatio < .5) {
			qDebug() << "taper ration is extreme: " << lengthRatio;
		}
		return true;
	} else {
		return true;
	}
}

std::pair<foamcut::Shape::handle, foamcut::Shape::handle> DatImportWizard::shapes()
{
	std::pair<foamcut::Shape::handle, foamcut::Shape::handle> ret;
	auto rootPage = dynamic_cast<DatImportPage*>(page(ROOT_IMPORT_PAGE));
	auto tipPage = dynamic_cast<DatImportPage*>(page(TIP_IMPORT_PAGE));

	ret.first = rootPage->shape();
	ret.second = tipPage->shape();
	return ret;
}

DatTypePage::DatTypePage(QWidget *parent) : QWizardPage(parent) {

	setTitle(tr("Select DAT file type"));

	QRadioButton *airfoil_radio = new QRadioButton(tr("&Airfoil (from xfoil)"), this);
	QRadioButton *genericDAT_radio = new QRadioButton(tr("&Generic .dat file"), this);

	typeLabel_ = new QLabel("", this);
	typeLabel_->setWordWrap(true);

	leLoop_check_ = new QCheckBox("Leading edge cooling loop", this);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(airfoil_radio);
	layout->addWidget(genericDAT_radio);
	layout->addWidget(typeLabel_);
	setLayout(layout);

	connect(airfoil_radio, SIGNAL(toggled(bool)), this, SLOT(typeChanged(bool)));

	airfoil_radio->setChecked(true);
	registerField("type.airfoil", airfoil_radio);
	registerField("type.leLoop", leLoop_check_);
}

void DatTypePage::typeChanged(bool checked) {
	if (checked) {
		typeLabel_->setText(tr("Importing an airfoil takes the following extra steps\n"
			"  1. Reorder points so top (suction) surface is first.\n"
			"  2. Reflect and translate TE to x=0 and LE to x=chord.\n"
			"  3. Add a synchronization point at the LE.\n"
			"  4. Scale to the specified chord.\n"
			"  5. Rotate to the specified angle of attack.\n"
			"  6. Add optional cooling loop at leading edge."
			));

		layout()->addWidget(leLoop_check_);
	}
	else {
		typeLabel_->setText(tr("Import a generic .dat file as is."));
		layout()->removeWidget(leLoop_check_);
	}
}


