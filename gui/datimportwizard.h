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
#ifndef DATIMPORTWIZARD_H
#define DATIMPORTWIZARD_H

#include <QWizard>
#include <Shape.hpp>

namespace foamcut {
	class Shape;
}

class QLabel;
class QRadioButton;
class DatImportPage;
class QCheckBox;

class DatImportWizard : public QWizard
{
    Q_OBJECT
public:
    enum {
        TYPE_PAGE,
        ROOT_IMPORT_PAGE,
		TIP_IMPORT_PAGE
    };

    DatImportWizard(QWidget *parent = 0);

	bool validateCurrentPage();
	std::pair<foamcut::Shape::handle, foamcut::Shape::handle> shapes();
};

class DatTypePage : public QWizardPage
{
    Q_OBJECT

public:
    DatTypePage(QWidget *parent = 0);

private slots:
    void typeChanged(bool checked);

private:
	QLabel *typeLabel_;
	QCheckBox *leLoop_check_;
};

#endif
