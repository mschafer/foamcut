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
class QCheckBox;

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
	QCheckBox *leLoop_check;

};

#endif // IMPORTWIZARD_H
